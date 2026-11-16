/* test_sortsearch.c — quicksort (both orders) + binary/linear search. Owner: M4. */
#include "sort.h"
#include "search.h"
#include "test_util.h"

static Alert al(const char *id, double s) { Alert a; memset(&a, 0, sizeof a); strncpy(a.emp_id, id, ID_LEN - 1); a.score = s; return a; }
static Event ev(const char *id, long ts)  { Event e; memset(&e, 0, sizeof e); strncpy(e.emp_id, id, ID_LEN - 1); e.ts = ts; return e; }

int main(void) {
    /* ---- quicksort alerts, descending ---- */
    Alert a[6] = { al("A", 40), al("B", 90), al("C", 10), al("D", 90), al("E", 65), al("F", 0) };
    quicksort_by_risk(a, 0, 5);
    for (int i = 0; i < 5; i++) CHECK(a[i].score >= a[i + 1].score);
    CHECK(a[0].score == 90 && a[5].score == 0);
    quicksort_by_risk(a, 0, 0);                    /* edge: single element */
    quicksort_by_risk(NULL, 0, 5);                 /* edge: NULL */
    quicksort_by_risk(a, 0, -1);                   /* edge: empty range */

    /* ---- quicksort events, ascending; already-sorted + reverse inputs ---- */
    Event e[200];
    for (int i = 0; i < 200; i++) e[i] = ev("E1", 199 - i);          /* reverse sorted */
    quicksort_events_by_time(e, 0, 199);
    int ok = 1; for (int i = 0; i < 199; i++) if (e[i].ts > e[i + 1].ts) ok = 0;
    CHECK(ok && e[0].ts == 0 && e[199].ts == 199);
    quicksort_events_by_time(e, 0, 199);           /* already sorted: must stay sorted */
    ok = 1; for (int i = 0; i < 199; i++) if (e[i].ts > e[i + 1].ts) ok = 0;
    CHECK(ok);

    /* ---- binary search ---- */
    CHECK(binary_search_by_time(e, 200, 0) == 0);
    CHECK(binary_search_by_time(e, 200, 199) == 199);
    CHECK(binary_search_by_time(e, 200, 77) == 77);
    CHECK(binary_search_by_time(e, 200, 500) == -1);     /* edge: absent */
    CHECK(binary_search_by_time(e, 0, 5) == -1);         /* edge: empty */
    Event d[4] = { ev("A", 1), ev("B", 5), ev("C", 5), ev("D", 9) };
    CHECK(binary_search_by_time(d, 4, 5) == 1);          /* first of duplicates */

    /* ---- linear search ---- */
    CHECK(linear_search_alert(a, 6, "C") >= 0 && strcmp(a[linear_search_alert(a, 6, "C")].emp_id, "C") == 0);
    CHECK(linear_search_alert(a, 6, "Z") == -1);
    CHECK(linear_search_event(d, 4, "C") == 2);
    CHECK(linear_search_event(d, 4, "Q") == -1);
    CHECK(linear_search_event(NULL, 4, "Q") == -1);
    REPORT("test_sortsearch");
}
