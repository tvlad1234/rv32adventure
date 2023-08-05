#pragma once

// RV32I opcodes

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

#define OP_JAL  0x6F
#define OP_JALR 0x67

#define OP_BRANCH 0x63
	#define BEQ  0b000
	#define BNE  0b001
	#define BLT  0b100
	#define BGE	 0b101
	#define BLTU 0b110
	#define BGEU 0b111

#define OP_LOAD  0x03
	#define LB	0b000
	#define LH	0b001
	#define LW	0b010
	#define LBU 0b100	
	#define LHU 0b101

#define OP_STORE 0x23
	#define SB 0b000
	#define SH 0b001
	#define SW 0b010

// RV32M opcodes

#define MUL		0b000
#define MULH	0b001
#define MULHSU	0b010
#define MULHU	0b011
#define DIV		0b100
#define DIVU	0b101
#define REM		0b110
#define	REMU	0b111
