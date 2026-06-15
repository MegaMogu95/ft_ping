#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include "options.h"

/*
** Long-only options (--ttl, --ip-timestamp) use OPT_* values as their
** return value so they go through the same switch as short options.
** --help and --usage both map to '?' to mirror inetutils behavior.
*/
static const struct option g_long_opts[] = {
    {"ttl",          required_argument, 0, OPT_TTL         },
    {"ip-timestamp", required_argument, 0, OPT_IP_TIMESTAMP},
    {"help",         no_argument,       0, '?'             },
    {"usage",        no_argument,       0, '?'             },
    {0,              0,                 0,  0              }
};

void    print_usage(const char *prog)
{
    printf("Usage: %s [OPTION...] HOST ...\n", prog);
    printf("Send ICMP ECHO_REQUEST packets to network hosts.

 Options available:
  -v, --verbose              verbose output
  -?, --help                 give this help list

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <akrings@student.42.fr>.
");
    return 0;
}

/*
** parse_long: safe strtol wrapper.
** Exits with an error if the string is not a valid integer in [min, max].
*/
static long parse_long(const char *str, const char *opt, long min, long max)
{
    char    *end;
    long    val;

    errno = 0;
    val = strtol(str, &end, 10);
    if (errno != 0 || *end != '\0' || end == str)
    {
        fprintf(stderr, "ft_ping: invalid argument for %s: '%s'\n", opt, str);
        exit(EXIT_FAILURE);
    }
    if (val < min || val > max)
    {
        fprintf(stderr,
            "ft_ping: %s value out of range [%ld, %ld]: '%s'\n",
            opt, min, max, str);
        exit(EXIT_FAILURE);
    }
    return val;
}

/*
** parse_pattern: read up to 16 hex bytes from a string like "ff0a3b".
** Odd-length strings are left-padded with a zero ("fa3" → "0fa3").
** Fills opts->pattern and sets opts->pattern_set.
*/
static void parse_pattern(const char *str, uint8_t *pattern)
{
    char    padded[33];
    size_t  len;
    size_t  i;
    char    byte_str[3];
    char    *end;

    len = strlen(str);
    if (len == 0 || len > 32)
    {
        fprintf(stderr, "ft_ping: pattern must be 1 to 16 hex bytes\n");
        exit(EXIT_FAILURE);
    }
    memset(padded, 0, sizeof(padded));
    if (len % 2)
    {
        padded[0] = '0';
        strcpy(padded + 1, str);
        len++;
    }
    else
        strcpy(padded, str);

    memset(pattern, 0, 16);
    i = 0;
    while (i < len / 2)
    {
        byte_str[0] = padded[i * 2];
        byte_str[1] = padded[i * 2 + 1];
        byte_str[2] = '\0';
        errno = 0;
        pattern[i] = (uint8_t)strtol(byte_str, &end, 16);
        if (errno != 0 || *end != '\0')
        {
            fprintf(stderr, "ft_ping: invalid hex pattern: '%s'\n", str);
            exit(EXIT_FAILURE);
        }
        i++;
    }
}

void    parse_options(int argc, char **argv, t_opts *opts)
{
    int opt;
    int longindex;

    memset(opts, 0, sizeof(t_opts));
    opts->size    = DEFAULT_SIZE;
    opts->ttl     = DEFAULT_TTL;
    opts->timeout = DEFAULT_TIMEOUT;

    /*
    ** We set opterr = 0 to suppress getopt's own error messages and handle
    ** them ourselves. This also lets us distinguish between:
    **   - the user passing '-?' intentionally  → opt == '?', optopt == 0
    **   - getopt signaling an unknown option   → opt == '?', optopt != 0
    */
    opterr = 0;

    while ((opt = getopt_long(argc, argv, "v?fnrl:w:W:p:s:T:",
                              g_long_opts, &longindex)) != -1)
    {
        switch (opt)
        {
            /* ── mandatory ─────────────────────────────────────── */
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
            "ft_ping: too many arguments — only one host allowed\n");
        exit(EXIT_FAILURE);
    }
    opts->target = argv[optind];
}
