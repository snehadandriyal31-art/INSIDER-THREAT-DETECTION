/* array.c — hand-written dynamic array (malloc / realloc / free). Owner: M2. */
#include "array.h"
#include <stdlib.h>
#include <string.h>

DynArray *arr_create(size_t initial_capacity) {
    DynArray *a = (DynArray *)malloc(sizeof(DynArray));
    if (!a) return NULL;
    if (initial_capacity == 0) initial_capacity = 4;
    a->items = (void **)malloc(initial_capacity * sizeof(void *));
    if (!a->items) { free(a); return NULL; }
    a->size = 0;
    a->capacity = initial_capacity;
    return a;
}

int arr_push(DynArray *a, void *item) {
    if (!a) return 0;
    if (a->size == a->capacity) {                 /* grow x2 — amortised O(1) push */
        size_t ncap = a->capacity * 2;
        void **tmp = (void **)realloc(a->items, ncap * sizeof(void *));
        if (!tmp) return 0;
        a->items = tmp;
        a->capacity = ncap;
    }
    a->items[a->size++] = item;
    return 1;
}

void *arr_get(const DynArray *a, size_t i) {
    if (!a || i >= a->size) return NULL;
    return a->items[i];
}

size_t arr_size(const DynArray *a) { return a ? a->size : 0; }

void *arr_remove(DynArray *a, size_t i) {
    if (!a || i >= a->size) return NULL;
    void *removed = a->items[i];
    memmove(&a->items[i], &a->items[i + 1], (a->size - i - 1) * sizeof(void *));
    a->size--;
    return removed;
}

void arr_free(DynArray *a) {
    if (!a) return;
    free(a->items);
    free(a);
}
