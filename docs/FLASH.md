# BeagleBone Black — Flash & Dev Environment Runbook

**Purpose:** Reproduce the full BBB setup from scratch (SD flash → boot → SSH → cross-compile kernel + modules). Written after the 2026-09-20 session where a corrupt SD card forced a full rebuild and the steps had been forgotten. Follow this next time instead of re-deriving.

**Board:** BeagleBone Black (Cortex-A8, ARMv7 32-bit, hard-float, eMMC + microSD)
**Host:** Pop!_OS (x86_64), user `mhle`, tree at `~/Learning/BBB/`
**Current target kernel:** `6.18.52-bone54` (must match across board + build tree)

---

## 0. Tree layout (what lives where)

```
~/Learning/BBB/
├── bb-kernel/          Robert Nelson ti-linux-kernel build scripts (git repo)
│   ├── KERNEL/         the actual kernel source (nested git repo, built here)
│   ├── dl/             toolchains auto-downloaded by build_kernel.sh
│   ├── deploy/         build outputs (zImage, modules.tar.gz, dtbs.tar.gz)
│   ├── version.sh      pins kernel version + toolchain for the checked-out tag
│   └── build_kernel.sh main build script
├── toolchain/          manually-downloaded toolchains (ARM GNU, Linaro) — NOT used
│                       for kernel build; bb-kernel uses its own in dl/
├── rootfs/             debian rootfs tarball (for manual SD assembly — not used in
│                       the fast path below)
├── u-boot/             MLO + u-boot.img (for manual SD assembly — not used fast path)
├── images/             downloaded official BBB .img.xz files
└── kmod/               kernel module projects
    ├── hello_world/    first module (hello.c + Makefile)
    ├── publicDriver/   built .ko files land here (Makefile copies them)
    └── scull/          LDD3 scull driver
```

---

## 1. Flash a bootable SD card (FAST PATH — official image)

This is the path we use for module work. (Manual assembly from bb-kernel parts =
Week 9 boot-chain material; not needed just to run modules.)

**1a. Get the official image**
Download the BeagleBone Black AM335x Debian image (IoT/base, non-graphical) from
https://www.beagleboard.org/distros into `~/Learning/BBB/images/`.
Latest used: `am335x-debian-13.7-base-v6.18-armhf-2026-09-15-4gb.img.xz` (kernel v6.18, U-Boot 2022.04).

**1b. Identify the SD card device node — CAREFULLY (this step destroys disks if wrong)**
```bash
# BEFORE inserting the card:
lsblk -d -o NAME,SIZE,TYPE,MODEL          # baseline
# INSERT card, then:
lsblk                                      # the NEW entry = the card
dmesg | tail                               # confirms which [sdX] just appeared
```
The SD card is the device with **RM=1** (removable) and ~29.2G (for a 32GB card).
On this host it came up as **/dev/sdc**. NEVER write to sda/sdb/nvme (those are the system).

**1c. Unmount, then flash**
```bash
sudo umount /dev/sdc1                       # unmount (do NOT eject)
lsblk /dev/sdc                              # confirm MOUNTPOINTS empty
# WRITE (triple-check of=/dev/sdc, the WHOLE disk, not a partition):
xzcat ~/Learning/BBB/images/am335x-debian-13.7-base-v6.18-armhf-2026-09-15-4gb.img.xz | \
  sudo dd of=/dev/sdc bs=4M conv=fsync status=progress
sync
lsblk /dev/sdc                              # should now show new partition layout
```

**1d. Set username/password BEFORE first boot (image has no default password)**
```bash
sudo mkdir -p /mnt/bbb-boot
sudo mount /dev/sdc1 /mnt/bbb-boot          # sdc1 = FAT boot partition
sudo nano /mnt/bbb-boot/sysconf.txt
```
Uncomment + set (leave root_password commented = root login disabled):
```
user_name=debian            # keep 'debian' — distinct from host user 'mhle'
user_password=<password>    # note: regenerated/wiped after first boot
hostname=bbb
usb_enable_dhcp=yes         # gives SSH over USB gadget as a fallback
```
```bash
sudo umount /mnt/bbb-boot
sudo eject /dev/sdc
```

---

## 2. Boot the board (S2 override is mandatory on BBB Black)

BBB Black boots eMMC before SD by default. To boot from SD you MUST hold S2.

1. Insert SD into board. Connect USB-serial (FTDI) to J1: GND→pin1 (dot corner),
   FTDI RX→pin4, FTDI TX→pin5. **Do NOT connect FTDI 3.3V.**
2. Open serial console on host: `sudo minicom -D /dev/ttyUSB0 -b 115200`
   (or `sudo minicom -s` once to set 115200 8N1, flow control OFF, save as dfl).
3. Power the board OFF: hold **S3** (power button) 8-10s until all LEDs dark.
4. **Hold S2** (button by microSD slot) → tap **S3** to power on → keep holding S2 ~5s → release.
5. Watch serial: U-Boot → kernel → first-boot sysconf runs → **auto-reboots once** →
   `bbb login:`. Log in as `debian` / <password>.

**Why S2:** the AM335x ROM checks eMMC before SD; S2 pulls a boot-config pin to
check SD first. #1 cause of "flashed card won't boot" = forgetting S2.

---

## 3. Networking + SSH

**Ethernet DHCP quirk:** if the board boots with the cable already in, it sometimes
misses the DHCP window (`ip addr show eth0` shows no IPv4, only inet6).
**Fix:** boot with ethernet UNPLUGGED, then plug in after fully booted. Or
`sudo dhclient eth0` after link is up. (Root cause: DHCP request races link-up.)

