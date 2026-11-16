/* search.h — binary search (sorted) + linear search (small/unsorted). Owner: M1/M2. */
#ifndef SEARCH_H
#define SEARCH_H
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* arr must be sorted ascending by ts. Returns index or -1. O(log n). */
int binary_search_by_time(const Event *arr, int n, long ts);

/* Returns index of first alert for emp_id, or -1. O(n). */
int linear_search_alert(const Alert *arr, int n, const char *emp_id);

/* Returns index of first event for emp_id, or -1. O(n). */
int linear_search_event(const Event *arr, int n, const char *emp_id);

#ifdef __cplusplus
}
#endif
#endif
