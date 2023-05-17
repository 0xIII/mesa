#ifndef MESA_MEM
#define MESA_MEM

#define MEM_SIZE 0xFFFF

typedef struct {
	unsigned char* memory;
} mesa_mem;

mesa_mem *memory_init();
void memory_free(mesa_mem*);

unsigned char fetchbyte(mesa_mem*, unsigned short);

#endif