**Set up ssh bbb (host side, one-time):**
```bash
# ~/.ssh/config on the HOST:
Host bbb
    HostName bbb.local          # mDNS — survives DHCP IP changes
    User debian
# then:
chmod 600 ~/.ssh/config
# passwordless login:
ssh-keygen -t ed25519           # if you don't have a key (Enter through prompts)
ssh-copy-id bbb                  # installs pubkey (asks board password once)
ssh bbb 'uname -r'              # verify: prints 6.18.52-bone54, no password
```
Board IP last seen: 192.168.1.164 (DHCP). `bbb.local` resolves via mDNS.
If mDNS breaks: `ssh debian@<ip>`, or `sudo systemctl restart avahi-daemon` on host.

---

## 4. Cross-compile the kernel (needed to build modules that load)

**Why:** modules must be built against a kernel tree matching the board's running
kernel EXACTLY (vermagic / "Match #2"). The bb-kernel source tree, checked out to
the matching tag and built, is the clean provider. (The linux-headers .deb is a
stripped tree — missing scripts/kconfig, ships ARM-built host tools that fail with
"fixdep: Exec format error" when cross-compiling on x86. Don't use it; build the
source tree instead.)

```bash
cd ~/Learning/BBB/bb-kernel
git status --short                          # untracked .vscode/ is fine
git checkout 6.18.52-bone54                 # the tag matching the board (detached HEAD is normal)
grep '^toolchain=' version.sh               # shows gcc_15_arm (release pins the compiler)
./build_kernel.sh                           # auto-downloads GCC15 into dl/, builds (~30-60min)
```
Notes on build_kernel.sh:
- It downloads the pinned toolchain (`x86_64-gcc-15.3.0-nolibc-arm-linux-gnueabi`)
  from rcn-ee.net into `dl/`. Download can be SLOW (KB/s). **Do NOT Ctrl-C** — a
  partial file causes "xz: Unexpected end of input" on re-run. If that happens:
  `rm dl/x86_64-gcc-15.3.0-nolibc-arm-linux-gnueabi.tar.xz` and re-run.
- May pause at a `menuconfig` TUI: change nothing, Exit, Save = Yes.
- Success builds a full KERNEL/ tree WITH x86 host tools (fixdep/modpost compiled
  for the host) → no "Exec format error", plus fresh deploy/ artifacts.

Result: `~/Learning/BBB/bb-kernel/KERNEL/` = build-ready tree for modules.
Toolchain prefix: `~/Learning/BBB/bb-kernel/dl/gcc-15.3.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-`

---

## 5. Build + load a module (the daily loop)

**Module Makefile** (`kmod/hello_world/Makefile`) — the two paths that matter:
```makefile
ifneq (${KERNELRELEASE},)
    obj-m += hello.o
else
    KERNEL_DIR := $(PWD)/../../bb-kernel/KERNEL                                              # Match #2 (kernel)
    CC := $(PWD)/../../bb-kernel/dl/gcc-15.3.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-   # Match #1 (toolchain)
    CORES=4
    PUBLIC_DRIVER_DIR=$(PWD)/../publicDriver
default:
	${MAKE} ARCH=arm CROSS_COMPILE=${CC} -C ${KERNEL_DIR} M=${PWD} -j${CORES} modules
	mkdir -p ${PUBLIC_DRIVER_DIR}
	cp *.ko ${PUBLIC_DRIVER_DIR}
all: default
clean:
	${MAKE} -C ${KERNEL_DIR} M=${PWD} clean
endif
```
(Recipe lines must be real TABs. `clean:` was typo'd `clear:` in the old file — harmless.)

**The loop:**
```bash
cd ~/Learning/BBB/kmod/hello_world
make                                        # builds hello.ko, copies to ../publicDriver/

# verify BEFORE loading:
modinfo publicDriver/hello.ko | grep -E 'vermagic|license'   # vermagic MUST = 6.18.52-bone54
file publicDriver/hello.ko                                    # MUST say ARM 32-bit

# deploy + load + check + unload (from host):
scp ~/Learning/BBB/kmod/publicDriver/hello.ko bbb:~/
ssh bbb 'sudo insmod hello.ko && dmesg | tail -3 && lsmod | grep hello && sudo rmmod hello && dmesg | tail -1'
```
Expected: "hello: module loaded on BBB" (insmod→hello_init), lsmod shows it (refcount 0),
"hello: module unloaded" (rmmod→hello_exit). ✓ verified working 2026-09-20.

---

## 6. Quick reference

| Thing | Value |
|---|---|
| Board kernel | `6.18.52-bone54` (`uname -r`) |
| Board login | `debian` / (your password) @ `bbb.local` / 192.168.1.164 |
| Serial | `/dev/ttyUSB0`, 115200 8N1, minicom, flow control OFF |
| SSH | `ssh bbb` (passwordless, via ~/.ssh/config alias) |
| SD device on host | `/dev/sdc` (RM=1, ~29.2G) — VERIFY each time |
| Kernel tree (KDIR) | `~/Learning/BBB/bb-kernel/KERNEL` |
| Toolchain prefix | `~/Learning/BBB/bb-kernel/dl/gcc-15.3.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-` |
| Modules output | `~/Learning/BBB/kmod/publicDriver/` |
| Boot from SD | hold **S2** while powering on |
| eMMC vs SD | root on `mmcblk0p3` = SD; `mmcblk1` = eMMC |

**Golden rules:**
- `dd` target = `/dev/sdc` (verify with lsblk RM=1 every time). Wrong node = wiped disk.
- Never Ctrl-C the toolchain download (corrupts the file).
- Module vermagic must equal board `uname -r`, or insmod rejects it.
- Boot from SD needs S2 held; ethernet DHCP wants cable plugged AFTER boot.
