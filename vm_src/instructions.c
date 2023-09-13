#include <stdint.h>

#include "rv32i.h"
#include "opcodes.h"

// Functions used for decoding instructions

uint8_t get_opcode(uint32_t inst)
{
	return inst & 0x7F;
}

uint8_t get_rd(uint32_t inst)
{
	return (inst >> 7) & 0x1F;
}

uint8_t get_rs1(uint32_t inst)
{
	return (inst >> 15) & 0x1F;
}

uint8_t get_rs2(uint32_t inst)
{
	return (inst >> 20) & 0x1F;
}

uint8_t get_func3(uint32_t inst)
{
	return (inst >> 12) & 0b111;
}

uint8_t get_func7(uint32_t inst)
{
	return (inst >> 25) & 0x7F;
}

// Get immediate value from I-type instruction
uint16_t imm_type_i(uint32_t inst)
{
	return inst >> 20;
}

// Sign extend from 12 bits to 32
uint32_t signextend_12(uint16_t val)
{
	if (val & 0x800)
		return val | 0xFFFFF000;
	else return val;
}

int exec_op_imm(rv32core *core, uint32_t inst)
{
	uint8_t func3 = get_func3(inst);
	uint8_t rs1 = get_rs1(inst);
	uint8_t rd = get_rd(inst);
	uint16_t imm = imm_type_i(inst);
	uint32_t ext_imm = signextend_12(imm);
	uint8_t shamt;

	switch (func3)
	{

	case ADDI:
		core->x[rd] = core->x[rs1] + ext_imm;
		break;
	
	case SLTI:
		if ((int32_t)(core->x[rs1]) < (int32_t)(ext_imm))
			core->x[rd] = 1;
		else core->x[rd] = 0;
		break;

	case SLTIU:
		if (core->x[rs1] < ext_imm)
			core->x[rd] = 1;
		else core->x[rd] = 0;
		break;

	case ANDI:
		core->x[rd] = core->x[rs1] & ext_imm;
		break;

	case ORI:
		core->x[rd] = core->x[rs1] | ext_imm;
		break;

	case XORI:
		core->x[rd] = core->x[rs1] ^ ext_imm;
		break;

	case SLLI:
		shamt = imm & 0x1F;
		core->x[rd] = core->x[rs1] << shamt;
		break;

	case SRLI_SRAI:
		shamt = imm & 0x1F;
		uint8_t func7 = get_func7(inst);
		if (func7) {
			// SRAI
			uint32_t sign = core->x[rs1] & (1 << 31);
			if (sign)
				sign = ~0 << (uint8_t)(32 - shamt);
			core->x[rd] = (core->x[rs1] >> shamt) | sign;
		}
		else // SRLI
			core->x[rd] = core->x[rs1] >> shamt;
		break;

	default:
		return UNDEF_FUNC3;
		break;
	}

	return 0;
}

int exec_op_lui(rv32core* core, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	core->x[rd] = inst & 0xFFFFF000;
	return 0;
}

int exec_op_auipc(rv32core* core, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	core->x[rd] = (inst & 0xFFFFF000) + core->pc;
	return 0;
}

int exec_op_op(rv32core* core, uint32_t inst)
{
	uint8_t func3 = get_func3(inst);
	uint8_t func7 = get_func7(inst);
	uint8_t rs1 = get_rs1(inst);
	uint8_t rs2 = get_rs2(inst);
	uint8_t rd = get_rd(inst);


	if (func7 == 1) // RV32M extension
	{
		switch (func3)
		{
		case MUL:
			core->x[rd] = core->x[rs1] * core->x[rs2];
			break;

		case MULH:
			core->x[rd] = ((int64_t)((int32_t)core->x[rs1]) * (int64_t)((int32_t)core->x[rs2])) >> 32;
			break;

		case MULHSU:
			core->x[rd] = ((int64_t)((int32_t)core->x[rs1]) * (uint64_t)core->x[rs2]) >> 32;
			break;

		case MULHU:
			core->x[rd] = ((uint64_t)core->x[rs1] * (uint64_t)core->x[rs2]) >> 32;
			break;

		// finish the M extension!!!

		default:
			return UNDEF_FUNC3;
			break;
		}
	}
	else { // RV32I
		switch (func3)
		{
		case ADD_SUB:
			if (!func7) // ADD
				core->x[rd] = core->x[rs1] + core->x[rs2];
			else core->x[rd] = core->x[rs1] - core->x[rs2]; // SUB
			break;

		case SLL:
			core->x[rd] = core->x[rs1] << (core->x[rs2] & 0x1F);
			break;

		case SRL_SRA:
			uint8_t shamt = (core->x[rs2] & 0x1F);
			if (func7) {
				// SRA
				uint32_t sign = core->x[rs1] & (1 << 31);
				if (sign)
					sign = ~0 << (uint8_t)(32 - shamt);
				core->x[rd] = (core->x[rs1] >> shamt) | sign;
			}
			else // SRL
				core->x[rd] = core->x[rs1] >> shamt;
			break;

		case SLT:
			if ((int32_t)(core->x[rs1]) < (int32_t)(core->x[rs2]))
				core->x[rd] = 1;
			else core->x[rd] = 0;
			break;

		case SLTU:
			if (core->x[rs1] < core->x[rs2])
				core->x[rd] = 1;
			else core->x[rd] = 0;
			break;

		case AND:
			core->x[rd] = core->x[rs1] & core->x[rs2];
			break;

		case OR:
			core->x[rd] = core->x[rs1] | core->x[rs2];
			break;

		case XOR:
			core->x[rd] = core->x[rs1] ^ core->x[rs2];
			break;

		default:
			return UNDEF_FUNC3;
			break;
		}
	}
	return 0;
}

