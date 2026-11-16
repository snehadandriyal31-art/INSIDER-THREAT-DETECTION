/* sort.c — quicksort (Lomuto partition, middle pivot). Owner: M2.
 * Average O(n log n); middle pivot avoids the worst case on already-sorted input. */
#include "sort.h"

static void swap_alert(Alert *a, Alert *b) { Alert t = *a; *a = *b; *b = t; }
static void swap_event(Event *a, Event *b) { Event t = *a; *a = *b; *b = t; }

/* ---- alerts: descending by score ---- */
static int partition_alert(Alert *arr, int lo, int hi) {
    swap_alert(&arr[(lo + hi) / 2], &arr[hi]);     /* move pivot to end */
    double pivot = arr[hi].score;
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j].score > pivot) { i++; swap_alert(&arr[i], &arr[j]); }
    }
    swap_alert(&arr[i + 1], &arr[hi]);
    return i + 1;
}

void quicksort_by_risk(Alert *arr, int lo, int hi) {
    if (!arr || lo >= hi) return;
    int p = partition_alert(arr, lo, hi);
    quicksort_by_risk(arr, lo, p - 1);
    quicksort_by_risk(arr, p + 1, hi);
}

/* ---- events: ascending by timestamp ---- */
static int partition_event(Event *arr, int lo, int hi) {
    swap_event(&arr[(lo + hi) / 2], &arr[hi]);
    long pivot = arr[hi].ts;
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j].ts < pivot) { i++; swap_event(&arr[i], &arr[j]); }
    }
    swap_event(&arr[i + 1], &arr[hi]);
    return i + 1;
}

void quicksort_events_by_time(Event *arr, int lo, int hi) {
    if (!arr || lo >= hi) return;
    int p = partition_event(arr, lo, hi);
    quicksort_events_by_time(arr, lo, p - 1);
    quicksort_events_by_time(arr, p + 1, hi);
}
