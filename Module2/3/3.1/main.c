#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "permissions.h"
#include "fperm.h"

int main(void)
{
    mode_t current_mode = 0;
    int has_mode = 0;
    char input[256];

    while (1) {
        printf("\n=== Permission Mask Calculator ===\n");
        printf("1. Enter permissions (symbolic or numeric)\n");
        printf("2. Enter filename to display permissions (via stat)\n");
        printf("3. Modify permissions (chmod simulation)\n");
        printf("4. Exit\n");
        printf("Choose an option: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                printf("Enter permissions (rwxr-xr-x or 755): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                if (parse_permissions(input, &current_mode) == 0) {
                    has_mode = 1;
                    printf("Permissions set successfully!\n");
                    print_permissions(current_mode);
                } else {
                    printf("Error: invalid permissions format!\n");
                }
                break;

            case 2:
                printf("Enter filename: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                mode_t m = get_file_permissions(input);
                if (m != (mode_t)-1) {
                    current_mode = m;
                    has_mode = 1;
                    printf("File permissions read (compare with ls -l %s):\n", input);
                    print_permissions(current_mode);
                } else {
                    printf("Error reading file permissions!\n");
                }
                break;

            case 3:
                if (!has_mode) {
                    printf("Please set initial permissions first (options 1 or 2)!\n");
                    break;
                }
                printf("Enter modification command (u+x / g-r / a=rwx etc.): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                apply_chmod_command(&current_mode, input);
                printf("Permissions updated (simulation; real file NOT changed)!\n");
                print_permissions(current_mode);
                break;

            case 4:
                printf("Exiting.\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }
}
