/* queue.c — linked FIFO queue for the alert review line. Owner: M2. */
#include "queue.h"
#include <stdlib.h>

Queue *queue_create(void) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) return NULL;
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

static QueueNode *make_node(const Alert *a) {
    QueueNode *n = (QueueNode *)malloc(sizeof(QueueNode));
    if (!n) return NULL;
    n->data = *a;
    n->next = NULL;
    return n;
}

int enqueue(Queue *q, const Alert *a) {
    if (!q || !a) return 0;
    QueueNode *n = make_node(a);
    if (!n) return 0;
    if (q->rear) q->rear->next = n; else q->front = n;
    q->rear = n;
    q->size++;
    return 1;
}

int enqueue_front(Queue *q, const Alert *a) {
    if (!q || !a) return 0;
    QueueNode *n = make_node(a);
    if (!n) return 0;
    n->next = q->front;
    q->front = n;
    if (!q->rear) q->rear = n;
    q->size++;
    return 1;
}

int dequeue(Queue *q, Alert *out) {
    if (!q || !q->front) return 0;
    QueueNode *n = q->front;
    if (out) *out = n->data;
    q->front = n->next;
    if (!q->front) q->rear = NULL;
    free(n);
    q->size--;
    return 1;
}

const Alert *queue_peek(const Queue *q) { return (q && q->front) ? &q->front->data : NULL; }
int    queue_is_empty(const Queue *q)   { return !q || q->front == NULL; }
size_t queue_size(const Queue *q)       { return q ? q->size : 0; }

size_t queue_to_array(const Queue *q, Alert *out, size_t max) {
    size_t i = 0;
    if (!q || !out) return 0;
    for (QueueNode *n = q->front; n && i < max; n = n->next) out[i++] = n->data;
    return i;
}

void queue_free(Queue *q) {
    if (!q) return;
    while (q->front) { QueueNode *n = q->front; q->front = n->next; free(n); }
    free(q);
}
