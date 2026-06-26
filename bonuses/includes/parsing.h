#ifndef PARSING_H
# define PARSING_H

# include <stdint.h>

typedef struct s_opts
{
    int     verbose;
    int     preload;
    char    *target;
}   t_opts;

void    parse_options(int argc, char **argv, t_opts *opts);
void    print_usage(const char *prog);

#endif
