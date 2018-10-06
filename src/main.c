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
        retcode = opt_Help();
    }
    else if (argc == 2)
    {
        retcode = opt_Execute(argv[1]);
    }
    else
    {
        while ((opt = getopt(argc, argv, ":e:dsvh")) != -1)
        {
            switch (opt)
            {
                case 'e':
                    retcode = opt_Execute(optarg);
                    break;
                case 'v':
                    retcode = opt_Version();
                    break;
                case 'h':
                    retcode = opt_Help();
                    break;
                case ':':
                    printf("Missing argument(s) for: %c\n", optopt);
                    retcode = opt_Help();
                    break;
                case '?':
                default:
                    printf("Invalid option: %c\n", optopt);
                    retcode = opt_Help();
                    break;
            }
        }
    }

    return retcode;
}
