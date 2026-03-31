#include "permissions.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int parse_symbolic(const char *s, mode_t *mode)
{
    if (strlen(s) != 9) return -1;
    *mode = 0;
    if (s[0] == 'r') *mode |= S_IRUSR;
    if (s[1] == 'w') *mode |= S_IWUSR;
    if (s[2] == 'x') *mode |= S_IXUSR;
    if (s[3] == 'r') *mode |= S_IRGRP;
    if (s[4] == 'w') *mode |= S_IWGRP;
    if (s[5] == 'x') *mode |= S_IXGRP;
    if (s[6] == 'r') *mode |= S_IROTH;
    if (s[7] == 'w') *mode |= S_IWOTH;
    if (s[8] == 'x') *mode |= S_IXOTH;
    return 0;
}

void print_permissions(mode_t mode)
{
    printf("Symbolic representation: ");
    printf("%c%c%c", (mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-', (mode & S_IXUSR) ? 'x' : '-');
    printf("%c%c%c", (mode & S_IRGRP) ? 'r' : '-', (mode & S_IWGRP) ? 'w' : '-', (mode & S_IXGRP) ? 'x' : '-');
    printf("%c%c%c\n", (mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-', (mode & S_IXOTH) ? 'x' : '-');

    printf("Numeric representation: %03o\n", mode);

    printf("Bit representation: ");
    for (int i = 8; i >= 0; i--) {
        printf("%d", (mode & (1U << i)) ? 1 : 0);
    }
    printf("\n");
}

int parse_permissions(const char *input, mode_t *mode)
{
    size_t len = strlen(input);
    if (len == 9 && strpbrk(input, "rwx-")) {
        return parse_symbolic(input, mode);
    } else if (strspn(input, "01234567") == len) {
        long val = strtol(input, NULL, 8);
        if (val >= 0 && val <= 0777) {
            *mode = (mode_t)val;
            return 0;
        }
    }
    return -1;
}

void apply_chmod_command(mode_t *mode, const char *cmd)
{
    if (strlen(cmd) < 3) {
        printf("Error: command too short!\n");
        return;
    }
    char who = cmd[0];
    char op  = cmd[1];
    const char *perm_str = &cmd[2];

    if (strchr("ugoa", who) == NULL || strchr("+-=", op) == NULL) {
        printf("Error: command format must be who op perms (u/g/o/a, +/-/=, r/w/x)\n");
        return;
    }

    mode_t who_mask = 0;
    if (who == 'u' || who == 'a') who_mask |= S_IRWXU;
    if (who == 'g' || who == 'a') who_mask |= S_IRWXG;
    if (who == 'o' || who == 'a') who_mask |= S_IRWXO;

    mode_t perm_val = 0;
    if (strchr(perm_str, 'r')) perm_val |= (who_mask & 0444);
    if (strchr(perm_str, 'w')) perm_val |= (who_mask & 0222);
    if (strchr(perm_str, 'x')) perm_val |= (who_mask & 0111);

    if (op == '+') {
        *mode |= perm_val;
    } else if (op == '-') {
        *mode &= ~perm_val;
    } else if (op == '=') {
        *mode &= ~who_mask;
        *mode |= perm_val;
    }
}
