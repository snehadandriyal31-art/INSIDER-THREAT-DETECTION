/* test_queue.c — FIFO order, dequeue from empty, enqueue_front (undo), snapshot. Owner: M4. */
#include "queue.h"
#include "test_util.h"

static Alert al(const char *id, double score) {
    Alert a; memset(&a, 0, sizeof a);
    strncpy(a.emp_id, id, ID_LEN - 1); a.score = score; return a;
}

int main(void) {
    Queue *q = queue_create();
    Alert out, buf[8];
    CHECK(q && queue_is_empty(q) && queue_size(q) == 0);
    CHECK(!dequeue(q, &out));                             /* edge: dequeue from empty */
    CHECK(queue_peek(q) == NULL);
    CHECK(queue_to_array(q, buf, 8) == 0);

    Alert a1 = al("E101", 70), a2 = al("E102", 80), a3 = al("E103", 90);
    CHECK(enqueue(q, &a1) && enqueue(q, &a2) && enqueue(q, &a3));
    CHECK(queue_size(q) == 3);
    CHECK(strcmp(queue_peek(q)->emp_id, "E101") == 0);    /* front = first in */

    CHECK(queue_to_array(q, buf, 8) == 3 && strcmp(buf[2].emp_id, "E103") == 0);
    CHECK(queue_to_array(q, buf, 2) == 2);                /* respects max */

    CHECK(dequeue(q, &out) && strcmp(out.emp_id, "E101") == 0);   /* FIFO */
    CHECK(enqueue_front(q, &out));                        /* undo puts it back at front */
    CHECK(strcmp(queue_peek(q)->emp_id, "E101") == 0 && queue_size(q) == 3);

    CHECK(dequeue(q, &out) && dequeue(q, &out) && dequeue(q, &out));
    CHECK(strcmp(out.emp_id, "E103") == 0);
    CHECK(queue_is_empty(q) && q->rear == NULL);          /* rear reset when emptied */

    CHECK(enqueue_front(q, &a2));                         /* enqueue_front on empty queue */
    CHECK(q->front == q->rear && queue_size(q) == 1);
    CHECK(!enqueue(q, NULL));

    queue_free(q);
    queue_free(NULL);
    REPORT("test_queue");
}
