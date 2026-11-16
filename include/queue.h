/* queue.h — FIFO queue of Alert (analyst review line). Owner: M1/M2. */
#ifndef QUEUE_H
#define QUEUE_H
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QueueNode {
    Alert             data;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    size_t     size;
} Queue;

Queue       *queue_create(void);
int          enqueue(Queue *q, const Alert *a);         /* add at rear, 1 ok */
int          enqueue_front(Queue *q, const Alert *a);   /* used by undo: put back at front */
int          dequeue(Queue *q, Alert *out);             /* 1 ok, 0 if empty */
const Alert *queue_peek(const Queue *q);
int          queue_is_empty(const Queue *q);
size_t       queue_size(const Queue *q);
size_t       queue_to_array(const Queue *q, Alert *out, size_t max); /* copy for display/sort */
void         queue_free(Queue *q);

#ifdef __cplusplus
}
#endif
#endif
