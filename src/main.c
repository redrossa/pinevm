#include "../include/options.h"
#include <getopt.h>

static struct option long_opts[] =
{
    {"execute", required_argument, NULL, 'e'},
    {"version", no_argument,       NULL, 'v'},
    {"help",    no_argument,       NULL, 'h'}
};

/*
 * The main function currently calls Create_Bin_File to create the bytecode
 * file. This is just temporary for the beta development. Once the alpha version
 * is released, Create_Bin_File is to be removed and main directly calls
 * pvm_Initialise which passes a file path to a bytecode binary file, assuming
 * it is available. Users have to create their own bytecode binary file with
 * their own implementation. The bytecode assembler is yet to be built.
 */
int main(int argc, char *argv[])
{
    int opt;
    int retcode = 0;
    extern char *optarg;

    while ((opt = getopt_long(argc, argv, "e:vh", long_opts, NULL)) != -1)
    {
        switch (opt)
        {
            case 'e':
                retcode = opt_execute(optarg);
                break;
            case 'v':
                retcode = opt_version();
                break;
            case 'h':
                retcode = opt_help();
                break;
        }
    }
    if (optind == 1)
        printf("pvm: no options specified\n");

    return retcode;
}
