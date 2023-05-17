#ifndef MESA_ISA
#define MESA_ISA

#include "mvm.h"

typedef enum {
	/*    Arithmetic            */
	OP_ADD = 1,
	OP_SUB,
	OP_DIV,
	OP_MUL,
	OP_INC,
	OP_DEC,
	/*    Binary                */
	OP_NOT,
	OP_AND,
	OP_XOR,
	OP_OR,
	OP_SHL,
	OP_SHR,
	/*    Memory I/O            */
	OP_LDA,
	OP_LDR,
	OP_STA,
	OP_STR,
	/*	  Stack Shenanigans		*/
	OP_PUSH,
	OP_PUSHSP,
	OP_PUSHBP,
	OP_PUSHTP,
	OP_PUSHPC,
	
	OP_POP,
	OP_POPSP,
	OP_POPBP,
	OP_POPTP,
	OP_POPPC,

	OP_SWP,
	OP_DUP,
	OP_OVR,
	OP_ROT,
	/*    Device I/O			*/
	OP_IO,

	OP_PROC,
	OP_RET,
	OP_HLT,
} Opcodes;

#define NUM_OPCODES ((int)OP_HLT+1)

typedef struct {
	Opcodes 		opcode;
	char*			ident;
	unsigned char	(*instruction_handler)(mesa_vm*);
} mesa_instruction;

void asm_inline(mesa_vm*, int, ...);

#endif
