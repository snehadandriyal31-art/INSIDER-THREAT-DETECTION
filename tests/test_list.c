/* test_list.c — linked list: append/find/remove/traverse. Owner: M4. */
#include "linkedlist.h"
#include "test_util.h"

static Event mk(const char *id, EventType t, long ts) {
    Event e; memset(&e, 0, sizeof e);
    strncpy(e.emp_id, id, ID_LEN - 1); e.type = t; e.ts = ts; return e;
}
static void count_cb(const Event *e, void *ctx) { (void)e; (*(int *)ctx)++; }

int main(void) {
    LinkedList *l = list_create();
    CHECK(l && list_size(l) == 0);
    CHECK(list_find(l, "E101") == NULL);                  /* edge: empty list */

    Event e1 = mk("E101", EV_LOGIN, 100), e2 = mk("E102", EV_USB_INSERT, 200), e3 = mk("E101", EV_LOGOUT, 300);
    ListNode *n1 = list_append(l, &e1);
    ListNode *n2 = list_append(l, &e2);
    ListNode *n3 = list_append(l, &e3);
    CHECK(n1 && n2 && n3 && list_size(l) == 3);
    CHECK(l->head == n1 && l->tail == n3);
    CHECK(n2->prev == n1 && n2->next == n3);              /* doubly linked */

    CHECK(list_find(l, "E101") == n1);                    /* first match */
    CHECK(list_find(l, "E102") == n2);
    CHECK(list_find(l, "E999") == NULL);

    int cnt = 0; list_traverse(l, count_cb, &cnt); CHECK(cnt == 3);

    CHECK(list_remove(l, n2));                            /* middle O(1) unlink */
    CHECK(list_size(l) == 2 && n1->next == n3 && n3->prev == n1);
    CHECK(list_remove(l, n3));                            /* tail */
    CHECK(l->tail == n1 && n1->next == NULL);
    CHECK(list_remove(l, n1));                            /* head -> empty */
    CHECK(l->head == NULL && l->tail == NULL && list_size(l) == 0);
    CHECK(!list_remove(l, NULL));                         /* edge: NULL node */

    for (int i = 0; i < 5000; i++) { Event e = mk("E1", EV_FILE_ACCESS, i); list_append(l, &e); }
    CHECK(list_size(l) == 5000);
    list_free(l);
    list_free(NULL);
    REPORT("test_list");
}
