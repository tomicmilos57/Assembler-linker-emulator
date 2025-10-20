## Project overview

This repository implements a small toolchain for a simple 32-bit educational CPU: an assembler, a linker and an emulator. The project is split into three major parts under `src/`:

- `src/assembler/` — the assembler front-end that parses assembly input and produces a textual object format (section dumps, symbol table and relocations).
- `src/linker/` — a simple linker that reads the assembler's textual object files, merges sections, resolves relocations and produces a flattened binary-like hex dump.
- `src/emulator/` — an instruction-level emulator that can load the linker's hex dump and execute the CPU instruction set.

The codebase is intentionally compact and readable: the assembler emits a human-readable object description (not a binary ELF), and the linker/emulator consume that textual format.

## High-level architecture

Assembler
- Purpose: translate assembly source (.s) into a textual object file containing: a symbol table, per-section byte arrays, and relocation tables.
- Key components:
  - `classes.hpp` / `classes.cpp` — core data structures used by the assembler: `Section`, `Sections`, `SymbolTable`, `FillTable`, and related helpers. Responsible for storing section bytes and relocations, symbol entries, literal tables used for delayed patching and producing textual output.
  - `defs.hpp` — global constants and instruction opcode definitions, register names and some helper prototypes used across assembler modules.
  - `directives.cpp` — implements assembler directives: `.global`, `.extern`, `.section`, `.word`, `.skip`, `.ascii`, `.equ`, labels and finishing actions. These routines manipulate the current `Sections` and `SymbolTable`.
  - `instructions.cpp` — implements register/csr parsing (`get_reg`, `get_csr`) and many instruction helpers such as arithmetic, push/pop, iret/ret, etc. It contains `insert_instruction` which composes the 32-bit instruction word.
  - `jumps.cpp` — implements branch and call directives that require deferred resolution (symbols/literals), using the `FillTable` to create placeholder instructions and add relocation entries.
  - `ldst.cpp` — load/store helpers and addressing modes; some operations create literal/symbol fill entries when immediate offsets or addresses are required.
  - `main.cpp` (assembler) — parser glue (uses `yyparse()`), global singletons (`sections`, `symtable`, `filltable`) and final assembly steps: finalize sections, apply fill table patches, compute `.equ` values, and write the textual `.o` file.

Linker
- Purpose: read the assembler's textual object files, combine multiple object inputs, lay out sections in memory, resolve relocation entries, and emit a final flattened textual binary representation.
- Key points (single file `src/linker/main.cpp`):
  - The linker implements a `Sections` parser that understands the assembler's textual format: a `#.symtab` block and per-section `#.section` blocks plus `#.rela.section` relocations.
  - `SymbolTable::from_string` and `Sections::parse` read and reconstruct the symbol table, section byte arrays and relocation entries.
  - `Sections::merge` merges multiple object files' sections and relocation tables into a single `Sections` instance.
  - Placement: `place_section` and `place_all_sections` compute section addresses; `resolve_relocations` writes final 32-bit values into section byte arrays using the symbol table.
  - Output: `Sections::binary()` produces a textual address-prefixed byte dump (address: byte byte ...), suitable for the emulator loader.

Emulator
- Purpose: emulate the target CPU: fetch-decode-execute on the textual hex dump produced by linker.
- Components:
  - `cpu.hpp` / `cpu.cpp` — `CPU` class implements register file (16 general registers), CSR registers (status, handler, cause), IR, instruction counter, a sparse memory (std::map<uint32_t,uint8_t>), instruction decoding and execution semantics (fetch, decode, execute, push/pop, interrupt handling). The instruction decoder extracts OC (4 bits), MOD (4 bits), REGA/REGB/REGC and DISP (12-bit signed offset).
  - `console.hpp` / `console.cpp` — terminal input helpers: non-blocking stdin reads using POSIX `termios` and `read`. Input characters are delivered to the CPU by setting an interrupt (keyboard) and writing the value into a memory-mapped location.
  - `main.cpp` (emulator) — simple loop that simulates console input and timer, then repeatedly calls `cpu.execute()` until HALT.

