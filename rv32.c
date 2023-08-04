#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

#include "rv32i.h"
#include "instructions.h"
#include "opcodes.h"

// Reset the HART (zero the registers and PC)
void hart_reset(rv32hart* hart)
{
	for (int i = 0; i < 32; i++)
		hart->x[i] = 0;
	hart->pc = RAM_BASE;
	hart->inst_count = 0;
}

// Clear RAM
void ram_clear(rv32hart* hart)
{
	for (int i = 0; i < RAM_SIZE; i++)
		hart->ram[i] = 0;
}

//Register ABI names
const char* reg_names[] = {
	"zero", "ra ", "sp ", "gp ", "tp ", "t0 ", "t1 ", "t2 ", "s0 ", "s1 ", "a0 ",
	"a1 ", "a2 ", "a3 ", "a4 ", "a5 ", "a6 ", "a7 ", "s2 ", "s3 ", "s4 ", "s5 ",
	"s6 ", "s7 ", "s8 ", "s9 ", "s10", "s11", "t3 ", "t4 ", "t5 ", "t6 "
};

// Print register contents
void hart_print(rv32hart* hart)
{
	printf("\n");
	for (int i = 1; i < 31; i++)
		printf("x%02d (%s): 0x%08x\t%d\n", i, reg_names[i], hart->x[i], hart->x[i]);
	printf("PC:  0x%08x\n", hart->pc);
}

// RAM access
uint8_t ram_read_8(rv32hart* hart, uint32_t addr)
{
	addr -= RAM_BASE;
	return hart->ram[addr];
}

uint16_t ram_read_16(rv32hart* hart, uint32_t addr)
{
	addr -= RAM_BASE;
	uint16_t r = (hart->ram[addr]) | (hart->ram[addr + 1] << 8);
	return r;
}

uint32_t ram_read_32(rv32hart* hart, uint32_t addr)
{
	addr -= RAM_BASE;
	uint32_t r = (hart->ram[addr]) | (hart->ram[addr + 1] << 8) | (hart->ram[addr + 2] << 16) | (hart->ram[addr+3] << 24);
	return r;
}

void ram_store_8(rv32hart* hart, uint32_t addr, uint8_t value)
{
	addr -= RAM_BASE;
	hart->ram[addr] = value;
}

void ram_store_16(rv32hart* hart, uint32_t addr, uint16_t value)
{
	addr -= RAM_BASE;
	hart->ram[addr] = value & 0xff;
	hart->ram[addr+1] = (value >> 8) & 0xff;
}

void ram_store_32(rv32hart* hart, uint32_t addr, uint32_t value)
{
	addr -= RAM_BASE;
	hart->ram[addr] = value & 0xff;
	hart->ram[addr + 1] = (value >> 8) & 0xff;
	hart->ram[addr + 2] = (value >> 16) & 0xff;
	hart->ram[addr + 3] = (value >> 24) & 0xff;
}

// Load program from uint32_t array into memory
void loadProgram(rv32hart* hart, uint32_t program[], int len)
{
	for (int i = 0; i < len; i++)
		ram_store_32(hart, RAM_BASE + (4*i), program[i]);
}

// Execute a single instruction
int rv32_execute(rv32hart* hart)
{
	uint32_t inst = ram_read_32(hart, hart->pc);
	uint8_t opcode = get_opcode(inst);

	int fault = 0;

	if (hart->pc & 0b11 != 0)
		return PC_UNALIGN;

	switch (opcode)
	{

	case OP_IMM:
		fault = exec_op_imm(hart, inst);
		break;
	
	case OP_LUI:
		fault = exec_op_lui(hart, inst);
		break;

	case OP_AUIPC:
		fault = exec_op_auipc(hart, inst);
		break;

	case OP_OP:
		fault = exec_op_op(hart, inst);
		break;

	default:
		fault = UNDEF_OPCODE;
		break;
	}

	hart->pc += 4;
	hart->inst_count++;
	hart->x[0] = 0;
	return fault;

}