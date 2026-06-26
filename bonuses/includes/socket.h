#ifndef SOCKET_H
# define SOCKET_H

# define DEFAULT_TTL     64
# define DEFAULT_TIMEOUT 1

/*
** create_socket: open a raw IPv4 socket for ICMP.
**
** The socket is created with IPPROTO_ICMP so the caller is responsible for
** building the ICMP message itself; the kernel still prepends the IP header
** on send. Requires CAP_NET_RAW (root or a setuid binary).
**
**   ttl : value for the outgoing IP TTL (e.g. DEFAULT_TTL)
**
** The socket is left blocking with no receive timeout, so recv() blocks until
** a packet arrives or a signal interrupts it (EINTR).
**
** Returns the socket fd on success, or -1 on failure (after printing why).
*/
int create_socket(int ttl);

#endif
