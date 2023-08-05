/*
* RISC-V adventures
* i made a RV32I virtual machine
* 
* RV32I User-mode ISA
* 64k of RAM at 0x80000000
* UART register at 0x10000000
* 
*/

#define _CRT_SECURE_NO_WARNINGS // allow building on MSVC

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Where the goodies live
#include "rv32i.h"

int main(int argc, char* argv[])
{
	rv32core cpu;	  // instantiate CPU
	ram_clear(&cpu);  // clear RAM
	core_reset(&cpu); // reset CPU
	
	if (argc < 2)
	{
		printf("Usage: %s [filename]\n", argv[0]);
		exit(-1);
	}

	FILE* binfile;
	binfile = fopen(argv[1], "rb");
	if (binfile == NULL)
	{
		printf("Error loading file. %s\n", argv[1]);
		exit(-2);
	}

	fseek(binfile, 0, SEEK_END);
	size_t filesize = ftell(binfile);
	fseek(binfile, 0, SEEK_SET);

	if (filesize > RAM_SIZE)
	{
		printf("File %s exceeds RAM size by %d bytes\n", argv[1], filesize - RAM_SIZE);
		fclose(binfile);
		exit(-2);
	}

	fread(cpu.ram, 1, filesize, binfile);
	fclose(binfile);

	int fault = 0;
	while (!fault)
	{
		fault = rv32_execute(&cpu); // execute one instruction

		/*
		if (!fault) {
			core_print(&cpu);   // show CPU contents
			printf("****************\n");
		}
		*/
	}
	
	printf("\n");
	switch (fault)
	{

	case UNDEF_OPCODE:
		printf("Undefined opcode\n");
		break;

	case UNDEF_FUNC3:
		printf("Undefined func3\n");
		break;

	case PC_UNALIGN:
		printf("Unaligned PC\n");
		break;

	case PC_OUT_OF_RANGE:
		printf("PC out of range!\n");
		break;

	case SYSCON_SHUTDOWN:
		printf("Poweroff by SYSCON\n");
		break;

	default:
		printf("Unknown fault\n");
		break;
	}

	printf("Executed %d instructions\n", cpu.inst_count - 1);
		

	return 0;
}