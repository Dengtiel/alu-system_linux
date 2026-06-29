# nm_objdump

A custom implementation of the GNU `nm` command for ELF files.

## Description

`hnm` reproduces the behavior of `nm -p`, displaying symbol table information
from ELF object files. It supports:

- 32-bit and 64-bit ELF files
- Little-endian and big-endian byte order
- All ELF file types (executables, shared libraries, relocatable objects)

## Compilation
