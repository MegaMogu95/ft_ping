#include "in_checksum.h"
#include "icmp.h"
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

/*
** build_icmp_packet: fill a 64-byte buffer (8-byte ICMP header + 56-byte
** payload) with an ICMP echo request. id and sequence go on the wire in
** network byte order; the send timestamp is stored at the start of the
** payload so it is echoed back for the RTT computation. The checksum is
** computed last, over the whole message, with the checksum field at 0.
*/
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

	/*
	** Fill the payload after the timestamp with an incrementing byte
	** pattern (0, 1, 2, ...), like inetutils ping's data_buffer[i] = i.
	** The checksum below then covers this data, so any corruption of the
	** payload is caught by recomputing the checksum on receipt.
	*/
	i = ICMP_HDRLEN + sizeof(struct timeval);
	while (i < ICMP_PKTLEN)
	{
		packet[i] = (char)(i - ICMP_HDRLEN - sizeof(struct timeval));
		i++;
	}

	icmp->checksum	= in_checksum(packet, ICMP_PKTLEN);
}
