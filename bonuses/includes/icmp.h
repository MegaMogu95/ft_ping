#ifndef ICMP_H
# define ICMP_H

# include <netinet/ip_icmp.h>
# include <stdint.h>

/*
** An ICMP echo message is an 8-byte ICMP header followed by the data
** payload. The default ping payload is 56 bytes ("56 data bytes"), giving a
** 64-byte ICMP message. The first sizeof(struct timeval) bytes of the data
** carry the send timestamp used to compute the round-trip time.
*/
# define ICMP_HDRLEN  8
# define ICMP_DATALEN 56
# define ICMP_PKTLEN  (ICMP_HDRLEN + ICMP_DATALEN)

void	build_icmp_packet(char *packet, uint16_t seq,
			const unsigned char *pattern, int pattern_len);

#endif
