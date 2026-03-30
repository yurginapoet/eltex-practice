#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

typedef struct {
    int id;
    char text[32];
} Message;

Message *make_message(int id, const char *text) {
    Message *m = (Message *)malloc(sizeof(Message));
    if (!m) return NULL;
    m->id = id;
    strncpy(m->text, text, sizeof(m->text) - 1);
    m->text[sizeof(m->text) - 1] = '\0';
    return m;
}

void print_and_free(Message *m, uint8_t priority, const char *label) {
    if (!m) {
        printf("%s: no message\n", label);
        return;
    }
    printf("%s: id=%d, priority=%u, text=%s\n", label, m->id, priority, m->text);
    free(m);
}

int main(void) {
    PriorityQueue q;
    pq_init(&q);

    srand(1); // чтобы результат генерации был воспроизводимым.
    for (int i = 1; i <= 100; ++i) {
        uint8_t p = (uint8_t)(rand() % 256);
        char buf[32];
        snprintf(buf, sizeof(buf), "msg_%d", i);
        Message *m = make_message(i, buf);
        if (!m) {
            printf("Error: failed to allocate memory\n");
            pq_clear(&q);
            return 1;
        }
        pq_push(&q, m, p);
        printf("Added: id=%d, priority=%u, text=%s\n", m->id, p, m->text);
    }

    printf("\n");

    for (int i = 0; i < 3; ++i) {
        uint8_t p = 0;
        Message *m = (Message *)pq_pop_first(&q, &p);
        print_and_free(m, p, "pop_first");
    }

    printf("\n");

    uint8_t exact_p = 175;
    Message *m_exact = (Message *)pq_pop_priority(&q, exact_p);
    print_and_free(m_exact, exact_p, "pop_priority(175)");

    printf("\n");

    exact_p = 100;
    m_exact = (Message *)pq_pop_priority(&q, exact_p);
    print_and_free(m_exact, exact_p, "pop_priority(100)");

    printf("\n");

    uint8_t min_p = 200;
    uint8_t real_p = 0;
    Message *m_min = (Message *)pq_pop_not_lower(&q, min_p, &real_p);
    print_and_free(m_min, real_p, "pop_not_lower(200)");

    printf("\n");

    while (1) {
        uint8_t p = 0;
        Message *m = (Message *)pq_pop_first(&q, &p);
        if (!m) break;
        free(m);
    }

    pq_clear(&q);
    return 0;
}
