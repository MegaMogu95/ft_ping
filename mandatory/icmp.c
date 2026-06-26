#include "in_checksum.h"
#include <netinet/ip_icmp.h>
#include <string.h>
#include <unistd.h>

void	build_icmp_header(struct icmphdr *icmp, u_int16_t seq)
{
	memset(icmp, 0, sizeof(*icmp));

	icmp->type				= ICMP_ECHO;
	icmp->code				= 0;
	icmp->un.echo.id		= getpid() & 0xFFFF;
	icmp->un.echo.sequence	= seq;
	icmp->checksum	 		= in_checksum(&icmp, sizeof(icmp));
}