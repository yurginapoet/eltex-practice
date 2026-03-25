#include "fperm.h"
#include <sys/stat.h>
#include <stdio.h>

mode_t get_file_permissions(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("stat");
        return (mode_t)-1;
    }
    return st.st_mode & 0777;
}