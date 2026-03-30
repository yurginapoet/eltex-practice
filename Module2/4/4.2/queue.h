#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stddef.h>

typedef struct QueueNode {
    void *data;
    uint8_t priority;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *head[256];
    QueueNode *tail[256];
    size_t size;
} PriorityQueue;

void pq_init(PriorityQueue *q);
void pq_clear(PriorityQueue *q);
int pq_push(PriorityQueue *q, void *data, uint8_t priority);
void *pq_pop_first(PriorityQueue *q, uint8_t *out_priority);
void *pq_pop_priority(PriorityQueue *q, uint8_t priority);
void *pq_pop_not_lower(PriorityQueue *q, uint8_t min_priority, uint8_t *out_priority);

#endif 
