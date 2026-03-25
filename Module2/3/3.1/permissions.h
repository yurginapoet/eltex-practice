#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <sys/types.h>

void print_permissions(mode_t mode);
int parse_permissions(const char *input, mode_t *mode);
void apply_chmod_command(mode_t *mode, const char *cmd);

#endif