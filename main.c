#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <math.h>           // sqrt() for stddev — link with -lm

// Networking
#include <sys/types.h>
#include <sys/socket.h>     // socket(), sendto(), recvfrom()
#include <netinet/in.h>     // struct sockaddr_in, IPPROTO_ICMP
#include <netinet/ip.h>     // struct ip (IP header)
#include <netinet/ip_icmp.h>// struct icmp, ICMP_ECHO, ICMP_ECHOREPLY
#include <arpa/inet.h>      // inet_ntoa(), inet_pton()
#include <netdb.h>          // getaddrinfo(), gethostbyname()
#include <sys/time.h>       // gettimeofday(), struct timeval

uint16_t ipv4_checksum(int count, void* addr);

int	main()
{
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); //check errno
	int ttl = 64;
	setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	struct icmp icmp_hdr;
	memset(&icmp_hdr, 0, sizeof(icmp_hdr));
	icmp_hdr.icmp_type	= ICMP_ECHO;
	icmp_hdr.icmp_code	= 0;
	icmp_hdr.icmp_id	= getpid() & 0xFFFF; // identify your packets
	// icmp_hdr.icmp_seq	= seq++;
	// optionally put a timestamp in icmp_data for RTT measurement
	icmp_hdr.icmp_cksum = 0;
	icmp_hdr.icmp_cksum = ipv4_checksum(sizeof(icmp_hdr), &icmp_hdr);
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;        // IPv4
	dest.sin_port = htons(8080);      // port must be in network byte order
	inet_pton(AF_INET, "142.251.39.110", &dest.sin_addr);
	sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0,
       (struct sockaddr *)&dest, sizeof(dest));
	char recv_buf[1024];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0,
						(struct sockaddr *)&from, &fromlen);

	// Parse: recv_buf starts with the IP header
	struct ip   *ip_hdr   = (struct ip *)recv_buf;
	int          ip_hlen  = ip_hdr->ip_hl * 4;  // header length in bytes
	struct icmp *icmp_reply = (struct icmp *)(recv_buf + ip_hlen);
	return (icmp_reply->icmp_type == ICMP_ECHOREPLY && icmp_reply->icmp_id==getpid());
}