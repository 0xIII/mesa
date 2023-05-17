#include "mvm.h"
#include "mesa_isa.c"

#include <stdlib.h>
#include <stdio.h>

mesa_stack *stack_init(unsigned short size) {
	mesa_stack *stack = (mesa_stack*)malloc(sizeof(mesa_stack));
	stack->stack = (unsigned char*)malloc(size);
	stack->TP = stack->BP+size;
	return stack;
}

void stack_free(mesa_stack *stack) {
	free(stack->stack);
	free(stack);
}

mesa_vm *mesa_init() {
    mesa_vm* vm = (mesa_vm*)malloc(sizeof(mesa_vm));
	vm->stack = stack_init(MESA_STACK_SIZE);
	vm->ret = stack_init(MESA_RET_SIZE);
	vm->memory = memory_init();
	return vm;
}

void mesa_free(mesa_vm *vm) {
	stack_free(vm->ret);
	stack_free(vm->stack);
	memory_free(vm->memory);
	free(vm);
}

mesa_vm *mesa_step(mesa_vm *vm) {
	printf("PC=0x%04x: ", PC(vm));
	unsigned char operand = fetchbyte(vm->memory, PC(vm));

	// TODO: Replace the for-loop with direct indexing
	for (int i = 0; i<NUM_OPCODES; i++) {
		if ((unsigned char)mesa_instructionset[i].opcode == operand) {
			printf("Found instruction '%s' (0x%02x)\n", mesa_instructionset[i].ident, mesa_instructionset[i].opcode);
			mesa_instructionset[i].instruction_handler(vm);
			break;
		}
	}
	PC(vm)++;
	return vm;
}

mesa_vm *mesa_run(mesa_vm *vm) {
	while(HLT(vm) != 1) {
		mesa_step(vm);
	}
	printf("EXECUTION HALTED!\n");
}

#ifdef DEBUG
#include <stdio.h>

mesa_vm *stack_info(mesa_vm *vm) {
	printf("\nBP: 0x%02x\n", BP(vm));
	printf("SP: 0x%02x\n", SP(vm));
	printf("TP: 0x%02x\n", TP(vm));
	return vm;
}

mesa_vm *stack_peek(mesa_vm *vm, unsigned short start, unsigned short len) {
	printf("\nAddress\t| Value\n");
	for (int offset = 0; offset<len; offset++) {
		printf("0x%04x\t| 0x%02x\n", start+offset, STACK(vm)[start+offset]);
	}
	return vm;
}

#endif