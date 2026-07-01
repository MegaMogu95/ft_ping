#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "dns.h"

void    resolve_host(const char *host, struct sockaddr_in *addr,
                 char *ip, size_t ip_size)
{
    struct addrinfo     hints;
    struct addrinfo     *res;
    struct sockaddr_in  *sin;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if (getaddrinfo(host, NULL, &hints, &res) != 0 || res == NULL)
    {
        fprintf(stderr, "ft_ping: unknown host\n");
        exit(EXIT_FAILURE);
    }

    sin = (struct sockaddr_in *)res->ai_addr;
    if (addr != NULL)
        memcpy(addr, sin, sizeof(struct sockaddr_in));
    if (ip != NULL && ip_size > 0)
        inet_ntop(AF_INET, &sin->sin_addr, ip, ip_size);

    freeaddrinfo(res);
}