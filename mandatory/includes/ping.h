#ifndef PING_H
# define PING_H

# include <signal.h>
# include <netinet/in.h>
# include "parsing.h"

/*
** run_ping: paced send + receive loop on a raw ICMP socket. Sends one echo
** request per second (driven by SIGALRM) to addr and receives, validates and
** reports replies (and ICMP error messages) until g_running becomes 0. Prints
** the final statistics block before returning.
*/
void	run_ping(int sockfd, const t_opts *opts, const char *ip,
			const struct sockaddr_in *addr);

#endif
