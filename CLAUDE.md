# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Embedded Linux learning and development repository targeting the **BeagleBone Black (BBB)** — an ARM Cortex-A8 board (AM3358 SoC). The repo covers the full software stack: bootloader (U-Boot), Linux kernel 5.4.242, loadable kernel modules (character drivers), and user-space IPC applications.

All application and kernel module code is **cross-compiled** on x86_64 host and deployed to the BBB.

## Build Commands

### Kernel

```bash
cd bb-kernel
./build_kernel.sh        # Full kernel build (downloads linux-stable on first run)
./tools/rebuild.sh       # Incremental rebuild after source changes
./build_deb.sh           # Produce installable .deb package
```

Configure via `bb-kernel/system.sh` (copy from `system.sh.sample`): set `CC` (cross-compiler), `CORES`, `MMC` (target device).

### Applications (CMake, cross-compiled)

```bash
# Any single app:
cd app/<appname>
mkdir -p build && cd build
cmake ..
make

# ipcSocket multi-target:
cd app/ipcSocket
./masterbuild.sh -b all           # Build all targets
./masterbuild.sh -b server -c     # Clean build, server only
./masterbuild.sh -v -b client     # Verbose
```

Scaffold a new app:
```bash
cd app && ./createApp.sh <appname>
```

### Kernel Modules

```bash
cd kmod/<module>
make        # Produces .ko in kmod/publicDriver/
```

## Architecture

### Software Stack

```
U-Boot (bootloader)         → initializes HW, loads kernel + DTB
Linux 5.4.242-bone66        → BeagleBone-patched kernel
Debian 10.10 minimal armhf  → root filesystem (rootfs/)
User-space apps (app/)      → IPC demos, debug tools
```

### Cross-Compilation

- **Primary toolchain:** `toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/`
- The toolchain path is **hardcoded** in `app/ipcSocket/Toolchain/beaglebone_black.toolchain.cmake` and each app's `CMakeLists.txt` — not resolved from `PATH`.
- Kernel and kmod builds pass `CROSS_COMPILE=arm-linux-gnueabi-` to Make.

### Applications (`app/`)

| Directory | Mechanism | Notes |
|-----------|-----------|-------|
| `hello_world/` | — | Minimal C demo |
| `ptrace/` | ptrace syscall | Debugger/tracer |
| `ipcMsg/` | POSIX message queues | Server + client pair |
| `ipcShrMem/` | Shared memory | CMake only, no sources yet |
| `ipcSemaphore/` | POSIX semaphores | Semaphore as mutex |
| `ipcSocket/` | TCP sockets + auth | Most complex; see below |

**ipcSocket** has a shared library (`lib/`) consumed by server, client, and tool:
- `encodeHandler.cpp` — SHA256 via OpenSSL
- `csvHandler.cpp` — CSV credential storage
- `fileHandler.cpp` — file I/O helpers
- `remoteHaShaHandler.cpp` — remote hash authentication

The server is multi-threaded (pthreads) and authenticates clients against a CSV user database. Passwords are stored as SHA256 hashes. The `tool/` binary manages that database interactively.

### Kernel Modules (`kmod/`)

- **hello_world** — minimal `printk` module with parameters (`whom`, `iHowMany`)
- **scull** — character device driver with linked-list quantum-set storage, configurable `quantum` (4000 B) and `qset` (1000 items), semaphore-protected, supports 4 minor devices (`/dev/scull0–3`)

Compiled `.ko` files are copied to `kmod/publicDriver/`.

### Kernel Build System (`bb-kernel/`)

- `build_kernel.sh` orchestrates source fetch, patch, configure, compile
- Default config: `omap2plus_defconfig`
- Patches live in `bb-kernel/patches/`
- Build output (zImage, DTBs, config) goes to `bb-kernel/deploy/`
- `KERNEL/` subdirectory is the Linux source tree (git submodule)
