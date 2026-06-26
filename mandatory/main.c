#include "dns.h"
#include "parsing.h"
#include "socket.h"
#include "icmp.h"
#include "ping.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    t_opts              opts;
    int                 sockfd;
    struct sockaddr_in  addr;
    char                ip[INET_ADDRSTRLEN];

    parse_options(argc, argv, &opts);
    sockfd = create_socket(DEFAULT_TTL);
    resolve_host(opts.target, &addr, ip, INET_ADDRSTRLEN);
    if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr, "ft_ping: connect: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("PING %s (%s): %d data bytes", opts.target, ip, ICMP_DATALEN);
    if (opts.verbose)
        printf(", id 0x%04x = %u", getpid() & 0xFFFF, getpid() & 0xFFFF);
    printf("\n");
    run_ping(sockfd, &opts, ip);
    close(sockfd);
    return (0);
}
