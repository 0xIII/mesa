#include "mesa_isa.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define _U(val)     (val >> 8)
#define _L(val)     (val & 0xff)

static unsigned char POP(mesa_vm *vm)           { unsigned char val8 = STACK(vm)[SP(vm)]; STACK(vm)[SP(vm)--] = 0; return val8; }
static unsigned char POPSP(mesa_vm *vm)         { int _t = SP(vm); SP(vm) = STACK(vm)[_t]; STACK(vm)[_t--] = 0; SP(vm) ^= STACK(vm)[_t] << 8; }
static unsigned char POPPC(mesa_vm *vm)         { PC(vm) = STACK(vm)[SP(vm)]; STACK(vm)[SP(vm)--] = 0; PC(vm) ^= STACK(vm)[SP(vm)] << 8; }

static unsigned char PUSH(mesa_vm *vm)          { *(STACK(vm)+(++SP(vm))) = fetchbyte(vm->memory, ++vm->PC); }
static unsigned char PUSHSP(mesa_vm *vm)        { int _t = SP(vm); SP(vm)++; *(STACK(vm)+SP(vm)) = _U(_t); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(_t); }
static unsigned char PUSHBP(mesa_vm *vm)        { SP(vm)++; *(STACK(vm)+SP(vm)) = _U(BP(vm)); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(BP(vm)); }
static unsigned char PUSHTP(mesa_vm *vm)        { SP(vm)++; *(STACK(vm)+SP(vm)) = _U(TP(vm)); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(TP(vm)); }
static unsigned char PUSHPC(mesa_vm *vm)        { SP(vm)++; *(STACK(vm)+SP(vm)) = _U(PC(vm)); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(PC(vm)); }

static unsigned char SWAP(mesa_vm *vm)          { unsigned char _t = *(STACK(vm)+SP(vm)); *(STACK(vm)+SP(vm)) = *(STACK(vm)+SP(vm)-1); *(STACK(vm)+SP(vm)-1) = _t; }
static unsigned char DUP(mesa_vm *vm)           { SP(vm)++; *(STACK(vm)+SP(vm)) = *(STACK(vm)+SP(vm)-1); }
static unsigned char OVER(mesa_vm *vm)          { SP(vm)++; *(STACK(vm)+SP(vm)) = *(STACK(vm)+SP(vm)-2); }
static unsigned char ROT(mesa_vm *vm)           { unsigned char _t = *(STACK(vm)+SP(vm)); *(STACK(vm)+SP(vm)) = *(STACK(vm)+SP(vm)-2); *(STACK(vm)+SP(vm)-2) = *(STACK(vm)+SP(vm)-1); *(STACK(vm)+SP(vm)-1) = _t; }

static unsigned char INC(mesa_vm *vm)           { *(STACK(vm)+SP(vm)) = *(STACK(vm)+SP(vm))+1; }
static unsigned char DEC(mesa_vm *vm)           { *(STACK(vm)+SP(vm)) = *(STACK(vm)+SP(vm))-1; }
static unsigned char ADD(mesa_vm *vm)           { int result = POP(vm) + POP(vm); *(STACK(vm)+(++SP(vm))) = _U(result); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(result); }
static unsigned char SUB(mesa_vm *vm)           { int result = POP(vm) - POP(vm); *(STACK(vm)+(++SP(vm))) = _U(result); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(result); }
static unsigned char DIV(mesa_vm *vm)           { int result = POP(vm) / POP(vm); *(STACK(vm)+(++SP(vm))) = _U(result); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(result); }
static unsigned char MUL(mesa_vm *vm)           { int result = POP(vm) * POP(vm); *(STACK(vm)+(++SP(vm))) = _U(result); SP(vm)++; *(STACK(vm)+SP(vm)) = _L(result); }

