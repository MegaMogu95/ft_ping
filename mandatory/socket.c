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
** We set IP_TTL on the outgoing packets and SO_RCVTIMEO so that a recvfrom()
** with no reply gives up after timeout_sec seconds (returning -1/EAGAIN)
** instead of blocking forever. The ICMP payload itself is left to the caller.
*/
int create_socket(int ttl, int timeout_sec)
{
    int             sockfd;
    struct timeval  tv;

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
    tv.tv_sec  = timeout_sec;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        fprintf(stderr, "ft_ping: setsockopt SO_RCVTIMEO: %s\n",
            strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    return (sockfd);
}
