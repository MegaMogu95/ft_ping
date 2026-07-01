#ifndef PING_H
# define PING_H

# include <signal.h>
# include <netinet/in.h>
# include "parsing.h"

void	run_ping(int sockfd, const t_opts *opts, const char *ip,
			const struct sockaddr_in *addr);

#endif
