#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "dns.h"

/*
** resolve_host: resolve a hostname (or a literal IPv4 string) into an
** AF_INET address. We restrict the lookup to IPv4 / raw ICMP, which is
** what ping ultimately sends. getaddrinfo() also accepts a dotted-decimal
** string directly, so "127.0.0.1" works without a name server round-trip.
*/
int resolve_host(const char *host, struct sockaddr_in *addr,
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
        return (-1);
    }

    sin = (struct sockaddr_in *)res->ai_addr;
    if (addr != NULL)
        memcpy(addr, sin, sizeof(struct sockaddr_in));
    if (ip != NULL && ip_size > 0)
        inet_ntop(AF_INET, &sin->sin_addr, ip, ip_size);

    freeaddrinfo(res);
    return (0);
}
