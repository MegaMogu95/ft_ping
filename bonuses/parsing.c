#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include "parsing.h"

/*
** Long options:
**   --verbose       -> 'v'
**   --help/--usage  -> '?'  (both map to the help screen, like inetutils)
**   --preload=N     -> 'l'  (send N packets as fast as possible first)
**   --timeout=N     -> 'w'  (stop after N seconds)
**   --count=N       -> 'c'  (stop after sending N packets)
*/
static const struct option g_long_opts[] = {
    {"verbose", no_argument,       0, 'v'},
    {"help",    no_argument,       0, '?'},
    {"usage",   no_argument,       0, '?'},
    {"preload", required_argument, 0, 'l'},
    {"timeout", required_argument, 0, 'w'},
    {"count",   required_argument, 0, 'c'},
    {0,         0,                 0,  0 }
};

void    print_usage(const char *prog)
{
    printf("Usage: %s [OPTION...] HOST ...\n", prog);
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
    printf("\n");
    printf("  -c, --count=NUMBER         stop after sending NUMBER packets\n");
    printf("  -l, --preload=NUMBER       send NUMBER packets as fast as "
           "possible before\n");
    printf("                             falling into normal mode of "
           "behavior\n");
    printf("  -w, --timeout=N            stop after N seconds\n");
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
    int             opt;
    int             longindex;
    char            *endptr;
    unsigned long   preload;
    unsigned long   timeout;
    unsigned long   count;

    memset(opts, 0, sizeof(t_opts));

    /*
    ** We set opterr = 0 to suppress getopt's own error messages and handle
    ** them ourselves. This also lets us distinguish between:
    **   - the user passing '-?' intentionally  -> opt == '?', optopt == 0
    **   - getopt signaling an unknown option   -> opt == '?', optopt != 0
    */
    opterr = 0;

    while ((opt = getopt_long(argc, argv, "v?l:w:c:", g_long_opts,
                              &longindex)) != -1)
    {
        switch (opt)
        {
            case 'v':
                opts->verbose = 1;
                break ;

            case 'c':
                /*
                ** Mirrors inetutils ping_cvt_number(arg, 0, 1): zero is
                ** allowed (it means "no limit"); only trailing garbage is
                ** rejected. We additionally cap at INT_MAX for our int field.
                */
                count = strtoul(optarg, &endptr, 0);
                if (*endptr != '\0')
                {
                    fprintf(stderr,
                        "ft_ping: invalid value (`%s' near `%s')\n",
                        optarg, endptr);
                    exit(EXIT_FAILURE);
                }
                if (count > INT_MAX)
                {
                    fprintf(stderr,
                        "ft_ping: option value too big: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                opts->count = (int)count;
                break ;

            case 'l':
                /*
                ** strtoul with base 0 accepts decimal/octal/hex, like
                ** inetutils. Reject trailing garbage or values above INT_MAX.
                */
                preload = strtoul(optarg, &endptr, 0);
                if (*endptr != '\0' || preload > INT_MAX)
                {
                    fprintf(stderr,
                        "ft_ping: invalid preload value (%s)\n", optarg);
                    exit(EXIT_FAILURE);
                }
                opts->preload = (int)preload;
                break ;

            case 'w':
                /*
                ** Mirrors inetutils ping_cvt_number(arg, INT_MAX, 0):
                ** reject trailing garbage, a zero value, and overflow.
                */
                timeout = strtoul(optarg, &endptr, 0);
                if (*endptr != '\0')
                {
                    fprintf(stderr,
                        "ft_ping: invalid value (`%s' near `%s')\n",
                        optarg, endptr);
                    exit(EXIT_FAILURE);
                }
                if (timeout == 0)
                {
                    fprintf(stderr,
                        "ft_ping: option value too small: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                if (timeout > INT_MAX)
                {
                    fprintf(stderr,
                        "ft_ping: option value too big: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                opts->timeout = (int)timeout;
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
