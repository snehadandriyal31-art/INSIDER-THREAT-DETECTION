/* linkedlist.h — doubly linked list holding the activity log. Owner: M1/M2. */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ListNode {
    Event            data;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t    size;
} LinkedList;

typedef void (*list_visit_fn)(const Event *e, void *ctx);

LinkedList *list_create(void);
ListNode   *list_append(LinkedList *l, const Event *e);           /* O(1) via tail */
ListNode   *list_find(const LinkedList *l, const char *emp_id);   /* first event of employee */
int         list_remove(LinkedList *l, ListNode *node);           /* O(1) unlink */
void        list_traverse(const LinkedList *l, list_visit_fn fn, void *ctx);
size_t      list_size(const LinkedList *l);
void        list_free(LinkedList *l);

#ifdef __cplusplus
}
#endif
#endif
