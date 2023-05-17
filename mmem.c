#include "mmem.h"

#include <stdlib.h>

mesa_mem *memory_init() {
	mesa_mem *memory = malloc(sizeof(memory));
	memory->memory = (unsigned char*)malloc(MEM_SIZE);
	return memory;
}

void memory_free(mesa_mem *memory) {
	free(memory->memory);
	free(memory);
}

unsigned char fetchbyte(mesa_mem *memory, unsigned short addr16) {
	return memory->memory[addr16];
}
