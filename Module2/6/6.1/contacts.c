#include "contacts.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void input_line(char *buf, size_t size) {
    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

static int parse_int(const char *text, int *out) {
    char *endptr;
    long value = strtol(text, &endptr, 10);

    while (*endptr == ' ' || *endptr == '\t') {
        endptr++;
    }
    if (*text == '\0' || *endptr != '\0') {
        return 0;
    }

    *out = (int)value;
    return 1;
}

int is_valid_email(const char *email) {
    const char *at;
    const char *dot_after_at;

    if (email == NULL || *email == '\0') {
        return 0;
    }

    at = strchr(email, '@');
    if (at == NULL || at == email) {
        return 0;
    }
    if (strchr(at + 1, '@') != NULL || *(at + 1) == '\0') {
        return 0;
    }

    dot_after_at = strchr(at + 1, '.');
    if (dot_after_at == NULL || dot_after_at == at + 1 || *(dot_after_at + 1) == '\0') {
        return 0;
    }

    return 1;
}

int is_valid_phone(const char *phone) {
    int digit_count = 0;

    if (phone == NULL || *phone == '\0') {
        return 0;
    }

    for (int i = 0; phone[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)phone[i];

        if (isdigit(ch)) {
            digit_count++;
            continue;
        }
        if (ch == '+' || ch == '-' || ch == '(' || ch == ')') {
            continue;
        }

        return 0;
    }

    return digit_count > 0;
}

static int read_count(const char *label, int max_count) {
    char buf[MAX_STR];
    int count;

    while (1) {
        printf("%s count (0-%d): ", label, max_count);
        input_line(buf, sizeof(buf));

        if (parse_int(buf, &count) && count >= 0 && count <= max_count) {
            return count;
        }

        printf("Invalid number\n");
    }
}

static void read_nonempty(const char *label, char *dst) {
    while (1) {
        printf("%s: ", label);
        input_line(dst, MAX_STR);
        if (strlen(dst) > 0) {
            return;
        }
        printf("Field is required\n");
    }
}

static void read_optional(const char *label, char *dst) {
    printf("%s: ", label);
    input_line(dst, MAX_STR);
}

static int validate_item(const char *label, const char *value) {
    if (strcmp(label, "Phone") == 0) {
        return is_valid_phone(value);
    }
    if (strcmp(label, "Email") == 0) {
        return is_valid_email(value);
    }
    return 1;
}

static const char *validation_error(const char *label) {
    if (strcmp(label, "Phone") == 0) {
        return "Phone must contain only digits, parentheses, '-' and '+'";
    }
    if (strcmp(label, "Email") == 0) {
        return "Email must contain '@' and a domain part";
    }
    return "Invalid value";
}

static void read_items(const char *label, char items[][MAX_STR], int *count, int max_count) {
    *count = read_count(label, max_count);

    for (int i = 0; i < *count; i++) {
        printf("%s #%d: ", label, i + 1);
        input_line(items[i], MAX_STR);
        if (strlen(items[i]) == 0) {
            i--;
            printf("Field is required\n");
            continue;
        }
        if (!validate_item(label, items[i])) {
            i--;
            printf("%s\n", validation_error(label));
        }
    }
}

static void print_items(const char *label, const char items[][MAX_STR], int count) {
    for (int i = 0; i < count; i++) {
        printf("%s #%d: %s\n", label, i + 1, items[i]);
    }
}

static int contact_cmp(const Contact *left, const Contact *right) {
    int last_cmp = strcmp(left->last_name, right->last_name);
    if (last_cmp != 0) {
        return last_cmp;
    }
    return strcmp(left->first_name, right->first_name);
}

static Item *insert_sorted(Item *head, const Contact *contact, int *inserted) {
    Item *node = (Item *)malloc(sizeof(Item));
    if (node == NULL) {
        *inserted = 0;
        return head;
    }
    *inserted = 1;
    node->data = *contact;

    if (head == NULL) {
        node->next = node;
        node->prev = node;
        return node;
    }

    Item *cur = head;
    do {
        if (contact_cmp(&node->data, &cur->data) <= 0) {
            node->next = cur;
            node->prev = cur->prev;
            cur->prev->next = node;
            cur->prev = node;
            return (cur == head) ? node : head;
        }
        cur = cur->next;
    } while (cur != head);

    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
    return head;
}

