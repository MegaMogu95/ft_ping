#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "parsing.h"

/*
** Long options. We only support the mandatory subset (-v and -?):
**   --verbose  -> 'v'
**   --help     -> '?'
**   --usage    -> '?'  (mirrors inetutils, both map to the help screen)
*/
static const struct option g_long_opts[] = {
    {"verbose", no_argument, 0, 'v'},
    {"help",    no_argument, 0, '?'},
    {"usage",   no_argument, 0, '?'},
    {0,         0,           0,  0 }
};

void    print_usage(const char *prog)
{
    printf("Usage: %s [OPTION...] HOST ...\n", prog);
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
    printf("\n");
    printf("  -v, --verbose              verbose output\n");
    printf("  -?, --help                 give this help list\n");
    printf("\n");
    printf("Mandatory or optional arguments to long options are also mandatory "
           "or optional\n");
    printf("for any corresponding short options.\n");
    printf("\n");
    printf("Report bugs to <akrings@student.42.fr>.\n");
}

void    parse_options(int argc, char **argv, t_opts *opts)
{
    int opt;
    int longindex;

    memset(opts, 0, sizeof(t_opts));

    /*
    ** We set opterr = 0 to suppress getopt's own error messages and handle
    ** them ourselves. This also lets us distinguish between:
    **   - the user passing '-?' intentionally  -> opt == '?', optopt == 0
    **   - getopt signaling an unknown option   -> opt == '?', optopt != 0
    */
    opterr = 0;

    while ((opt = getopt_long(argc, argv, "v?", g_long_opts, &longindex)) != -1)
    {
        switch (opt)
        {
            case 'v':
                opts->verbose = 1;
                break ;

            case '?':
                /*
                ** optopt is set to the offending character on unknown options.
                ** It is 0 when '-?' was passed explicitly.
                */
                if (optopt != 0)
                {
                    fprintf(stderr,
                        "ft_ping: invalid option -- '%c'\n", optopt);
                    fprintf(stderr,
                        "Try '%s -?' for more information.\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);

            default:
                fprintf(stderr, "ft_ping: unexpected error in option parsing\n");
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc)
    {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try '%s -?' for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (optind < argc - 1)
    {
        fprintf(stderr,
            "ft_ping: too many arguments -- only one host allowed\n");
        exit(EXIT_FAILURE);
    }
    opts->target = argv[optind];
}
