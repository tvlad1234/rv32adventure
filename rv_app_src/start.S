
.section .initial_jump
.global _start

.align 4
_start:
	la	sp, _sstack
	addi	sp,sp,-16
	sw	ra,12(sp)
	jal	ra, baremain


