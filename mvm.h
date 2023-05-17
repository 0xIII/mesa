#ifndef MESA_VM
#define MESA_VM

#include "mmem.h"

#define MESA_STACK_SIZE  	0x1000
#define MESA_RET_SIZE		0xFF*2

// TODO: rework the guards
#define SOF_GUARD(stack)	(stack->SP == stack->TP)
#define SUF_GUARD(stack) 	(stack->BP == stack->SP)
#define STACK_GUARD(stack)	(SOF_GUARD(stack) || SUF_GUARD(stack))

#define SP(vm)				vm->stack->SP
#define BP(vm)				vm->stack->BP
#define TP(vm)				vm->stack->TP
#define STACK(vm)			vm->stack->stack

#define RSP(vm)				vm->ret->SP
#define RBP(vm)				vm->ret->BP
#define RTP(vm)				vm->ret->TP
#define RSTACK(vm)			vm->ret->stack

#define PC(vm)				vm->PC

/*
*	mesa_stack keeps track of the stack state.
*	
*	unsigned char *stack	memory space allocated for the stack
*	unsigned short BP		base of stack address
*	unsigned short SP		highest stack address in use
*	unsigned short TP		highest usable stack address (SOF prevention)
*/
typedef struct {
	unsigned char *stack;
	unsigned short BP, SP, TP;	// BP: 0xFF SP: 0xFE TP: 0xFD
} mesa_stack;

mesa_stack *stack_init(unsigned short);
void stack_free(mesa_stack*);

/*
*	mesa_errors errors returned onto the stack by the virtual machine.
* 	
*	M_STACKOVERFLOW			returned upon attempting to push a value onto a full stack
*	M_STACKUNDERFLOW		returned upon attempting to pop a value from an empty stack
*	M_INVALIDACCESS			attempt to access invalid memory locations (out of bounds or protected)
*	M_INVALIDOPCODE			returned if an opcode cannot be found in the instruction set
*/
typedef enum {
	M_STACKOVERFLOW = 0xFC,
	M_STACKUNDERFLOW = 0xFD,
	M_INVALIDACCESS = 0xFE,
	M_INVALIDOPCODE = 0xFF,
} mesa_errors;

/*
*	mesa_vm holds the current state of the virtual machine
*	and that of its multiple sub-systems.
*	
*	mesa_mem *memory		pointer to the program memory structure
*	unsigned short PC		program counter indexes the current instruction
*	unsigned char flags		flags influencing program execution
*	mesa_stack *stack		pointer to the main stack
*	mesa_stack *ret			pointer to the return stack used to keep track of call return values
*/
typedef struct {
	mesa_mem *memory;
	unsigned short PC;
	unsigned char flags;
	mesa_stack *stack;
	mesa_stack *ret;
} mesa_vm;

#define	HLT(vm)				((vm->flags & 0x80) >> 7)
#define	ZER(vm)				((vm->flags & 0x40) >> 6)

/*
 *	flags
 *	BIT	0	1	2	3	4	5	6	7
 *      HLT	ZER	
 * 	
 * Description (if bit is enabled)
 * HLT	execution is halted
 * ZER	used for control flow logic tests
*/

mesa_vm *mesa_init();
void mesa_free(mesa_vm*);
mesa_vm *mesa_step(mesa_vm*);
mesa_vm *mesa_run(mesa_vm*);

#define DEBUG
#ifdef DEBUG

mesa_vm	*stack_info(mesa_vm*);
mesa_vm *stack_peek(mesa_vm*, unsigned short, unsigned short);

#endif

#endif
