#ifndef MESA_ROM
#define MESA_ROM

#include <stdio.h>

#define MAGIC       "MESA"
#define MAGIC_SIZE  4

typedef struct {
    unsigned short address;
    unsigned short size;
    unsigned char *instructions;
} mesa_section;

typedef struct {
    unsigned short section_num;
    mesa_section* sections;
} mesa_rom;

int rom_to_binary(FILE*, mesa_rom*);

#endif