#include <stdio.h>
#include "parsing.h"

/*
** Standalone harness for parse_options. Builds the t_opts from argv and
** prints its content so the parsing logic can be inspected in isolation.
**
**   ./test_parsing -v google.com
**   ./test_parsing -?
*/
int main(int argc, char **argv)
{
    t_opts opts;

    parse_options(argc, argv, &opts);

    printf("=== parsed options ===\n");
    printf("verbose : %d\n", opts.verbose);
    printf("target  : %s\n", opts.target ? opts.target : "(none)");
    return 0;
}
