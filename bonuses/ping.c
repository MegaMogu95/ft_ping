#include "ping.h"
#include "icmp.h"
#include "in_checksum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

static volatile sig_atomic_t	g_running = 1;
static volatile sig_atomic_t	g_send = 0;

/*
** State shared between the loop and the report function. g_seen is a bitmap
** of sequence numbers already reported (duplicate check).
*/
static uint16_t			g_seq = 0;
static uint8_t			g_seen[65536 / 8];
static int				g_transmitted = 0;
static int				g_received = 0;
static int				g_duplicates = 0;
static int				g_rtt_count = 0;
static double			g_rtt_min = 0;
static double			g_rtt_max = 0;
static double			g_rtt_sum = 0;

static void	handle_sigint(int sig)
{
	(void)sig;
	g_running = 0;
}

static void	handle_sigalrm(int sig)
{
	(void)sig;
	g_send = 1;
	alarm(1);
}

/*
** Signals are installed without SA_RESTART so that a blocking recv() is
** interrupted (EINTR) by SIGALRM/SIGINT, letting the loop send the next
** packet or notice it must stop.
*/
static void	setup_signals(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		fprintf(stderr, "ft_ping: sigaction SIGINT: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = handle_sigalrm;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
	{
		fprintf(stderr, "ft_ping: sigaction SIGALRM: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static int	seq_seen(uint16_t seq)
{
	return ((g_seen[seq >> 3] >> (seq & 7)) & 1);
}

static void	seq_mark(uint16_t seq)
{
	g_seen[seq >> 3] |= (uint8_t)(1 << (seq & 7));
}

static void	seq_clear(uint16_t seq)
{
	g_seen[seq >> 3] &= (uint8_t)~(1 << (seq & 7));
}

static void	send_packet(int sockfd, const t_opts *opts)
{
	char	packet[ICMP_PKTLEN];

	/*
	** Clear the seen-bit for the sequence we are about to send, like
	** inetutils' _PING_CLR. A stale or forged reply for this sequence is
	** thus forgotten, so the genuine reply is counted fresh rather than as
	** a duplicate.
	*/
	seq_clear(g_seq);
	build_icmp_packet(packet, g_seq, opts->pattern, opts->pattern_len);
	if (send(sockfd, packet, ICMP_PKTLEN, 0) >= 0)
		g_transmitted++;
	g_seq++;
}

/*
** report: validate one received datagram and print a ping line for it.
** Skips packets that are not echo replies or not addressed to us. Damage is
** detected the way inetutils does it: recomputing the ICMP checksum over the
** whole message. Because the stored checksum is included in the sum, an
** intact packet folds to 0; anything else means corruption, and a "checksum
** mismatch" warning is printed (the reply is still shown and counted, as in
** the real ping). Duplicates are detected via the already-seen bitmap.
*/
static void	report(const char *buf, ssize_t len, const char *ip)
{
	const struct iphdr		*iph;
	const struct icmphdr	*icmp;
	const struct timeval	*recv_tv;
	struct timeval			now;
	size_t					iphlen;
	size_t					icmplen;
	uint16_t				seq;
	double					rtt;
	int						dup;
	int						timing;

	iph = (const struct iphdr *)buf;
	iphlen = (size_t)iph->ihl * 4;
	if ((size_t)len < iphlen + ICMP_HDRLEN)
		return ;
	icmp = (const struct icmphdr *)(buf + iphlen);
	if (icmp->type != ICMP_ECHOREPLY
		|| ntohs(icmp->un.echo.id) != (getpid() & 0xFFFF))
		return ;
	icmplen = (size_t)len - iphlen;
	seq = ntohs(icmp->un.echo.sequence);
	if (in_checksum(icmp, icmplen) != 0)
		fprintf(stderr, "checksum mismatch from %s\n", ip);
	dup = seq_seen(seq);
	/*
	** A reply is only timed when its payload is large enough to hold the
	** send timestamp. Smaller packets are still reported, just without the
	** RTT (matching inetutils' PING_TIMING check).
	*/
	rtt = 0;
	timing = (icmplen >= ICMP_HDRLEN + sizeof(struct timeval));
	if (timing)
	{
		recv_tv = (const struct timeval *)(buf + iphlen + ICMP_HDRLEN);
		gettimeofday(&now, NULL);
		rtt = (now.tv_sec - recv_tv->tv_sec) * 1000.0
			+ (now.tv_usec - recv_tv->tv_usec) / 1000.0;
	}
	printf("%zu bytes from %s: icmp_seq=%u ttl=%d",
		icmplen, ip, seq, iph->ttl);
	if (timing)
		printf(" time=%.3f ms", rtt);
	if (dup)
		printf(" (DUP!)");
	printf("\n");
	if (dup)
		g_duplicates++;
	else
	{
		seq_mark(seq);
		g_received++;
	}
	if (timing)
	{
		if (g_rtt_count == 0 || rtt < g_rtt_min)
			g_rtt_min = rtt;
		if (rtt > g_rtt_max)
			g_rtt_max = rtt;
		g_rtt_sum += rtt;
		g_rtt_count++;
	}
}

static void	print_stats(const char *target)
{
	printf("--- %s ping statistics ---\n", target);
	printf("%d packets transmitted, %d packets received, ",
		g_transmitted, g_received);
	if (g_duplicates > 0)
		printf("+%d duplicates, ", g_duplicates);
	if (g_transmitted > 0)
	{
		if (g_received > g_transmitted)
			printf("-- somebody is printing forged packets!");
		else
			printf("%d%% packet loss",
				(g_transmitted - g_received) * 100 / g_transmitted);
	}
	printf("\n");
	if (g_rtt_count > 0)
		printf("round-trip min/avg/max = %.3f/%.3f/%.3f ms\n",
			g_rtt_min, g_rtt_sum / g_rtt_count, g_rtt_max);
}

/*
** setup_timeout: arm a one-shot POSIX timer that delivers SIGINT after
** `timeout` seconds (the -w option). SIGINT is reused on purpose: its handler
** already clears g_running, so the loop stops and the statistics are printed,
** exactly as if the user had pressed Ctrl-C. A timeout of 0 means "no limit".
*/
static void	setup_timeout(int timeout)
{
	timer_t				timerid;
	struct sigevent		sev;
	struct itimerspec	its;

	if (timeout == 0)
		return ;
	memset(&sev, 0, sizeof(sev));
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGINT;
	if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1)
	{
		fprintf(stderr, "ft_ping: timer_create: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	memset(&its, 0, sizeof(its));
	its.it_value.tv_sec = timeout;
	if (timer_settime(timerid, 0, &its, NULL) == -1)
	{
		fprintf(stderr, "ft_ping: timer_settime: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void	run_ping(int sockfd, const t_opts *opts, const char *ip)
{
	char	buf[1024];
	ssize_t	len;
	int		i;

	setup_signals();
	setup_timeout(opts->timeout);
	i = 0;
	while (i < opts->preload)
	{
		send_packet(sockfd, opts);
		i++;
	}
	send_packet(sockfd, opts);
	alarm(1);
	while (g_running)
	{
		if (g_send)
		{
			g_send = 0;
			/*
			** Keep sending until we reach the requested count (0 = no
			** limit). Once all packets are sent, the next alarm tick gives
			** outstanding replies a one-second grace period, then exits.
			*/
			if (opts->count == 0 || g_transmitted < opts->count)
				send_packet(sockfd, opts);
			else
				break ;
		}
		len = recv(sockfd, buf, sizeof(buf), 0);
		if (len < 0)
		{
			if (errno == EINTR)
				continue ;
			fprintf(stderr, "ft_ping: recv: %s\n", strerror(errno));
			break ;
		}
		report(buf, len, ip);
		if (opts->count != 0 && g_received >= opts->count)
			break ;
	}
	print_stats(opts->target);
}
