#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "ip_utils.h"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <gateway_ip> <subnet_mask> <N>\n", argv[0]);
        fprintf(stderr, "Example: %s 192.168.1.1 255.255.255.0 100\n", argv[0]);
        return 1;
    }

    uint32_t gw_ip       = parse_ip(argv[1]);
    uint32_t subnet_mask = parse_ip(argv[2]);
    int N = atoi(argv[3]);

    if (gw_ip == 0 || N <= 0) {
        fprintf(stderr, "Error: invalid parameters!\n");
        return 1;
    }

    if (subnet_mask == 0 && strcmp(argv[2], "0.0.0.0") != 0) {
        fprintf(stderr, "Error: invalid parameters!\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    int own_count = 0;
    printf("Processing %d packets...\n\n", N);

    for (int i = 0; i < N; i++) {
        uint8_t o1 = rand() % 256;
        uint8_t o2 = rand() % 256;
        uint8_t o3 = rand() % 256;
        uint8_t o4 = rand() % 256;

        uint32_t dest_ip = ((uint32_t)o1 << 24) | 
                           ((uint32_t)o2 << 16) | 
                           ((uint32_t)o3 << 8)  | o4;

        printf("Packet %d: destination IP ", i + 1);
        print_ip(dest_ip);

        if (is_in_subnet(dest_ip, gw_ip, subnet_mask)) {
            printf(" - belongs to own subnet\n");
            own_count++;
        } else {
            printf(" - belongs to another subnet\n");
        }
    }

    printf("Packets destined for nodes in own subnet: %d (%.2f%%)\n",
           own_count, N > 0 ? (double)own_count / N * 100.0 : 0.0);

    return 0;
}