int exec_op_jal(rv32core* core, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	uint32_t imm = ((inst & 0x80000000) >> 11) | ((inst & 0x7FE00000) >> 20) | ((inst & 0x00100000) >> 9) | (inst & 0x000ff000); // J-type immediate
	if (imm & 0x00100000) // Sign extend
		imm |= 0xffe00000;
	core->x[rd] = core->pc + 4;
	core->pc = core->pc + imm - 4;
	return 0;
}

int exec_op_jalr(rv32core* core, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	uint8_t rs1 = get_rs1(inst);
	core->x[rd] = core->pc + 4;
	core->pc = ((signextend_12(imm_type_i(inst)) + core->x[rs1]) & 0xFFFFFFFE) - 4;
	return 0;
}

int exec_op_load(rv32core* core, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	uint8_t rs1 = get_rs1(inst);
	uint32_t addr = signextend_12(imm_type_i(inst)) + core->x[rs1];
	uint8_t func3 = get_func3(inst);

	if (!inMemory(addr)) // MMIO
	{
		core->x[rd] = mmio_load(addr);
		return 0;
	}

	switch (func3)
	{

	case LW:
		core->x[rd] = mem_read_32(core, addr);
		break;

	case LH:
		core->x[rd] = (int16_t)mem_read_16(core, addr);
		break;

	case LHU:
		core->x[rd] = mem_read_16(core, addr);
		break;

	case LB:
		core->x[rd] = (int8_t)mem_read_8(core, addr);
		break;

	case LBU:
		core->x[rd] = mem_read_8(core, addr);
		break;

	default:
		return UNDEF_FUNC3;
		break;
	}

	return 0;
}

int exec_op_store(rv32core* core, uint32_t inst)
{
	uint8_t rs1 = get_rs1(inst);
	uint8_t rs2 = get_rs2(inst);
	uint32_t addr = signextend_12( ((inst >> 7) & 0x1F) | ((inst & 0xFE000000) >> 20)) + core->x[rs1];
	uint8_t func3 = get_func3(inst);

	if (!inMemory(addr)) // MMIO
		return mmio_store(addr, core->x[rs2]);

	if(inROM(addr))
		return WRITE_ROM;

	switch (func3)
	{

	case SW:
		mem_store_32(core, addr, core->x[rs2]);
		break;

	case SH:
		mem_store_16(core, addr, core->x[rs2]);
		break;

	case SB:
		mem_store_8(core, addr, core->x[rs2]);
		break;

	default:
		return UNDEF_FUNC3;
		break;
	}

	return 0;
}

int exec_op_branch(rv32core* core, uint32_t inst)
{
	uint8_t rs1 = get_rs1(inst);
	uint8_t rs2 = get_rs2(inst);
	uint8_t func3 = get_func3(inst);
	uint32_t addr = ((inst & 0xF00) >> 7) | ((inst & 0x7E000000) >> 20) | ((inst & 0x80) << 4) | ((inst >> 31) << 12); // B-type immediate
	if (addr & 0x1000) // Sign extend
		addr |= 0xFFFFE000;
	addr = addr + core->pc - 4;

	switch (func3)
	{

	case BEQ:
		if (core->x[rs1] == core->x[rs2])
			core->pc = addr;
		break;

	case BNE:
		if (core->x[rs1] != core->x[rs2])
			core->pc = addr;
		break;

	case BLTU:
		if (core->x[rs1] < core->x[rs2])
			core->pc = addr;
		break;

	case BLT:
		if ((int32_t)core->x[rs1] < (int32_t)core->x[rs2])
			core->pc = addr;
		break;

	case BGEU:
		if (core->x[rs1] >= core->x[rs2])
			core->pc = addr;
		break;

	case BGE:
		if ((int32_t)core->x[rs1] > (int32_t)core->x[rs2])
			core->pc = addr;
		break;

	default:
		return UNDEF_FUNC3;
		break;
	}

	return 0;
}
