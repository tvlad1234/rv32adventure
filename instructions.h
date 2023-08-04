#pragma once
#include <stdint.h>

uint8_t get_opcode(uint32_t inst);
uint8_t get_rd(uint32_t inst);
uint8_t get_rs1(uint32_t inst);
uint8_t get_rs2(uint32_t inst);
uint8_t get_func3(uint32_t inst);

int exec_op_imm(rv32hart* hart, uint32_t inst);
int exec_op_lui(rv32hart* hart, uint32_t inst);
int exec_op_auipc(rv32hart* hart, uint32_t inst);
int exec_op_op(rv32hart* hart, uint32_t inst);