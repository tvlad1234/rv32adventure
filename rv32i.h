#pragma once

#include <stdint.h>

//Where RAM lives
#define RAM_BASE 0x80000000

// How much RAM we have (in bytes)
#define RAM_SIZE 8192

// RAM End
#define RAM_END (RAM_BASE + RAM_SIZE)

// Errors
#define UNDEF_OPCODE -1
#define UNDEF_MICROOP -2
#define PC_UNALIGN -3

// RISC-V harts are basically CPU cores (execution contexts)
struct rv32hart {
	uint32_t x[32]; //32 registers
	uint32_t pc;

	uint8_t ram[RAM_SIZE];
	
	uint64_t inst_count;
};
typedef struct rv32hart rv32hart;

void ram_clear(rv32hart* hart);

void hart_reset(rv32hart* hart);
void hart_print(rv32hart* hart);

void ram_store_8(rv32hart* hart, uint32_t addr, uint8_t value);
void ram_store_16(rv32hart* hart, uint32_t addr, uint16_t value);
void ram_store_32(rv32hart* hart, uint32_t addr, uint32_t value);
uint8_t ram_read_8(rv32hart* hart, uint32_t addr);
uint16_t ram_read_16(rv32hart* hart, uint32_t addr);
uint32_t ram_read_32(rv32hart* hart, uint32_t addr);

void loadProgram(rv32hart* hart, uint32_t program[], int len);

int rv32_execute(rv32hart* hart);