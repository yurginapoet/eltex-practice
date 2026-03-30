#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contacts.h"

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

    while (*endptr == ' ' || *endptr == '\t') 
        endptr++;
    if (*text == '\0' || *endptr != '\0') 
        return 0;

    *out = (int)value;
    return 1;
}

int is_valid_email(const char *email) {
    const char *at;
    const char *dot_after_at;

    if (email == NULL || *email == '\0') return 0;

    at = strchr(email, '@');
    if (at == NULL || at == email)  return 0;
    if (strchr(at + 1, '@') != NULL || *(at + 1) == '\0') return 0;

    dot_after_at = strchr(at + 1, '.');
    if (dot_after_at == NULL || dot_after_at == at + 1 || *(dot_after_at + 1) == '\0') return 0;

    return 1;
}

int is_valid_phone(const char *phone) {
    int digit_count = 0;

    if (phone == NULL || *phone == '\0') return 0;

    for (int i = 0; phone[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)phone[i];

        if (isdigit(ch)) {
            digit_count++;
            continue;
        }
        if (ch == '+' || ch == '-' || ch == '(' || ch == ')') continue;

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

        if (parse_int(buf, &count) && count >= 0 && count <= max_count) return count;

        printf("Invalid number\n");
    }
}

static void read_nonempty(const char *label, char *dst) {
    while (1) {
        printf("%s: ", label);
        input_line(dst, MAX_STR);

        if (strlen(dst) > 0) return;

        printf("Field is required\n");
    }
}

static void read_optional(const char *label, char *dst) {
    printf("%s: ", label);
    input_line(dst, MAX_STR);
}

static int validate_item(const char *label, const char *value) {
    if (strcmp(label, "Phone") == 0) return is_valid_phone(value);
    if (strcmp(label, "Email") == 0) return is_valid_email(value);
    return 1;
}

static const char *validation_error(const char *label) {
    if (strcmp(label, "Phone") == 0) return "Phone must contain only digits, parentheses, '-' and '+'";
    if (strcmp(label, "Email") == 0) return "Email must contain '@' and a domain part";
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
    for (int i = 0; i < count; i++) 
        printf("%s #%d: %s\n", label, i + 1, items[i]);
}

int find_contacts_by_name(const ContactList *list, const char *first, const char *last,
                          int matches[], int max_matches) {
    int need_last_name = last != NULL && strlen(last) > 0;
    int match_count = 0;

    if (list == NULL || first == NULL || matches == NULL || max_matches <= 0) return 0;

    if (list->head == NULL) return 0;

    Item *tmp = list->head;
    int index = 0;

    do {
        if (strcmp(tmp->data.first_name, first) == 0 &&
            (!need_last_name || strcmp(tmp->data.last_name, last) == 0)) {
            if (match_count >= max_matches) break;
            matches[match_count] = index;
            match_count++;
        }
        tmp = tmp->next;
        index++;
    } while (tmp != list->head);

    return match_count;
}

static int contact_cmp(const Contact *c1, const Contact *c2) {
    int last = strcmp(c1->last_name, c2->last_name);
    if (last != 0) return last;
    return strcmp(c1->first_name, c2->first_name);
}