void init_list(ContactList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void free_list(ContactList *list) {
    if (list == NULL || list->head == NULL) {
        return;
    }

    Item *cur = list->head->next;
    while (cur != list->head) {
        Item *next = cur->next;
        free(cur);
        cur = next;
    }

    free(list->head);
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

int find_contacts_by_name(const ContactList *list, const char *first, const char *last,
                          int matches[], int max_matches) {
    int need_last_name = last != NULL && strlen(last) > 0;
    int match_count = 0;

    if (list == NULL || first == NULL || matches == NULL || max_matches <= 0) {
        return 0;
    }
    if (list->head == NULL) {
        return 0;
    }

    Item *cur = list->head;
    int index = 0;

    do {
        if (strcmp(cur->data.first_name, first) == 0 &&
            (!need_last_name || strcmp(cur->data.last_name, last) == 0)) {
            if (match_count >= max_matches) {
                break;
            }
            matches[match_count++] = index;
        }
        cur = cur->next;
        index++;
    } while (cur != list->head);

    return match_count;
}

void add_contact(ContactList *list) {
    if (list->size >= MAX_CONTACTS) {
        printf("Contact list full\n");
        return;
    }

    Contact contact;
    read_nonempty("First name", contact.first_name);
    read_nonempty("Last name", contact.last_name);
    read_optional("Workplace", contact.workplace);
    read_optional("Position", contact.position);
    read_items("Phone", contact.phones, &contact.phone_count, MAX_PHONES);
    read_items("Email", contact.emails, &contact.email_count, MAX_EMAILS);
    read_items("Social link", contact.social_links, &contact.social_count, MAX_SOCIALS);
    read_items("Messenger profile", contact.messengers, &contact.messenger_count, MAX_MESSENGERS);

    int inserted = 0;
    Item *new_head = insert_sorted(list->head, &contact, &inserted);
    if (!inserted) {
        printf("Memory allocation error\n");
        return;
    }

    list->head = new_head;
    if (list->head != NULL) {
        list->tail = list->head->prev;
        list->size++;
    }
    printf("Contact added\n");
}

void print_contacts(const ContactList *list) {
    if (list->head == NULL) {
        printf("No contacts\n");
        return;
    }

    Item *cur = list->head;
    int idx = 1;

    do {
        Contact *contact = &cur->data;

        printf("\n--- Contact %d ---\n", idx++);
        printf("Name: %s %s\n", contact->first_name, contact->last_name);
        if (strlen(contact->workplace) > 0) {
            printf("Workplace: %s\n", contact->workplace);
        }
        if (strlen(contact->position) > 0) {
            printf("Position: %s\n", contact->position);
        }
        print_items("Phone", contact->phones, contact->phone_count);
        print_items("Email", contact->emails, contact->email_count);
        print_items("Social link", contact->social_links, contact->social_count);
        print_items("Messenger profile", contact->messengers, contact->messenger_count);

        cur = cur->next;
    } while (cur != list->head);
}

void edit_contact(ContactList *list, const char *first, const char *last) {
    if (list->head == NULL) {
        printf("No contacts\n");
        return;
    }

    Item *cur = list->head;
    int need_last_name = last != NULL && strlen(last) > 0;

    do {
        if (strcmp(cur->data.first_name, first) == 0 &&
            (!need_last_name || strcmp(cur->data.last_name, last) == 0)) {
            Contact *contact = &cur->data;
            Contact updated;
            char buf[MAX_STR];

            printf("New first name (Enter to keep '%s'): ", contact->first_name);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) {
                strcpy(contact->first_name, buf);
            }

            printf("New last name (Enter to keep '%s'): ", contact->last_name);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) {
                strcpy(contact->last_name, buf);
            }

            printf("New workplace (Enter to keep '%s'): ", contact->workplace);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) {
                strcpy(contact->workplace, buf);
            }

            printf("New position (Enter to keep '%s'): ", contact->position);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) {
                strcpy(contact->position, buf);
            }

            printf("Update phone numbers? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y') {
                read_items("Phone", contact->phones, &contact->phone_count, MAX_PHONES);
            }

            printf("Update emails? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y') {
                read_items("Email", contact->emails, &contact->email_count, MAX_EMAILS);
            }

            printf("Update social links? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y') {
                read_items("Social link", contact->social_links, &contact->social_count, MAX_SOCIALS);
            }

            printf("Update messenger profiles? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y') {
                read_items("Messenger profile", contact->messengers, &contact->messenger_count, MAX_MESSENGERS);
            }

            updated = *contact;

            if (cur->next == cur) {
                free(cur);
                list->head = NULL;
                list->tail = NULL;
                list->size = 0;
            } else {
                cur->next->prev = cur->prev;
                cur->prev->next = cur->next;
                if (cur == list->head) {
                    list->head = cur->next;
                }
                if (cur == list->tail) {
                    list->tail = cur->prev;
                }
                free(cur);
                list->size--;
            }

            {
                int inserted = 0;
                list->head = insert_sorted(list->head, &updated, &inserted);
                if (!inserted) {
                    printf("Memory allocation error\n");
                    return;
                }
            }
            if (list->head != NULL) {
                list->tail = list->head->prev;
                list->size++;
            }
            printf("Contact updated\n");
            return;
        }
        cur = cur->next;
    } while (cur != list->head);

    printf("Contact not found\n");
}

void delete_contact(ContactList *list, const char *first, const char *last) {
    if (list->head == NULL) {
        printf("No contacts\n");
        return;
    }

    Item *cur = list->head;
    int need_last_name = last != NULL && strlen(last) > 0;

    do {
        if (strcmp(cur->data.first_name, first) == 0 &&
            (!need_last_name || strcmp(cur->data.last_name, last) == 0)) {
            if (cur->next == cur) {
                free(cur);
                list->head = NULL;
                list->tail = NULL;
                list->size = 0;
                printf("Contact deleted\n");
                return;
            }

            cur->next->prev = cur->prev;
            cur->prev->next = cur->next;
            if (cur == list->head) {
                list->head = cur->next;
            }
            if (cur == list->tail) {
                list->tail = cur->prev;
            }
            free(cur);
            list->size--;
            printf("Contact deleted\n");
            return;
        }
        cur = cur->next;
    } while (cur != list->head);

    printf("Contact not found\n");
}
