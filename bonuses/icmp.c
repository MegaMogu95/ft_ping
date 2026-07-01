#include "in_checksum.h"
#include "icmp.h"
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

void	build_icmp_packet(char *packet, uint16_t seq,
		const unsigned char *pattern, int pattern_len)
{
	struct icmphdr	*icmp;
	struct timeval	*timestamp;
	size_t			start;
	size_t			i;

	memset(packet, 0, ICMP_PKTLEN);

	icmp 					= (struct icmphdr *)packet;
	icmp->type				= ICMP_ECHO;
	icmp->code				= 0;
	icmp->un.echo.id		= htons(getpid() & 0xFFFF);
	icmp->un.echo.sequence	= htons(seq);

	timestamp = (struct timeval *)(packet + ICMP_HDRLEN);
	gettimeofday(timestamp, NULL);

	start = ICMP_HDRLEN + sizeof(struct timeval);
	i = start;
	while (i < ICMP_PKTLEN)
	{
		if (pattern_len > 0)
			packet[i] = (char)pattern[(i - start) % (size_t)pattern_len];
		else
			packet[i] = (char)(i - start);
		i++;
	}

	icmp->checksum	= in_checksum(packet, ICMP_PKTLEN);
}