static unsigned char NOT(mesa_vm *vm)           { unsigned char _t = ~*(STACK(vm)+SP(vm)); *(STACK(vm)+SP(vm)) = _t; }
static unsigned char AND(mesa_vm *vm)           { unsigned char result = POP(vm) & POP(vm); *(STACK(vm)+(++SP(vm))) = result; }
static unsigned char XOR(mesa_vm *vm)           { unsigned char result = POP(vm) ^ POP(vm); *(STACK(vm)+(++SP(vm))) = result; }
static unsigned char OR(mesa_vm *vm)            { unsigned char result = POP(vm) | POP(vm); *(STACK(vm)+(++SP(vm))) = result; }
static unsigned char SHL(mesa_vm *vm)           { unsigned char result = POP(vm) << POP(vm); *(STACK(vm)+(++SP(vm))) = result; }
static unsigned char SHR(mesa_vm *vm)           { unsigned char result = POP(vm) >> POP(vm); *(STACK(vm)+(++SP(vm))) = result; }

// TODO: Test Procedures RSP(vm)++; RSTACK(vm)[(RSP(vm))] = _U(PC(vm)); RSP(vm)++; RSTACK(vm)[RSP(vm)] = _L(PC(vm));
static unsigned char PROC(mesa_vm *vm)          { RSP(vm)++; RSTACK(vm)[(RSP(vm))] = _U(PC(vm)); RSP(vm)++; RSTACK(vm)[RSP(vm)] = _L(PC(vm)); POPPC(vm); }
static unsigned char RET(mesa_vm *vm)           { PC(vm) = *(RSTACK(vm)+RSP(vm)); *(RSTACK(vm)+(RSP(vm)--)) = 0; PC(vm) ^= *(RSTACK(vm)+RSP(vm)) << 8; *(RSTACK(vm)+(RSP(vm)--)) = 0; }
static unsigned char HALT(mesa_vm *vm)          { vm->flags^=0x80; }

mesa_instruction mesa_instructionset[NUM_OPCODES] = {
    { OP_PUSH,      "push",         &PUSH   },
    { OP_PUSHSP,    "pushsp",       &PUSHSP },
    { OP_PUSHBP,    "pushbp",       &PUSHBP },
    { OP_PUSHTP,    "pushtp",       &PUSHTP },
    { OP_PUSHPC,    "pushpc",       &PUSHPC },
    { OP_POP,       "pop",          &POP    },
    { OP_POPSP,     "popsp",        &POPSP  },
    { OP_POPPC,     "poppc",        &POPPC  },
    { OP_SWP,       "swp",          &SWAP   },
    { OP_DUP,       "dup",          &DUP    },
    { OP_OVR,       "ovr",          &OVER   },
    { OP_ROT,       "rot",          &ROT    },
    { OP_INC,       "inc",          &INC    },
    { OP_DEC,       "dec",          &DEC    },
    { OP_ADD,       "add",          &ADD    },
    { OP_SUB,       "sub",          &SUB    },
    { OP_MUL,       "mul",          &MUL    },
    { OP_DIV,       "div",          &DIV    },
    { OP_NOT,       "not",          &NOT    },
    { OP_AND,       "and",          &AND    },
    { OP_XOR,       "xor",          &XOR    },
    { OP_OR,        "or",           &OR     },
    { OP_SHL,       "shl",          &SHL    },
    { OP_SHR,       "shr",          &SHR    },
    { OP_PROC,      "proc",         &PROC   },
    { OP_RET,       "ret",          &RET    },
    { OP_HLT,       "hlt",          &HALT   }
};

void asm_inline(mesa_vm *vm, int x, ...) {
    va_list lines;
    va_start(lines, x);

    int memidx = 0;
    int i;
    for(i = 0; i < x; i++) {
        char *line = va_arg(lines, char *);
        char *instruction = malloc(sizeof(char *)*strlen(line));
        strcpy(instruction, line);
        char *a = strtok(instruction, " ");
        char *b = strtok(NULL, " ");

        for (int j = 0; j<NUM_OPCODES; j++) {
            if (strcmp(mesa_instructionset[j].ident, a) == 0) {
                vm->memory->memory[memidx++] = mesa_instructionset[j].opcode;
                if(b) { vm->memory->memory[memidx++] = atoi(b); }
                break;
            }
	    }
        free(instruction);
    }

    va_end(lines);
}
