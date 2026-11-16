/* sort.h — hand-written quicksort. Owner: M1/M2. */
#ifndef SORT_H
#define SORT_H
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sort alerts by score, highest risk first. Call with lo=0, hi=n-1. */
void quicksort_by_risk(Alert *arr, int lo, int hi);

/* Sort events by timestamp ascending (needed before binary search). */
void quicksort_events_by_time(Event *arr, int lo, int hi);

#ifdef __cplusplus
}
#endif
#endif
