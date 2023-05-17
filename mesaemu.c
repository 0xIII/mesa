#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mvm.h"
#include "mesa_isa.h"

#include <getopt.h>

#define error(message)      printf("Error at line %d: %s\n", __LINE__, message)

static struct option options[] = {
    {   "help",     no_argument,    0,  'h' },
    {   "verbose",  no_argument,    0,  'v' },
    {   0,          0,              0,  0   }
};

void usage(char *name) {
    printf("   _____   \n");
    printf("  | ___ |  MESA\n");
    printf("  ||   ||  Emulator\n");
    printf("  ||___||  v0.1.0\n");
    printf("  |   _ |  %s\n", __DATE__);
    printf("  |_____|  \n\n");
    printf(
        "Usage: %s [OPTIONS]... [ROM]\n"
        "Options:\n"
        "-h, --help\toutput usage information\n"
        "-v, --verbose\toutput additional compilation information to stdin\n",
        name
    );
}

struct mopts {
    unsigned char verbose: 1;
    char *rom;
};

int main(int argc, char **argv) {
    struct mopts mesa_options;
    int long_index, opt = 0;
    while ((opt = getopt_long(argc, argv,"hv", options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                mesa_options.verbose = 1;
                break;
            default:
                usage(argv[0]);
                return EXIT_FAILURE;
        }
    }
    
    if (optind < argc) {
        mesa_options.rom = argv[optind];
    } else {
        error("Please specify a rom file");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    FILE *handle = fopen(mesa_options.rom, "r");
    if(!handle) {
        error("Unable to open handle");
        return EXIT_FAILURE;
    }

    mesa_vm *vm = mesa_init();

    asm_inline(
        vm,
        13,
        "push 1",
        "push 1",
        "push 0",
        "push 12",
        "poppc",
        "dup",
        "rot",
        "add",
        "ret",
        "push 0",
        "push 10",
        "proc",
        "hlt"
        );

    mesa_run(vm);
    stack_info(vm);
    stack_peek(vm, 1, 10);

    fgets((unsigned char*)vm->memory->memory, MEM_SIZE, handle);
    fclose(handle);

    return 0;
}