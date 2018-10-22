#include "../include/options.h"
#include <string.h>

int opt_execute(char * arg)
{
    int retcode;
    VM vm;

    vm = pvm_initialise(arg);
    retcode = pvm_run(&vm);
    pvm_finalise(&vm);

    return retcode;
}


/*
 * TODO: Add more option outputs for specific OSes. Perhaps add a centralised
 *       versioning output system as well.
 */
int opt_version(void)
{
    printf
    (
        "pvm version 0.0.1\n"
        "Pine Virtual Machine (TM) - 64-bit MacOS 0.0.1\n"
    );
    return 0;
}

int opt_help(void)
{
    printf
    (
        "Usage: pvm [options] [args]\n"
        "           (general options)\n"
        "   or  pvm [file]\n"
        "           (to execute bytecode file)\n"
        "\n"
        "List of possible options:\n"
        "   -h  : prints this message.\n"
        "   -e  : executes bytecode file. (args: file name in current directory)\n"
        "   -v  : prints product version.\n"
    );
    return 0;
}
