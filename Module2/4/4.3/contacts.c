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

    if (email == NULL || *email == '\0')
        return 0;

    at = strchr(email, '@');
    if (at == NULL || at == email)
        return 0;
    if (strchr(at + 1, '@') != NULL || *(at + 1) == '\0')
        return 0;

    dot_after_at = strchr(at + 1, '.');
    if (dot_after_at == NULL || dot_after_at == at + 1 || *(dot_after_at + 1) == '\0')
        return 0;

    return 1;
}

int is_valid_phone(const char *phone) {
    int digit_count = 0;

    if (phone == NULL || *phone == '\0')
        return 0;

    for (int i = 0; phone[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)phone[i];

        if (isdigit(ch)) {
            digit_count++;
            continue;
        }
        if (ch == '+' || ch == '-' || ch == '(' || ch == ')')
            continue;

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

        if (parse_int(buf, &count) && count >= 0 && count <= max_count)
            return count;

        printf("Invalid number\n");
    }
}

static void read_nonempty(const char *label, char *dst) {
    while (1) {
        printf("%s: ", label);
        input_line(dst, MAX_STR);

        if (strlen(dst) > 0)
            return;

        printf("Field is required\n");
    }
}

static void read_optional(const char *label, char *dst) {
    printf("%s: ", label);
    input_line(dst, MAX_STR);
}

static int validate_item(const char *label, const char *value) {
    if (strcmp(label, "Phone") == 0)
        return is_valid_phone(value);
    if (strcmp(label, "Email") == 0)
        return is_valid_email(value);
    return 1;
}

static const char *validation_error(const char *label) {
    if (strcmp(label, "Phone") == 0)
        return "Phone must contain only digits, parentheses, '-' and '+'";
    if (strcmp(label, "Email") == 0)
        return "Email must contain '@' and a domain part";
    return "Invalid value";
}

static void read_items(const char *label, char items[][MAX_STR], int *count, int max_count) {
    *count = read_count(label, max_count);

    for (int i = 0; i < *count; i++) {
        printf("%s #%d: ", label, i + 1);
        input_line(items[i], MAX_STR);

        if (strlen(items[i]) == 0) {
            printf("Field is required\n");
            i--;
            continue;
        }

        if (!validate_item(label, items[i])) {
            printf("%s\n", validation_error(label));
            i--;
        }
    }
}

static void print_items(const char *label, const char items[][MAX_STR], int count) {
    for (int i = 0; i < count; i++)
        printf("%s #%d: %s\n", label, i + 1, items[i]);
}

static int contact_cmp(const Contact *a, const Contact *b) {
    int x = strcmp(a->last_name, b->last_name);
    if (x != 0)
        return x;

    x = strcmp(a->first_name, b->first_name);
    if (x != 0)
        return x;

    if (a->id < b->id)
        return -1;
    if (a->id > b->id)
        return 1;

    return 0;
}

static int Ins_Btree(Contact val, btree **q) {
    if (*q == NULL) {
        *q = (btree *)malloc(sizeof(btree));
        if (*q == NULL)
            return 0;

        (*q)->left = NULL;
        (*q)->right = NULL;
        (*q)->data = val;
        return 1;
    }

    if (contact_cmp(&val, &(*q)->data) < 0)
        return Ins_Btree(val, &(*q)->left);

    return Ins_Btree(val, &(*q)->right);
}

static int Delete_Btree(Contact key, btree **node) {
    btree *t;
    btree *up;
    int cmp;

    if (*node == NULL)
        return 0;

    cmp = contact_cmp(&key, &(*node)->data);

    if (cmp < 0)
        return Delete_Btree(key, &(*node)->left);
    if (cmp > 0)
        return Delete_Btree(key, &(*node)->right);

    if ((*node)->left == NULL && (*node)->right == NULL) {
        free(*node);
        *node = NULL;
        return 1;
    }

    if ((*node)->left == NULL) {
        t = *node;
        *node = (*node)->right;
        free(t);
        return 1;
    }

    if ((*node)->right == NULL) {
        t = *node;
        *node = (*node)->left;
        free(t);
        return 1;
    }

    up = *node;
    t = (*node)->left;

    while (t->right != NULL) {
        up = t;
        t = t->right;
    }

    (*node)->data = t->data;

    if (up != (*node)) {
        if (t->left != NULL)
            up->right = t->left;
        else
            up->right = NULL;
    } else {
        (*node)->left = t->left;
    }

    free(t);
    return 1;
}

