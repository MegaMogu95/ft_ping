#include <netinet/ip_icmp.h>

struct icmp	create_icmp_header(t_ping &ping)
{
	struct icmp	icmp_hdr;

	memset(&icmp_hdr, 0, sizeof(icmp_hdr));

	icmp_hdr.icmp_type	= ICMP_ECHO;
	icmp_hdr.icmp_code	= 0;
	icmp_hdr.icmp_id	= getpid() & 0xFFFF;
	// icmp_hdr.icmp_seq	= seq++;
	// optionally put a timestamp in icmp_data for RTT measurement
	icmp_hdr.icmp_cksum = 0;
	icmp_hdr.icmp_cksum = checksum(sizeof(icmp_hdr), &icmp_hdr);
	
	return (icmp_hdr);
}