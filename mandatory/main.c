#include "dns.h"
#include "parsing.h"
#include "socket.h"
#include "icmp.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    t_opts              opts;
    int                 sockfd;
    struct sockaddr_in  addr;
    char                ip[INET_ADDRSTRLEN];
	struct icmphdr		icmp;
	u_int16_t			seq;

    parse_options(argc, argv, &opts);
    sockfd = create_socket(DEFAULT_TTL, DEFAULT_TIMEOUT);
    resolve_host(opts.target, &addr, ip, INET_ADDRSTRLEN);
    if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr, "ft_ping: connect: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
	seq = 0;
	build_icmp_header(&icmp, seq);
	write(sockfd, &icmp, sizeof(icmp));
}