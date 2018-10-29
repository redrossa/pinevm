#include "../include/options.h"
#include <getopt.h>

static struct option long_opts[] =
{
    {"execute", required_argument, NULL, 'e'},
    {"version", no_argument,       NULL, 'v'},
    {"help",    no_argument,       NULL, 'h'}
};

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
    if (optind == 1 && argc == 2)
        retcode = opt_execute(argv[1]);
    if (optind == 1)
        printf("pvm: no options specified\n");

    return retcode;
}
