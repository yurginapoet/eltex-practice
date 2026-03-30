#include "queue.h"
#include <stdlib.h>

void pq_init(PriorityQueue *q) {
    if (!q) return;
    for (int i = 0; i < 256; ++i) {
        q->head[i] = NULL;
        q->tail[i] = NULL;
    }
    q->size = 0;
}

void pq_clear(PriorityQueue *q) {
    if (!q) return;
    for (int i = 0; i < 256; ++i) {
        QueueNode *cur = q->head[i];
        while (cur) {
            QueueNode *next = cur->next;
            free(cur);
            cur = next;
        }
        q->head[i] = NULL;
        q->tail[i] = NULL;
    }
    q->size = 0;
}

int pq_push(PriorityQueue *q, void *data, uint8_t priority) {
    if (!q) return -1;

    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (!node) return -1;

    node->data = data;
    node->priority = priority;
    node->next = NULL;

    if (q->tail[priority]) {
        q->tail[priority]->next = node;
    } else {
        q->head[priority] = node;
    }
    q->tail[priority] = node;
    q->size++;
    return 0;
}

static void *pop_from_list(PriorityQueue *q, uint8_t priority) {
    QueueNode *node = q->head[priority];
    if (!node) return NULL;

    q->head[priority] = node->next;
    if (!q->head[priority]) {
        q->tail[priority] = NULL;
    }

    void *data = node->data;
    free(node);
    q->size--;
    return data;
}

void *pq_pop_first(PriorityQueue *q, uint8_t *out_priority) {
    if (!q || q->size == 0) return NULL;

    for (int p = 255; p >= 0; --p) {
        if (q->head[p]) {
            if (out_priority) *out_priority = (uint8_t)p;
            return pop_from_list(q, (uint8_t)p);
        }
    }
    return NULL;
}

void *pq_pop_priority(PriorityQueue *q, uint8_t priority) {
    if (!q) return NULL;
    return pop_from_list(q, priority);
}

void *pq_pop_not_lower(PriorityQueue *q, uint8_t min_priority, uint8_t *out_priority) {
    if (!q || q->size == 0) return NULL;

    for (int p = 255; p >= (int)min_priority; --p) {
        if (q->head[p]) {
            if (out_priority) *out_priority = (uint8_t)p;
            return pop_from_list(q, (uint8_t)p);
        }
    }
    return NULL;
}
