/* array.h — dynamic array of void* (employee registry holds User* objects). Owner: M1/M2. */
#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void  **items;
    size_t  size;
    size_t  capacity;
} DynArray;

DynArray *arr_create(size_t initial_capacity);
int       arr_push(DynArray *a, void *item);          /* 1 ok, 0 on alloc failure */
void     *arr_get(const DynArray *a, size_t i);       /* O(1); NULL if out of range */
size_t    arr_size(const DynArray *a);
void     *arr_remove(DynArray *a, size_t i);          /* returns removed item, shifts left */
void      arr_free(DynArray *a);                      /* frees the array, NOT the items */

#ifdef __cplusplus
}
#endif
#endif
