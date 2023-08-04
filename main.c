/*
* RISC-V adventures
* idk what im doing (writing an emulator i guess)
*/

#define CRT_SECURE_NO_WARNINGS // MSVC silliness

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Where the goodies live
#include "rv32i.h"

uint32_t program[]={ 
	// XOR instruction test
	0x4d200293,
	0x09900313,
	0x0062c3b3,
	0x0063ce33
};

int main()
{
	rv32hart cpu;	  // instantiate CPU
	ram_clear(&cpu);  // clear RAM
	hart_reset(&cpu); // reset CPU
	
	loadProgram(&cpu, program, _countof(program));

	int fault = 0;
	while (!fault)
	{
		fault = rv32_execute(&cpu); // execute one instruction
		if (!fault) {
			hart_print(&cpu);   // show CPU contents
			printf("****************\n");
		}
	}

	printf("FAULT! ");

	switch (fault)
	{

	case UNDEF_OPCODE:
		printf("Undefined opcode\n");
		break;

	case UNDEF_MICROOP:
		printf("Undefined micro-operation\n");
		break;

	case PC_UNALIGN:
		printf("Unaligned PC\n");
		break;

	default:
		printf("Unknown fault\n");
		break;
	}

	printf("Executed %d instructions\n", cpu.inst_count - 1);
		

	return 0;
}