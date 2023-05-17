#include <stdio.h>
#include <stdlib.h>

#include "rom.h"

int rom_to_binary(FILE* fp, mesa_rom *rom) {
    fwrite(MAGIC, 1, MAGIC_SIZE, fp);
    unsigned short last_section_end = 0;
    for(int i = 0; i<rom->section_num; i++) {
        unsigned short padding = rom->sections[i].address - last_section_end;
        unsigned char* zerobuffer = malloc(padding);
        fwrite(zerobuffer, 1, padding, fp);
        fwrite(rom->sections[i].instructions, 1, rom->sections[i].size, fp);
        last_section_end  = rom->sections[i].address + rom->sections[i].size;
    }

    return EXIT_SUCCESS;
}