#include "../include/options.h"
#include <unistd.h>

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
    int retcode;
    extern char *optarg;

    if (argc == 1)
    {
        printf("Missing arguments.\n");
        retcode = opt_help();
    }
    else if (argc == 2)
    {
        retcode = opt_execute(argv[1]);
    }
    else
    {
        while ((opt = getopt(argc, argv, ":e:dsvh")) != -1)
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
                case ':':
                    printf("Missing argument(s) for: %c\n", optopt);
                    retcode = opt_help();
                    break;
                case '?':
                default:
                    printf("Invalid option: %c\n", optopt);
                    retcode = opt_help();
                    break;
            }
        }
    }

    return retcode;
}
