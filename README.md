# MIPSEngine

MIPSEngine is a fully-featured MIPS32 emulator written entirely in C from scratch. Built as an open
educational project, it loads statically-linked MIPS32 ELF binaries, emulates the full
fetch–decode–execute pipeline, and features a pluggable branch predictor. 

Everything-including the memory model, register file, ELF parser, and instruction decoder-is 
implemented bare-metal without external libraries, using only the C standard library.

---

## Architecture

### Memory Model (`src/memory.c`)
A contiguous 64 MB byte array allocated on the heap. Includes explicit big-endian word
access functions (`mem_read_word` and `mem_write_word`). This allows the emulator to safely 
translate memory and run flawlessly on little-endian host machines (like Apple Silicon / ARM and Intel).

### CPU & Register File (`src/cpu.c`)
Implements 32 general-purpose registers ($0–$31), the program counter (PC), and the HI/LO
registers for multiplication and division. Enforces the hardware rule that `$0` (`$zero`)
is permanently hardwired to 0 on the silicon.

### ELF Loader (`src/elf_loader.c`)
Reads 32-bit Big-Endian ELF binaries as raw bytes. Validates magic bytes and architecture
requirements, extracts the `PT_LOAD` segments from the program headers, maps them into
simulated RAM, and sets up the initial stack pointer (`$sp`) and program entry point.

### Decoder & ISA (`src/decoder.c`)
Emulates the MIPS32 Instruction Set Architecture, including:
* **R-Type:** Arithmetic, Logic, Shifts (`sll`, `srl`, `sra`, `sllv`, `srlv`, `srav`), Math (`mult`, `multu`, `div`, `divu`), and Jumps (`jr`, `jalr`).
* **I-Type:** Immediate Math, Logic, and Branching (`beq`, `bne`, `blez`, `bgtz`).
* **J-Type:** Absolute Jumps (`j`, `jal`).
* **REGIMM:** Specialized conditional branches (`bltz`, `bgez`, `bltzal`, `bgezal`).
* **Memory Access:** Word and sub-word access (`lw`, `sw`, `lb`, `lbu`, `lh`, `sh`, `sb`).

### Branch Predictor (`src/predictor.c`)
A 1024-entry hash table for branch prediction, enabled via command-line flags.
* `static`: Always predicts not-taken.
* `1bit`: Remembers the last outcome and predicts it will happen again (prone to thrashing on alternating loops).
* `2bit`: A saturating counter (0-3) that absorbs one mispredict before changing its state, effectively learning loop behaviors.

---

## Build Instructions

To compile the emulator natively on macOS (Apple Silicon or Intel) or Linux:

```bash
make
```

---

## Usage

```bash
./MIPSEngine [--predictor=static|1bit|2bit] [--trace] <elf-binary>
```

* `--trace`: Prints the PC and disassembled instruction for every clock cycle.
* `--predictor`: Enables the branch predictor and prints a statistical accuracy report on exit.

---

## Bare-Metal Testing (Python Generators)

If you don't want to use Docker or a C cross-compiler, you can still test the emulator's core architecture using the included Python scripts. These scripts act as micro-assemblers, manually packing ELF headers and MIPS instructions into raw binary files.

The scripts are located in `tests/generators/`. You can run the entire automated test suite (which generates the binaries and tests memory, branching, syscalls, and math logic) with a single command:

```bash
make test-all
```

Or individual scripts too by:

```bash
make
python3 <filename>.py
./MIPSEngine --trace tests/<filename>.elf
```

---

## Compiling C Programs for MIPSEngine (macOS / Apple Silicon)

MIPSEngine runs bare-metal MIPS binaries. Because macOS does not natively provide a MIPS
cross-compiler, this project uses a Dockerized Ubuntu 22.04 container to cross-compile standard C code
into freestanding MIPS ELF binaries.

**1. Write your C test (e.g., `tests/c_tests/fib.c`)** using the provided `mini_lib.h` for inline assembly syscalls.

**2. Compile using Docker (requires Docker Desktop):**
```bash
docker run --rm --platform linux/amd64 -v $(pwd):/app -w /app ubuntu:22.04 bash -c "apt-get update && apt-get install -y gcc-mips-linux-gnu && mips-linux-gnu-gcc -nostdlib -static tests/c_tests/fib.c -o tests/c_tests/fib.elf"
```

**3. Run the compiled binary:**
```bash
./MIPSEngine --predictor=2bit --trace tests/c_tests/fib.elf
```

---

## Supported Syscalls

The emulator supports a minimal set of SPIM-style system calls to communicate with the host terminal:
| `$v0` | Name           | Effect                                   |
|-------|----------------|------------------------------------------|
| 1     | `print_int`    | Prints `$a0` as a signed decimal integer |
| 4     | `print_string` | Prints null-terminated string at `$a0`   |
| 10    | `exit`         | Safely halts the CPU execution loop      |

---