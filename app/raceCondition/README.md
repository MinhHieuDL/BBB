# Race Condition Demo

Demonstrates a classic race condition on a shared counter incremented by two threads, and fixes it using C11 `stdatomic`.

## The Problem

Two threads each increment a shared `long counter` 100,000 times. The expected result is **200,000**. Without synchronization, the actual result is unpredictable.

```
Expected: 200000
Actual:   109434   ← lost updates due to race
```

### Why it happens: Read-Modify-Write

The C expression `counter++` is not atomic. The compiler translates it into three separate machine instructions:

```asm
; Buggy: commit b6b613d
; counter++ expands into three distinct steps

mov rax, QWORD PTR [rip+0x2e53]   ; (1) LOAD  – read counter from memory into register
add rax, 0x1                       ; (2) ADD   – increment the register
mov QWORD PTR [rip+0x2e48], rax   ; (3) STORE – write register back to memory
```

Each step is a separate memory access. The OS scheduler can preempt a thread between any two of them. When that happens, another thread reads the **stale** value before the first thread's write lands:

```
Time │  Thread 1                          Thread 2
─────┼────────────────────────────────────────────────────
  1  │  LOAD  counter → rax  (reads 5)
  2  │                                LOAD  counter → rax  (also reads 5, stale!)
  3  │  ADD   rax = 6
  4  │                                ADD   rax = 6
  5  │  STORE rax → counter  (writes 6)
  6  │                                STORE rax → counter  (writes 6, overwrites!)
     │
     │  counter is 6 — one increment was lost
```

Both threads read the same value, both compute `+1`, both write the same result. One increment is silently discarded.

---

## The Fix

Use `atomic_long` and `atomic_fetch_add` from `<stdatomic.h>` (C11):

```c
// Before (buggy)
long counter = 0;
counter++;

// After (fixed)
#include <stdatomic.h>
atomic_long counter = 0;
atomic_fetch_add(&counter, 1);
```

### What changes in the assembly

```asm
; Fixed: commit 36d799c
; atomic_fetch_add compiles to a single locked instruction

lock add QWORD PTR [rip+0x2e51], 0x1   ; atomic read-modify-write in one instruction
```

The `lock` prefix is the key. It tells the CPU to assert exclusive ownership of the cache line for the duration of the `add`. No other core can read or write that memory location until the operation completes. The entire read-modify-write is now **one indivisible step** — there is no window for another thread to observe a stale value.

### Side-by-side comparison

| | Buggy (`counter++`) | Fixed (`atomic_fetch_add`) |
|---|---|---|
| Instructions | 3 (`mov` / `add` / `mov`) | 1 (`lock add`) |
| Atomic | No | Yes |
| Interruptible between steps | Yes | No |
| Result (200,000 expected) | Non-deterministic | Always correct |

### Full disassembly of `worker()`

**Buggy binary** (`b6b613d`):
```asm
00000000000011a9 <worker>:
    11a9:  endbr64
    11ad:  push   rbp
    11ae:  mov    rbp,rsp
    11b1:  mov    QWORD PTR [rbp-0x18],rdi
    11b5:  mov    DWORD PTR [rbp-0x4],0x0
    11bc:  jmp    11d4 <worker+0x2b>
    11be:  mov    rax,QWORD PTR [rip+0x2e53]       ; LOAD  counter
    11c5:  add    rax,0x1                           ; ADD   (register only)
    11c9:  mov    QWORD PTR [rip+0x2e48],rax        ; STORE counter  ← race window
    11d0:  add    DWORD PTR [rbp-0x4],0x1
    11d4:  cmp    DWORD PTR [rbp-0x4],0x1869f
    11db:  jle    11be <worker+0x15>
    11dd:  mov    eax,0x0
    11e2:  pop    rbp
    11e3:  ret
```

**Fixed binary** (`36d799c`):
```asm
00000000000011a9 <worker>:
    11a9:  endbr64
    11ad:  push   rbp
    11ae:  mov    rbp,rsp
    11b1:  mov    QWORD PTR [rbp-0x18],rdi
    11b5:  mov    DWORD PTR [rbp-0x4],0x0
    11bc:  jmp    11cb <worker+0x22>
    11be:  lock add QWORD PTR [rip+0x2e51],0x1      ; atomic RMW — no race possible
    11c7:  add    DWORD PTR [rbp-0x4],0x1
    11cb:  cmp    DWORD PTR [rbp-0x4],0x1869f
    11d2:  jle    11be <worker+0x15>
    11d4:  mov    eax,0x0
    11d9:  pop    rbp
    11da:  ret
```

The loop body shrinks from **3 instructions** (lines `11be`–`11c9`) to **1 instruction** (line `11be`). The separate load and store are gone entirely — the CPU performs the whole operation directly in memory under the bus lock.

---

## Build & Run

```bash
mkdir -p build && cd build
cmake ..
make
./bin/race
```

## Reproduce the objdump

```bash
# Buggy version — check out original commit, build, disassemble
git stash
git checkout b6b613d -- src/race.c
make -C build
objdump -d -M intel --no-show-raw-insn build/bin/race | awk '/^[0-9a-f]+ <worker>:$/,/^$/'

# Restore fixed version
git checkout HEAD -- src/race.c
make -C build
objdump -d -M intel --no-show-raw-insn build/bin/race | awk '/^[0-9a-f]+ <worker>:$/,/^$/'
```