static void Free_Btree(btree *p) {
    if (p == NULL)
        return;

    Free_Btree(p->left);
    Free_Btree(p->right);
    free(p);
}

static void Save_Inorder(btree *p, Contact arr[], int *n) {
    if (p == NULL)
        return;

    Save_Inorder(p->left, arr, n);
    arr[*n] = p->data;
    (*n)++;
    Save_Inorder(p->right, arr, n);
}

static btree *Build_Balanced(Contact arr[], int left, int right) {
    int mid;
    btree *node;

    if (left > right)
        return NULL;

    mid = (left + right) / 2;

    node = (btree *)malloc(sizeof(btree));
    if (node == NULL)
        return NULL;

    node->data = arr[mid];
    node->left = Build_Balanced(arr, left, mid - 1);
    node->right = Build_Balanced(arr, mid + 1, right);
    return node;
}

static void rebalance_tree(ContactList *list) {
    Contact arr[MAX_CONTACTS];
    int n = 0;

    if (list->root == NULL || list->size < 2)
        return;

    Save_Inorder(list->root, arr, &n);
    Free_Btree(list->root);
    list->root = Build_Balanced(arr, 0, n - 1);
}

static void maybe_balance(ContactList *list) {
    list->change_count++;

    if (list->change_count % BALANCE_PERIOD == 0) {
        rebalance_tree(list);
        printf("Tree balanced\n");
    }
}

static void print_contact_summary(const Contact *c, int list_number) {
    printf("%d. %s %s", list_number, c->first_name, c->last_name);

    if (c->phone_count > 0)
        printf(", phone: %s", c->phones[0]);
    if (strlen(c->workplace) > 0)
        printf(", workplace: %s", c->workplace);

    printf("\n");
}

static int read_selection(int max_value) {
    char buf[MAX_STR];
    int choice;

    while (1) {
        printf("Choose contact number (1-%d): ", max_value);
        input_line(buf, sizeof(buf));

        if (parse_int(buf, &choice) && choice >= 1 && choice <= max_value)
            return choice;

        printf("Invalid number\n");
    }
}

static void collect_matches(btree *p, const char *first, const char *last,
                            btree *matches[], int *count) {
    if (p == NULL || *count >= MAX_CONTACTS)
        return;

    collect_matches(p->left, first, last, matches, count);

    if (*count < MAX_CONTACTS && strcmp(p->data.first_name, first) == 0) {
        if (last[0] == '\0' || strcmp(p->data.last_name, last) == 0) {
            matches[*count] = p;
            (*count)++;
        }
    }

    collect_matches(p->right, first, last, matches, count);
}

static btree *resolve_contact_node(ContactList *list, const char *first, const char *last,
                                   const char *action_name) {
    btree *matches[MAX_CONTACTS];
    int count = 0;

    if (list->root == NULL) {
        printf("No contacts\n");
        return NULL;
    }

    collect_matches(list->root, first, last, matches, &count);

    if (count == 0) {
        printf("Contact not found\n");
        return NULL;
    }

    if (count == 1)
        return matches[0];

    if (last[0] == '\0')
        printf("Several contacts found with first name %s. Choose one to %s:\n", first, action_name);
    else
        printf("Several contacts found for %s %s. Choose one to %s:\n", first, last, action_name);

    for (int i = 0; i < count; i++)
        print_contact_summary(&matches[i]->data, i + 1);

    return matches[read_selection(count) - 1];
}

static void print_contact_full(const Contact *c, int number) {
    printf("\n--- Contact %d ---\n", number);
    printf("Name: %s %s\n", c->first_name, c->last_name);

    if (strlen(c->workplace) > 0)
        printf("Workplace: %s\n", c->workplace);
    if (strlen(c->position) > 0)
        printf("Position: %s\n", c->position);

    print_items("Phone", c->phones, c->phone_count);
    print_items("Email", c->emails, c->email_count);
    print_items("Social link", c->social_links, c->social_count);
    print_items("Messenger profile", c->messengers, c->messenger_count);
}

