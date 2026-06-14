#include <stdint.h>

uint16_t ipv4_checksum(int count, void* addr)
{
    uint32_t	sum = 0;
	uint16_t	*chunks = addr;

    while (count > 1)
	{
        sum += *chunks++;
        count -= 2;
    }

    if (count > 0)
        sum += * (unsigned char *) addr;

    while (sum >> 16)
	{
        sum = (sum & 0xffff) + (sum >> 16);
    }

	return (~sum);
}