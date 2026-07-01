#include "in_checksum.h"
#include "icmp.h"
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

void	build_icmp_packet(char *packet, uint16_t seq)
{
	struct icmphdr	*icmp;
	struct timeval	*timestamp;
	size_t			i;

	memset(packet, 0, ICMP_PKTLEN);

	icmp 					= (struct icmphdr *)packet;
	icmp->type				= ICMP_ECHO;
	icmp->code				= 0;
	icmp->un.echo.id		= htons(getpid() & 0xFFFF);
	icmp->un.echo.sequence	= htons(seq);

	timestamp = (struct timeval *)(packet + ICMP_HDRLEN);
	gettimeofday(timestamp, NULL);

	i = ICMP_HDRLEN + sizeof(struct timeval);
	while (i < ICMP_PKTLEN)
	{
		packet[i] = (char)(i - ICMP_HDRLEN - sizeof(struct timeval));
		i++;
	}

	icmp->checksum	= in_checksum(packet, ICMP_PKTLEN);
}
