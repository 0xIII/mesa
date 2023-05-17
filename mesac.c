#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "rom.h"

#define error(message)      printf("Error at line %d: %s\n", __LINE__, message)

static struct option options[] = {
    {   "help",     no_argument,    0,  'h' },
    {   "verbose",  no_argument,    0,  'v' },
    {   0,          0,              0,  0 }
};

void usage(char *name) {
    printf(
        "      ____\n"
        " ____|    \\\n"
        "(____|     `._____\n"
        " ____| mesac _|___\n"
        "(____|     .'\n"
        "     |____/\n\n"
        "Usage: %s [OPTIONS]... [FILE]...\n"
        "compiles mesa source files to rom binaries.\n\n"
        "Options:\n"
        "-h, --help\toutput usage information\n"
        "-v, --verbose\toutput additional compilation information to stdin\n",
        name
    );
}

struct copts {
    unsigned int verbose: 1;
    char* rom_out;
};

int main(int argc, char **argv) {
    struct copts compile_options;
    int long_index, opt = 0;
    while ((opt = getopt_long(argc, argv,"hv", options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                compile_options.verbose = 1;
                break;
            default:
                usage(argv[0]);
                return EXIT_FAILURE;
        }
    }
    
    if (optind < argc) {
        compile_options.rom_out = argv[optind];
    } else {
        error("Please specify a file to write the rom to");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    FILE *handle = fopen(compile_options.rom_out, "wb+");
    if(!handle) {
        error("Unable to open handle");
        return EXIT_FAILURE;
    }

    mesa_rom rom;
    rom.section_num = 1;
    rom.sections = malloc(sizeof(mesa_section*)*rom.section_num);
    rom.sections[0].address = 256;
    rom.sections[0].size = 100;
    rom.sections[0].instructions = malloc(rom.sections[0].size);
    memset(rom.sections[0].instructions, 0xff, rom.sections[0].size);

    rom_to_binary(handle, &rom);

    return EXIT_SUCCESS;
}