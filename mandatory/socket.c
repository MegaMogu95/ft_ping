#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "socket.h"

/*
** create_socket: raw ICMP socket used to send (and receive) echo packets.
** We set IP_TTL on the outgoing packets.
*/
int create_socket(int ttl)
{
    int             sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        fprintf(stderr, "ft_ping: setsockopt IP_TTL: %s\n", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    return (sockfd);
}