void init_list(ContactList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

struct Item* InsertSort(const Contact *c, Item* head){
    Item* newItem = (Item*)malloc(sizeof(Item));
    newItem->data = *c;

    Item* tmp = head;

    if (head == NULL) {
        newItem->next = newItem;
        newItem->prev = newItem;
        return newItem;
    }

    do {
        if (contact_cmp(&newItem->data, &tmp->data) <= 0) {

            newItem->next = tmp;
            newItem->prev = tmp->prev;

            tmp->prev->next = newItem; 
            tmp->prev = newItem;

            if (tmp == head) return newItem; 

            return head;
        }
        tmp = tmp->next;
    } while (tmp != head);

    newItem->next = head;
    newItem->prev = head->prev;

    head->prev->next = newItem;
    head->prev = newItem;

    return head;
}


void add_contact(ContactList *list) {
    if (list->size >= MAX_CONTACTS) {
        printf("Contact list full\n");
        return;
    }

    Contact c;

    read_nonempty("First name", c.first_name);
    read_nonempty("Last name", c.last_name);
    read_optional("Workplace", c.workplace);
    read_optional("Position", c.position);
    read_items("Phone", c.phones, &c.phone_count, MAX_PHONES);
    read_items("Email", c.emails, &c.email_count, MAX_EMAILS);
    read_items("Social link", c.social_links, &c.social_count, MAX_SOCIALS);
    read_items("Messenger profile", c.messengers, &c.messenger_count, MAX_MESSENGERS);

    list->head = InsertSort(&c, list->head);
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

    Item *tmp = list->head;
    int i = 1;

    do {
        Contact *c = &tmp->data;

        printf("\n--- Contact %d ---\n", i++);
        printf("Name: %s %s\n", c->first_name, c->last_name);

        if (strlen(c->workplace) > 0) printf("Workplace: %s\n", c->workplace);
        if (strlen(c->position) > 0) printf("Position: %s\n", c->position);

        print_items("Phone", c->phones, c->phone_count);
        print_items("Email", c->emails, c->email_count);
        print_items("Social link", c->social_links, c->social_count);
        print_items("Messenger profile", c->messengers, c->messenger_count);

        tmp = tmp->next;
    } while (tmp != list->head);
}

void edit_contact(ContactList *list, const char *first, const char *last) {
    if (list->head == NULL) {
        printf("No contacts\n");
        return;
    }

    Item *tmp = list->head;

    do {
        if (strcmp(tmp->data.first_name, first) == 0 &&
            strcmp(tmp->data.last_name, last) == 0) {

            Contact *c = &tmp->data;
            char buf[MAX_STR];

            printf("New first name (Enter to keep '%s'): ", c->first_name);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) strcpy(c->first_name, buf);

            printf("New last name (Enter to keep '%s'): ", c->last_name);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) strcpy(c->last_name, buf);

            printf("New workplace (Enter to keep '%s'): ", c->workplace);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) strcpy(c->workplace, buf);

            printf("New position (Enter to keep '%s'): ", c->position);
            input_line(buf, sizeof(buf));
            if (strlen(buf) > 0) strcpy(c->position, buf);

            printf("Update phone numbers? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y')
                read_items("Phone", c->phones, &c->phone_count, MAX_PHONES);

            printf("Update emails? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y')
                read_items("Email", c->emails, &c->email_count, MAX_EMAILS);

            printf("Update social links? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y')
                read_items("Social link", c->social_links, &c->social_count, MAX_SOCIALS);

            printf("Update messenger profiles? (y/n): ");
            input_line(buf, sizeof(buf));
            if (buf[0] == 'y' || buf[0] == 'Y')
                read_items("Messenger profile", c->messengers, &c->messenger_count, MAX_MESSENGERS);

            Contact updated = *c;

            if (tmp->next == tmp) {
                free(tmp);
                list->head = NULL;
            } else {
                tmp->next->prev = tmp->prev;
                tmp->prev->next = tmp->next;

                if (tmp == list->head)
                    list->head = tmp->next;

                free(tmp);
            }

            list->head = InsertSort(&updated, list->head);
            if (list->head != NULL) list->tail = list->head->prev;
            printf("Contact updated\n");
            return;
        }

        tmp = tmp->next;
    } while (tmp != list->head);

    printf("Contact not found\n");
}

void delete_contact(ContactList *list, const char *first, const char *last) {
    if (list->head == NULL) return;

    Item *tmp = list->head;

    do {
        if (strcmp(tmp->data.first_name, first) == 0 &&
            strcmp(tmp->data.last_name, last) == 0) {

            if (tmp->next == tmp) {
                free(tmp);
                list->head = NULL;
                list->tail = NULL;
                list->size = 0;
                printf("Contact deleted\n");
                return;
            }

            tmp->next->prev = tmp->prev;
            tmp->prev->next = tmp->next;

            if (tmp == list->head)
                list->head = tmp->next;
            if (tmp == list->tail)
                list->tail = tmp->prev;

            free(tmp);
            list->size--;
            printf("Contact deleted\n");
            return;
        }

        tmp = tmp->next;
    } while (tmp != list->head);

    printf("Contact not found\n");
}
