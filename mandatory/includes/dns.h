#ifndef DNS_H
# define DNS_H

# include <netinet/in.h>
# include <stddef.h>

/*
** resolve_host: resolve a hostname (or a literal IPv4 string) to an IPv4
** address usable with sendto().
**
**   host    : name or dotted-decimal string from the command line
**   addr    : filled with the resolved AF_INET address (may be NULL)
**   ip      : buffer to receive the printable dotted-decimal IP (may be NULL)
**   ip_size : size of the ip buffer (use INET_ADDRSTRLEN)
**   On failure it prints "ft_ping: unknown host"
**   (matching the real ping) and exits with EXIT_FAILURE
*/
void    resolve_host(const char *host, struct sockaddr_in *addr,
                 char *ip, size_t ip_size);

#endif
