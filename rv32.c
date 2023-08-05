#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

#include "rv32i.h"
#include "instructions.h"
#include "opcodes.h"

// Reset the HART (zero the registers and PC)
void core_reset(rv32core* core)
{
	for (int i = 0; i < 32; i++)
		core->x[i] = 0;
	core->pc = RAM_BASE;
	core->inst_count = 0;
}

// Clear RAM
void ram_clear(rv32core* core)
{
	for (int i = 0; i < RAM_SIZE; i++)
		core->ram[i] = 0;
}

//Register ABI names
const char* reg_names[] = {
	"zero", "ra ", "sp ", "gp ", "tp ", "t0 ", "t1 ", "t2 ", "s0 ", "s1 ", "a0 ",
	"a1 ", "a2 ", "a3 ", "a4 ", "a5 ", "a6 ", "a7 ", "s2 ", "s3 ", "s4 ", "s5 ",
	"s6 ", "s7 ", "s8 ", "s9 ", "s10", "s11", "t3 ", "t4 ", "t5 ", "t6 "
};

// Print register contents
void core_print(rv32core* core)
{
	printf("\n");
	for (int i = 1; i < 31; i++)
		printf("x%02d (%s): 0x%08x\t%d\n", i, reg_names[i], core->x[i], core->x[i]);
	printf("PC:  0x%08x\n", core->pc);
}

// RAM access
uint8_t ram_read_8(rv32core* core, uint32_t addr)
{
	addr -= RAM_BASE;
	return core->ram[addr];
}

uint16_t ram_read_16(rv32core* core, uint32_t addr)
{
	addr -= RAM_BASE;
	uint16_t r = (core->ram[addr]) | (core->ram[addr + 1] << 8);
	return r;
}

uint32_t ram_read_32(rv32core* core, uint32_t addr)
{
	addr -= RAM_BASE;
	uint32_t r = (core->ram[addr]) | (core->ram[addr + 1] << 8) | (core->ram[addr + 2] << 16) | (core->ram[addr+3] << 24);
	return r;
}

void ram_store_8(rv32core* core, uint32_t addr, uint8_t value)
{
	addr -= RAM_BASE;
	core->ram[addr] = value;
}

void ram_store_16(rv32core* core, uint32_t addr, uint16_t value)
{
	addr -= RAM_BASE;
	core->ram[addr] = value & 0xff;
	core->ram[addr+1] = (value >> 8) & 0xff;
}

void ram_store_32(rv32core* core, uint32_t addr, uint32_t value)
{
	addr -= RAM_BASE;
	core->ram[addr] = value & 0xff;
	core->ram[addr + 1] = (value >> 8) & 0xff;
	core->ram[addr + 2] = (value >> 16) & 0xff;
	core->ram[addr + 3] = (value >> 24) & 0xff;
}

// Load program from uint32_t array into memory
void loadProgram(rv32core* core, uint32_t program[], int len)
{
	for (int i = 0; i < len; i++)
		ram_store_32(core, RAM_BASE + (4*i), program[i]);
}

// MMIO reads
uint32_t mmio_load(uint32_t addr)
{
	return 1234;
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
int rv32_execute(rv32core* core)
{
	int fault = 0;

	if (core->pc & 0b11 != 0)
		return PC_UNALIGN;

	if (core->pc > (RAM_END - 4) || core->pc < RAM_BASE)
		return PC_OUT_OF_RANGE;

	uint32_t inst = ram_read_32(core, core->pc);
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