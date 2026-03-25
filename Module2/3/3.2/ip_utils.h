#ifndef IP_UTILS_H
#define IP_UTILS_H

#include <stdint.h>

uint32_t parse_ip(const char *str);
void print_ip(uint32_t ip);
int is_in_subnet(uint32_t dest, uint32_t gw, uint32_t mask);

#endif