#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
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
	uint16_t sign = val & 0x800;
	if (sign)
		return val | 0xFFFFF000;
	return val;
}

int exec_op_imm(rv32hart *hart, uint32_t inst)
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
		hart->x[rd] = hart->x[rs1] + ext_imm;
		break;
	
	case SLTI:
		int32_t sg_reg = (int32_t)(hart->x[rs1]);
		int32_t sg_extimm = (int32_t)(ext_imm);
		if (sg_reg < sg_extimm)
			hart->x[rd] = 1;
		else hart->x[rd] = 0;
		break;

	case SLTIU:
		if (hart->x[rs1] < ext_imm)
			hart->x[rd] = 1;
		else hart->x[rd] = 0;
		break;

	case ANDI:
		hart->x[rd] = hart->x[rs1] & ext_imm;
		break;

	case ORI:
		hart->x[rd] = hart->x[rs1] | ext_imm;
		break;

	case XORI:
		hart->x[rd] = hart->x[rs1] ^ ext_imm;
		break;

	case SLLI:
		shamt = imm & 0x1F;
		hart->x[rd] = hart->x[rs1] << shamt;
		break;

	case SRLI_SRAI:
		shamt = imm & 0x1F;
		uint8_t func7 = get_func7(inst);
		if (func7) {
			// SRAI
			uint32_t sign = hart->x[rs1] & (1 << 31);
			if (sign)
				sign = ~0 << (uint8_t)(32 - shamt);
			hart->x[rd] = (hart->x[rs1] >> shamt) | sign;
		}
		else // SRLI
			hart->x[rd] = hart->x[rs1] >> shamt;
		break;

	default:
		return UNDEF_MICROOP;
		break;
	}

	return 0;
}

int exec_op_lui(rv32hart* hart, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	hart->x[rd] = inst & 0xFFFFF000;
	return 0;
}

int exec_op_auipc(rv32hart* hart, uint32_t inst)
{
	uint8_t rd = get_rd(inst);
	hart->x[rd] = (inst & 0xFFFFF000) + hart->pc;
	return 0;
}

int exec_op_op(rv32hart* hart, uint32_t inst)
{
	uint8_t func3 = get_func3(inst);
	uint8_t func7 = get_func7(inst);
	uint8_t rs1 = get_rs1(inst);
	uint8_t rs2 = get_rs2(inst);
	uint8_t rd = get_rd(inst);

	switch (func3)
	{

	case ADD_SUB:
		if (!func7) // ADD
			hart->x[rd] = hart->x[rs1] + hart->x[rs2];
		else hart->x[rd] = hart->x[rs1] - hart->x[rs2]; // SUB
		break;

	case SLL:
		hart->x[rd] = hart->x[rs1] << (hart->x[rs2] & 0x1F);
		break;

	case SRL_SRA:
		uint8_t shamt = (hart->x[rs2] & 0x1F);
		if (func7) {
			// SRA
			uint32_t sign = hart->x[rs1] & (1 << 31);
			if (sign)
				sign = ~0 << (uint8_t)(32 - shamt);
			hart->x[rd] = (hart->x[rs1] >> shamt) | sign;
		}
		else // SRL
			hart->x[rd] = hart->x[rs1] >> shamt;
		break;

	case SLT:
		int32_t sg_rs1 = (int32_t)(hart->x[rs1]);
		int32_t sg_rs2 = (int32_t)(hart->x[rs2]);
		if (sg_rs1 < sg_rs2)
			hart->x[rd] = 1;
		else hart->x[rd] = 0;
		break;

	case SLTU:
		if (hart->x[rs1] < hart->x[rs2])
			hart->x[rd] = 1;
		else hart->x[rd] = 0;
		break;

	case AND:
		hart->x[rd] = hart->x[rs1] & hart->x[rs2];
		break;

	case OR:
		hart->x[rd] = hart->x[rs1] | hart->x[rs2];
		break;

	case XOR:
		hart->x[rd] = hart->x[rs1] ^ hart->x[rs2];
		break;

	default:
		return UNDEF_MICROOP;
		break;
	}

	return 0;
}