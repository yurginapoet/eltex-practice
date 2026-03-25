#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contacts.h"

static int read_int(void) {
    char buf[MAX_STR];
    char *endptr;
    long value;

    if (!fgets(buf, sizeof(buf), stdin))
        return -1;

    value = strtol(buf, &endptr, 10);
    while (*endptr == ' ' || *endptr == '\t') 
        endptr++;
    if (endptr == buf || (*endptr != '\0' && *endptr != '\n')) 
        return -1;

    return (int)value;
}

static void read_line(char *buf, size_t size) {
    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

int main() {
    ContactList list;
    init_list(&list);

    int choice;

    while (1) {
        printf("\n1. Add\n2. Show\n3. Edit\n4. Delete\n5. Sort\n0. Exit\n");
        printf("Choice: ");
        choice = read_int();

        if (choice == 0) break;

        if (choice == 1) add_contact(&list);
        else if (choice == 2) print_contacts(&list);
        else if (choice == 3 || choice == 4) {
            char first[MAX_STR], last[MAX_STR];

            printf("First name: ");
            read_line(first, sizeof(first));

            printf("Last name (leave empty to search only by first name): ");
            read_line(last, sizeof(last));

            if (choice == 3) edit_contact(&list, first, last);
            else delete_contact(&list, first, last);
        } else if (choice == 5) sort_contacts(&list);
        else printf("Invalid choice\n");
    }

    return 0;
}
