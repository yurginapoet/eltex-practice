#ifndef CONTACTS_H
#define CONTACTS_H

#define MAX_CONTACTS 100
#define MAX_STR 128
#define MAX_PHONES 5
#define MAX_EMAILS 5
#define MAX_SOCIALS 5
#define MAX_MESSENGERS 5
#define BALANCE_PERIOD 5

typedef struct {
    int id;
    char first_name[MAX_STR];
    char last_name[MAX_STR];
    char workplace[MAX_STR];
    char position[MAX_STR];
    char phones[MAX_PHONES][MAX_STR];
    int phone_count;
    char emails[MAX_EMAILS][MAX_STR];
    int email_count;
    char social_links[MAX_SOCIALS][MAX_STR];
    int social_count;
    char messengers[MAX_MESSENGERS][MAX_STR];
    int messenger_count;
} Contact;

typedef struct btree {
    Contact data;
    struct btree *left;
    struct btree *right;
} btree;

typedef struct {
    btree *root;
    int size;
    int next_id;
    int change_count;
} ContactList;

void init_list(ContactList *list);
void free_list(ContactList *list);

void add_contact(ContactList *list);
void edit_contact(ContactList *list, const char *first, const char *last);
void delete_contact(ContactList *list, const char *first, const char *last);
void print_contacts(const ContactList *list);

int is_valid_email(const char *email);
int is_valid_phone(const char *phone);

#endif