## Instruction encoding (summary)

Each instruction is encoded in a 32-bit word comprised of fields (bit positions shown approximately):

- OC: 4 bits (bits 31..28) — major opcode group
- MOD: 4 bits (bits 27..24) — sub-opcode / addressing mode
- REGA: 4 bits (bits 23..20) — register A index
- REGB: 4 bits (bits 19..16) — register B index
- REGC: 4 bits (bits 15..12) — register C index
- DISP: 12 bits (bits 11..0) — immediate/displacement as a signed 12-bit value

The assembler composes this with shifts and masks and the emulator decodes it the same way. `defs.hpp` lists the numerical opcode constants used by the assembler helpers.

## Textual object file format (assembler output)

The assembler writes a textual object file (e.g. `foo.o`) containing three types of sections:

- `#.symtab` — human-readable symbol table where each non-empty line encodes: index: <hex-value> <found> <LOC|GLOB> <symbol> <section-name?> <equ>. This is used by the linker to reconstruct symbol definitions and attributes.
- `#.<section_name>` — hex bytes of the section, grouped by line (16 bytes per line), representing section contents.
- `#.rela.<section_name>` — relocation entries: lines with `<offset> <symbol> <addend> <section_name>`. Offsets are relative to the beginning of the section. The linker uses these to compute absolute addresses after section placement.

The linker's `Sections::binary()` flattens placed sections into an address-prefixed hex dump (lines like `00400000: 12 34 56 78 ...`) which is what the emulator's CPU loader expects.

## Key data structures (assembler)

- Section: fixed-size array (4 KiB) for bytes, `offset` write pointer, `list_of_relocations` and `list_of_literals` for literal placeholders.
- Sections: container of `Section*` with helpers to add/get current section and finalize the assembly output.
- symbolTableEntry / SymbolTable: stores symbol name, value, binding (local/global), section name and whether it originates from an `.equ` expression.
- FillTable / toFill: stores pending patches for instructions that reference symbols or large literals. During a finalization phase the FillTable patches instructions and, if necessary, inserts relocation entries.

## Relocations and `.equ`

- The assembler creates relocation entries when a symbol's address must be fixed later by the linker. For symbol-referencing instructions the assembler either emits a relocation or leaves a placeholder literal and records an entry in `FillTable`.
- `.equ` expressions are tracked as operand lists in the assembler (`SymbolTable::equ`). After parsing all source, `SymbolTable::finnishAssembly()` evaluates these expressions by classifying operands as integer, label, or negative label and resolving them against the symbol map.

## Emulator details and memory model

- Registers: 16 general-purpose 32-bit registers. R14 is SP, R15 is PC. Register 0 is hardwired to zero.
- CSRs: small control registers (status, handler, cause) used for interrupts and system behavior.
- Memory: implemented as a sparse map (std::map/unordered_map) from 32-bit address to byte. This allows loading sparse address ranges.
- I/O: the emulator treats certain memory addresses as I/O; the console writes characters to a special MMIO address (e.g., 0xFFFFFF00) and keyboard/timer interrupts are signalled via CSRs and CPU flags.

## Limitations and notable implementation details

- Sections have a hard-coded 4 KiB fixed buffer in the assembler; large sections may overflow in current design.
- The assembler and linker use a textual object file format which is easier to inspect and debug but not an industry-standard object format (no ELF/COFF).
- The emulator's CPU pop/push and fetch/store assume little-endian 32-bit words.
- Error handling is minimal in places: some unsupported operations call `exit()` with magic codes.

## Tests and example inputs

- The `tests/` directory contains subfolders (nivo-a, nivo-b, nivo-c) with example assembly (`.s`) sources and various ISR/handler files used to exercise features.

## Developer notes

- If you need to extend instruction encodings, ensure both the assembler helpers (in `instructions.cpp`, `jumps.cpp`, `ldst.cpp`) and the emulator decode/execute logic (`cpu.cpp`) remain in sync.
- The textual object format is intentionally simple: changes to the format require updating both the assembler writer (`Section::..._to_string`) and the linker's parser (`Sections::parse`).
