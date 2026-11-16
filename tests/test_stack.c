/* test_stack.c — LIFO order, pop from empty, peek. Owner: M4. */
#include "stack.h"
#include "test_util.h"

static ActionRecord rec(const char *id, AnalystAction a) {
    ActionRecord r; memset(&r, 0, sizeof r);
    strncpy(r.alert.emp_id, id, ID_LEN - 1); r.action = a; return r;
}

int main(void) {
    Stack *s = stack_create();
    ActionRecord out;
    CHECK(s && stack_is_empty(s) && stack_size(s) == 0);
    CHECK(!stack_pop(s, &out));                          /* edge: pop from empty */
    CHECK(stack_peek(s) == NULL);

    ActionRecord r1 = rec("E101", ACT_REVIEWED), r2 = rec("E102", ACT_DISMISSED), r3 = rec("E103", ACT_ESCALATED);
    CHECK(stack_push(s, &r1) && stack_push(s, &r2) && stack_push(s, &r3));
    CHECK(stack_size(s) == 3 && !stack_is_empty(s));
    CHECK(strcmp(stack_peek(s)->alert.emp_id, "E103") == 0);   /* top = last pushed */

    CHECK(stack_pop(s, &out));
    CHECK(strcmp(out.alert.emp_id, "E103") == 0 && out.action == ACT_ESCALATED);
    CHECK(stack_pop(s, &out) && strcmp(out.alert.emp_id, "E102") == 0);
    CHECK(stack_pop(s, &out) && strcmp(out.alert.emp_id, "E101") == 0);  /* LIFO verified */
    CHECK(!stack_pop(s, &out) && stack_is_empty(s));

    CHECK(!stack_push(s, NULL));                         /* edge: NULL record */
    stack_free(s);
    stack_free(NULL);
    REPORT("test_stack");
}
