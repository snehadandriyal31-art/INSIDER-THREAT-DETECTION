/* search.c — binary search over a time-sorted array, linear search otherwise. Owner: M2. */
#include "search.h"
#include <string.h>

int binary_search_by_time(const Event *arr, int n, long ts) {
    if (!arr || n <= 0) return -1;
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid].ts == ts) {
            while (mid > 0 && arr[mid - 1].ts == ts) mid--;   /* first of equal timestamps */
            return mid;
        }
        if (arr[mid].ts < ts) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

int linear_search_alert(const Alert *arr, int n, const char *emp_id) {
    if (!arr || !emp_id) return -1;
    for (int i = 0; i < n; i++)
        if (strcmp(arr[i].emp_id, emp_id) == 0) return i;
    return -1;
}

int linear_search_event(const Event *arr, int n, const char *emp_id) {
    if (!arr || !emp_id) return -1;
    for (int i = 0; i < n; i++)
        if (strcmp(arr[i].emp_id, emp_id) == 0) return i;
    return -1;
}
