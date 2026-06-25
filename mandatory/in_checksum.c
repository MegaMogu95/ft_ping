#include <stdint.h>
#include <stddef.h>

uint16_t in_checksum(const void *data, size_t len)
{
        const uint16_t  *p;
        uint32_t                sum;

        p = data;
        sum = 0;
        while (len > 1)
        {
                sum += *p++;
                len -= 2;
        }
        if (len)
                sum += *(const uint8_t *)p;
        while (sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);
        return ((uint16_t)~sum);
}