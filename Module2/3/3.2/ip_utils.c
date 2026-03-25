#include "ip_utils.h"
#include <stdio.h>
#include <string.h>

uint32_t parse_ip(const char *str)
{
    unsigned int a, b, c, d;
    if (sscanf(str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4 ||
        a > 255 || b > 255 || c > 255 || d > 255) {
        return 0;
    }
    return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | d;
}

void print_ip(uint32_t ip)
{
    printf("%u.%u.%u.%u", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
}

int is_in_subnet(uint32_t dest, uint32_t gw, uint32_t mask)
{
    return (dest & mask) == (gw & mask);
}