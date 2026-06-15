#ifndef OPTIONS_H
# define OPTIONS_H

# include <stdint.h>

# define DEFAULT_TTL      64
# define DEFAULT_SIZE     56
# define DEFAULT_TIMEOUT  1

/*
** OPT_TTL and OPT_IP_TIMESTAMP are values returned by getopt_long()
** for long-only options. They must be outside the ASCII range (> 255)
** to avoid collisions with short option characters.
*/
# define OPT_TTL           256
# define OPT_IP_TIMESTAMP  257

typedef struct s_opts
{
    int         verbose;
    char        *target;
}   t_opts;

void    parse_options(int argc, char **argv, t_opts *opts);
void    print_usage(const char *prog);

#endif