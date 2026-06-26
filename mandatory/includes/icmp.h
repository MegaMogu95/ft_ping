#ifndef ICMP_H
# define ICMP_H

# include <netinet/ip_icmp.h>

void	build_icmp_header(struct icmphdr *icmp_hdr, u_int16_t seq);

#endif