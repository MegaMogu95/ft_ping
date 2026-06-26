#ifndef ICMP_H
# define ICMP_H

# include <netinet/ip_icmp.h>

void	build_icmp_packet(char packet[56], u_int16_t seq);

#endif