#ifndef PARSING_H
# define PARSING_H

# include <stdint.h>

# define PATTERN_MAXLEN 16

typedef struct s_opts
{
    int             verbose;
    int             preload;
    int             timeout;
    int             count;
    int             ttl;
    int             pattern_len;
    unsigned char   pattern[PATTERN_MAXLEN];
    char            *target;
}   t_opts;

void    parse_options(int argc, char **argv, t_opts *opts);
void    print_usage(const char *prog);

#endif
