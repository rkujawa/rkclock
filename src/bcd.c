#include <stdint.h>

#include "bcd.h"

static uint8_t
median(uint8_t x, uint8_t y, uint8_t z)
{
     return (x & (y | z)) | (y & z);
}

/*
 * BCD addition based on Knuth's TAOCP Vol.4A Part 1, adapted to 8-bit.
 */
uint8_t 
bcd_add(uint8_t x, uint8_t y)
{
    uint8_t z, u, t;
    z = y + 0x66;
    u = x + z;
    t = median(~x, ~z, u) & 0x88;
    return u - t + (t >> 2);
}

