#ifndef PING_H
# define PING_H

# include <signal.h>
# include "parsing.h"

/*
** run_ping: paced send + receive loop on an already connected raw socket.
** Sends one echo request per second (driven by SIGALRM) and receives,
** validates and reports replies until g_running becomes 0. Prints the final
** statistics block before returning.
*/
void	run_ping(int sockfd, const t_opts *opts, const char *ip);

#endif
