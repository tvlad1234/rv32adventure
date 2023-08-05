# rv32adventure
Writing a RV32I emulator (work in progress)

## Project goal
The goal of this project is to learn how the RISC-V architecture works, by writing an emulator. So far, this project emulates the RV32I instruction set, with work on the M extension being in progress and Zicsr planned.

## What it provides
This repository provides the source code of the emulator (in the [vm_src](vm_src) folder), as well as an [example C program](rv_app_src/main.c) which can be compiled and ran on the emulator. 

## How to use
Both the emulator and example program are build by running `make`. To build and run the program inside the emulator, run `make test`. The compiled program binary will be called _rv_app.bin_. The program filename is passed to the emulator as a command line argument (`emulator [filename]`). In order to compile the program, `riscv64-unkown-elf-gcc` must be available.

## Credits
[The RISC-V Instruction Set Manual](https://github.com/riscv/riscv-isa-manual/releases/download/Ratified-IMAFDQC/riscv-spec-20191213.pdf) has been the main source of documentation when developing the emulator. \
The very small libc provided in the RISC-V example program [barelibc.c](rv_app_src/barelibc.c) is heavily based on [ch32v003fun.c by cnlohr](https://github.com/cnlohr/ch32v003fun/blob/master/ch32v003fun/ch32v003fun.c)