static void Print_Btree(btree *p, int *counter) {
    if (p == NULL)
        return;

    Print_Btree(p->left, counter);
    print_contact_full(&p->data, *counter);
    (*counter)++;
    Print_Btree(p->right, counter);
}

void init_list(ContactList *list) {
    list->root = NULL;
    list->size = 0;
    list->next_id = 1;
    list->change_count = 0;
}

void free_list(ContactList *list) {
    Free_Btree(list->root);
    list->root = NULL;
    list->size = 0;
}

void add_contact(ContactList *list) {
    Contact c;

    if (list->size >= MAX_CONTACTS) {
        printf("Contact list is full\n");
        return;
    }

    read_nonempty("First name", c.first_name);
    read_nonempty("Last name", c.last_name);
    read_optional("Workplace", c.workplace);
    read_optional("Position", c.position);
    read_items("Phone", c.phones, &c.phone_count, MAX_PHONES);
    read_items("Email", c.emails, &c.email_count, MAX_EMAILS);
    read_items("Social link", c.social_links, &c.social_count, MAX_SOCIALS);
    read_items("Messenger profile", c.messengers, &c.messenger_count, MAX_MESSENGERS);

    c.id = list->next_id;
    list->next_id++;

    if (!Ins_Btree(c, &list->root)) {
        printf("Memory allocation error\n");
        return;
    }

    list->size++;
    maybe_balance(list);
    printf("Contact added\n");
}

void print_contacts(const ContactList *list) {
    int counter = 1;

    if (list->root == NULL) {
        printf("No contacts\n");
        return;
    }

    Print_Btree(list->root, &counter);
}

void edit_contact(ContactList *list, const char *first, const char *last) {
    btree *node = resolve_contact_node(list, first, last, "edit");
    Contact old_data;
    Contact new_data;
    char buf[MAX_STR];

    if (node == NULL)
        return;

    old_data = node->data;
    new_data = node->data;

    printf("New first name (Enter to keep '%s'): ", new_data.first_name);
    input_line(buf, sizeof(buf));
    if (strlen(buf) > 0)
        strcpy(new_data.first_name, buf);

    printf("New last name (Enter to keep '%s'): ", new_data.last_name);
    input_line(buf, sizeof(buf));
    if (strlen(buf) > 0)
        strcpy(new_data.last_name, buf);

    printf("New workplace (Enter to keep '%s'): ", new_data.workplace);
    input_line(buf, sizeof(buf));
    if (strlen(buf) > 0)
        strcpy(new_data.workplace, buf);

    printf("New position (Enter to keep '%s'): ", new_data.position);
    input_line(buf, sizeof(buf));
    if (strlen(buf) > 0)
        strcpy(new_data.position, buf);

    printf("Update phone numbers? (y/n): ");
    input_line(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
        read_items("Phone", new_data.phones, &new_data.phone_count, MAX_PHONES);

    printf("Update emails? (y/n): ");
    input_line(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
        read_items("Email", new_data.emails, &new_data.email_count, MAX_EMAILS);

    printf("Update social links? (y/n): ");
    input_line(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
        read_items("Social link", new_data.social_links, &new_data.social_count, MAX_SOCIALS);

    printf("Update messenger profiles? (y/n): ");
    input_line(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
        read_items("Messenger profile", new_data.messengers, &new_data.messenger_count, MAX_MESSENGERS);

    if (strcmp(old_data.first_name, new_data.first_name) != 0 ||
        strcmp(old_data.last_name, new_data.last_name) != 0) {

        if (!Delete_Btree(old_data, &list->root)) {
            printf("Contact not found\n");
            return;
        }

        if (!Ins_Btree(new_data, &list->root)) {
            Ins_Btree(old_data, &list->root);
            printf("Memory allocation error\n");
            return;
        }
    } else {
        node->data = new_data;
    }

    maybe_balance(list);
    printf("Contact updated\n");
}

void delete_contact(ContactList *list, const char *first, const char *last) {
    btree *node = resolve_contact_node(list, first, last, "delete");
    Contact key;

    if (node == NULL)
        return;

    key = node->data;

    if (Delete_Btree(key, &list->root)) {
        list->size--;
        maybe_balance(list);
        printf("Contact deleted\n");
    } else {
        printf("Contact not found\n");
    }
}
