#pragma once

#define OP_IMM 0x13
	#define ADDI	0b000
	#define SLTI	0b010
	#define SLTIU	0b011
	#define XORI	0b100
	#define ORI		0b110
	#define ANDI	0b111
	#define SLLI	0b001
	#define SRLI_SRAI 0b101

#define OP_LUI	 0x37
#define OP_AUIPC 0x17

#define OP_OP	 0x33 
	#define ADD_SUB 0b000
	#define SLL		0b001
	#define SLT		0b010
	#define SLTU	0b011
	#define XOR		0b100
	#define SRL_SRA 0b101
	#define OR		0b110
	#define AND		0b111
