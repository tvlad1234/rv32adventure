#pragma once

#include <stdint.h>

// Where RAM lives
#define RAM_BASE 0x20000000

// Where ROM lives
#define ROM_BASE 0x80000000

// How much RAM we have (in bytes)
#define RAM_SIZE (1024 * 2)

// How much ROM we have (in bytes)
#define ROM_SIZE (1024 * 16)

// RAM End
#define RAM_END (RAM_BASE + RAM_SIZE)

// ROM End
#define ROM_END (ROM_BASE + ROM_SIZE)

// Errors
#define UNDEF_OPCODE -1
#define UNDEF_FUNC3 -2
#define UNDEF_FUNC7 -3
#define PC_UNALIGN -4
#define PC_OUT_OF_RANGE -5
#define SYSCON_SHUTDOWN -6
#define WRITE_ROM -7

// RISC-V 32bit core
struct rv32core
{
	uint32_t x[32]; // 32 registers
	uint32_t pc;

	uint8_t ram[RAM_SIZE];
	uint8_t rom[ROM_SIZE];

	uint64_t inst_count;
};
typedef struct rv32core rv32core;

void ram_clear(rv32core *core);

void core_reset(rv32core *core);
void core_print(rv32core *core);

uint8_t inMemory(uint32_t addr);
uint8_t inROM(uint32_t addr);

void mem_store_8(rv32core *core, uint32_t addr, uint8_t value);
void mem_store_16(rv32core *core, uint32_t addr, uint16_t value);
void mem_store_32(rv32core *core, uint32_t addr, uint32_t value);
uint8_t mem_read_8(rv32core *core, uint32_t addr);
uint16_t mem_read_16(rv32core *core, uint32_t addr);
uint32_t mem_read_32(rv32core *core, uint32_t addr);

void loadProgram(rv32core *core, uint32_t program[], int len);

uint32_t mmio_load(uint32_t addr);
int mmio_store(uint32_t addr, uint32_t val);

int rv32_execute(rv32core *core);