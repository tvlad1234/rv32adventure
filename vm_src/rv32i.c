#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

#include "rv32i.h"
#include "instructions.h"
#include "opcodes.h"

// Reset the HART (zero the registers and PC)
void core_reset(rv32core *core)
{
	for (int i = 0; i < 32; i++)
		core->x[i] = 0;
	core->pc = ROM_BASE;
	core->inst_count = 0;
}

// Clear RAM
void ram_clear(rv32core *core)
{
	for (int i = 0; i < RAM_SIZE; i++)
		core->ram[i] = 0;
}

// Register ABI names
const char *reg_names[] = {
	"zero", "ra ", "sp ", "gp ", "tp ", "t0 ", "t1 ", "t2 ", "s0 ", "s1 ", "a0 ",
	"a1 ", "a2 ", "a3 ", "a4 ", "a5 ", "a6 ", "a7 ", "s2 ", "s3 ", "s4 ", "s5 ",
	"s6 ", "s7 ", "s8 ", "s9 ", "s10", "s11", "t3 ", "t4 ", "t5 ", "t6 "};

// Print register contents
void core_print(rv32core *core)
{
	printf("\n");
	for (int i = 1; i < 31; i++)
		printf("x%02d (%s): 0x%08x\t%d\n", i, reg_names[i], core->x[i], core->x[i]);
	printf("PC:  0x%08x\n", core->pc);
}

uint8_t inRAM(uint32_t addr)
{
	if (addr >= RAM_BASE && addr <= RAM_END)
		return 1;
	return 0;
}

uint8_t inROM(uint32_t addr)
{
	if (addr >= ROM_BASE && addr <= ROM_END)
		return 1;
	return 0;
}

uint8_t inMemory(uint32_t addr)
{
	if ((addr >= RAM_BASE && addr <= RAM_END) || (addr >= ROM_BASE && addr <= ROM_END))
		return 1;
	return 0;
}

uint32_t getMemBase(uint32_t addr)
{
	if (inRAM(addr))
		return RAM_BASE;

	if (inROM(addr))
		return ROM_BASE;

	return 0;
}

uint8_t *memoryPointer(rv32core *core, uint32_t addr)
{
	if (inRAM(addr))
		return core->ram;

	if (inROM(addr))
		return core->rom;

	return 0;
}

// Memory access
uint8_t mem_read_8(rv32core *core, uint32_t addr)
{
	uint8_t *memPtr = memoryPointer(core, addr);
	addr -= getMemBase(addr);
	return memPtr[addr];
}

uint16_t mem_read_16(rv32core *core, uint32_t addr)
{
	uint8_t *memPtr = memoryPointer(core, addr);
	addr -= getMemBase(addr);
	uint16_t r = (memPtr[addr]) | (memPtr[addr + 1] << 8);
	return r;
}

uint32_t mem_read_32(rv32core *core, uint32_t addr)
{
	uint8_t *memPtr = memoryPointer(core, addr);
	addr -= getMemBase(addr);
	uint32_t r = (memPtr[addr]) | (memPtr[addr + 1] << 8) | (memPtr[addr + 2] << 16) | (memPtr[addr + 3] << 24);
	return r;
}

void mem_store_8(rv32core *core, uint32_t addr, uint8_t value)
{
	uint8_t *memPtr = memoryPointer(core, addr);
	addr -= getMemBase(addr);
	memPtr[addr] = value;
}

void mem_store_16(rv32core *core, uint32_t addr, uint16_t value)
{
	uint8_t *memPtr = memoryPointer(core, addr);
	addr -= getMemBase(addr);
	memPtr[addr] = value & 0xff;
	memPtr[addr + 1] = (value >> 8) & 0xff;
}

void mem_store_32(rv32core *core, uint32_t addr, uint32_t value)
{
	uint8_t *memPtr = memoryPointer(core, addr);
	addr -= getMemBase(addr);
	memPtr[addr] = value & 0xff;
	memPtr[addr + 1] = (value >> 8) & 0xff;
	memPtr[addr + 2] = (value >> 16) & 0xff;
	memPtr[addr + 3] = (value >> 24) & 0xff;
}

// Load program from uint32_t array into memory
void loadProgram(rv32core *core, uint32_t program[], int len)
{
	for (int i = 0; i < len; i++)
		mem_store_32(core, ROM_BASE + (4 * i), program[i]);
}

// MMIO reads
uint32_t mmio_load(uint32_t addr)
{
	return 0xdeadbeef;
}

int mmio_store(uint32_t addr, uint32_t val)
{
	if (addr == 0x11100000) // SYSCON
	{
		if (val == 0x5555) // POWEROFF
			return SYSCON_SHUTDOWN;
	}
	else if (addr == 0x10000000) // UART
	{
		printf("%c", val);
	}
	return 0;
}

// Execute a single instruction
int rv32_execute(rv32core *core)
{
	int fault = 0;

	if (core->pc & 0b11 != 0)
		return PC_UNALIGN;

	if (!inMemory(core->pc))
		return PC_OUT_OF_RANGE;

	uint32_t inst = mem_read_32(core, core->pc);
	uint8_t opcode = get_opcode(inst);

	switch (opcode)
	{

	case OP_IMM:
		fault = exec_op_imm(core, inst);
		break;

	case OP_LUI:
		fault = exec_op_lui(core, inst);
		break;

	case OP_AUIPC:
		fault = exec_op_auipc(core, inst);
		break;

	case OP_OP:
		fault = exec_op_op(core, inst);
		break;

	case OP_JAL:
		fault = exec_op_jal(core, inst);
		break;

	case OP_JALR:
		fault = exec_op_jalr(core, inst);
		break;

	case OP_BRANCH:
		fault = exec_op_branch(core, inst);
		break;

	case OP_LOAD:
		fault = exec_op_load(core, inst);
		break;

	case OP_STORE:
		fault = exec_op_store(core, inst);
		break;

	default:
		fault = UNDEF_OPCODE;
		break;
	}

	core->pc += 4;
	core->inst_count++;
	core->x[0] = 0;
	return fault;
}