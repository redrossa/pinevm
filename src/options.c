#include "../include/options.h"
#include <string.h>

int opt_Execute(char * arg)
{
    int retcode;
    VM vm;

    vm = pvm_Initialise(arg);
    retcode = pvm_Run(&vm);
    pvm_Finalise(&vm);

    return retcode;
}

int opt_Version(void)
{
    printf
    (
        "pvm version 0.0.1\n"
        "Pine Virtual Machine (TM) - 64-bit MacOS 0.0.1\n"
    );
    return 0;
}

int opt_Help(void)
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
        "   -v  : execute\n"
    );
    return 0;
}
