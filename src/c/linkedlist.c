/* linkedlist.c — doubly linked list for the activity log. Owner: M2. */
#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>

LinkedList *list_create(void) {
    LinkedList *l = (LinkedList *)malloc(sizeof(LinkedList));
    if (!l) return NULL;
    l->head = l->tail = NULL;
    l->size = 0;
    return l;
}

ListNode *list_append(LinkedList *l, const Event *e) {
    if (!l || !e) return NULL;
    ListNode *n = (ListNode *)malloc(sizeof(ListNode));
    if (!n) return NULL;
    n->data = *e;                   /* copy by value — list owns its node */
    n->next = NULL;
    n->prev = l->tail;
    if (l->tail) l->tail->next = n; else l->head = n;
    l->tail = n;
    l->size++;
    return n;
}

ListNode *list_find(const LinkedList *l, const char *emp_id) {
    if (!l || !emp_id) return NULL;
    for (ListNode *n = l->head; n; n = n->next)
        if (strcmp(n->data.emp_id, emp_id) == 0) return n;
    return NULL;
}

int list_remove(LinkedList *l, ListNode *node) {
    if (!l || !node) return 0;
    if (node->prev) node->prev->next = node->next; else l->head = node->next;
    if (node->next) node->next->prev = node->prev; else l->tail = node->prev;
    free(node);
    l->size--;
    return 1;
}

void list_traverse(const LinkedList *l, list_visit_fn fn, void *ctx) {
    if (!l || !fn) return;
    for (ListNode *n = l->head; n; n = n->next) fn(&n->data, ctx);
}

size_t list_size(const LinkedList *l) { return l ? l->size : 0; }

void list_free(LinkedList *l) {
    if (!l) return;
    ListNode *n = l->head;
    while (n) { ListNode *next = n->next; free(n); n = next; }
    free(l);
}
