#ifndef IN_CHECKSUM_H
# define IN_CHECKSUM_H

# include <stdint.h>
# include <stddef.h>

uint16_t in_checksum(const void *data, size_t len);

#endif