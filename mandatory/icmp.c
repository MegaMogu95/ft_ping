#include "in_checksum.h"
#include <netinet/ip_icmp.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

void	build_icmp_packet(char packet[56], u_int16_t seq)
{
	struct icmphdr	*icmp;
	struct timeval	*timestamp;

	memset(packet, 0, 56);

	icmp 					= (struct icmphdr *)packet;
	icmp->type				= ICMP_ECHO;
	icmp->code				= 0;
	icmp->un.echo.id		= getpid() & 0xFFFF;
	icmp->un.echo.sequence	= seq;

	timestamp = (struct timeval *)(packet + 8);
	gettimeofday(timestamp, NULL);

	icmp->checksum	= in_checksum(packet, 56);
}