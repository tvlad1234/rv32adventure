#pragma once

#include <stdint.h>
#include "rv32i.h"

uint8_t get_opcode(uint32_t inst);

int exec_op_op(rv32core* core, uint32_t inst);
int exec_op_imm(rv32core* core, uint32_t inst);

int exec_op_lui(rv32core* core, uint32_t inst);
int exec_op_auipc(rv32core* core, uint32_t inst);

int exec_op_jal(rv32core* core, uint32_t inst);
int exec_op_jalr(rv32core* core, uint32_t inst);
int exec_op_branch(rv32core* core, uint32_t inst);

int exec_op_load(rv32core* core, uint32_t inst);
int exec_op_store(rv32core* core, uint32_t inst);