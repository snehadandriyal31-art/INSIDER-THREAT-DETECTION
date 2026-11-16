/* test_array.c — dynamic array: push/get/remove/grow + edge cases. Owner: M4. */
#include "array.h"
#include "test_util.h"

int main(void) {
    int a = 1, b = 2, c = 3;
    DynArray *arr = arr_create(2);                 /* tiny capacity forces realloc */
    CHECK(arr != NULL);
    CHECK(arr_size(arr) == 0);
    CHECK(arr_get(arr, 0) == NULL);                /* edge: get from empty */

    CHECK(arr_push(arr, &a));
    CHECK(arr_push(arr, &b));
    CHECK(arr_push(arr, &c));                      /* triggers growth 2 -> 4 */
    CHECK(arr_size(arr) == 3);
    CHECK(arr->capacity >= 3);
    CHECK(*(int *)arr_get(arr, 0) == 1);
    CHECK(*(int *)arr_get(arr, 2) == 3);
    CHECK(arr_get(arr, 3) == NULL);                /* edge: out of range */

    CHECK(arr_remove(arr, 1) == &b);               /* middle removal shifts */
    CHECK(arr_size(arr) == 2);
    CHECK(*(int *)arr_get(arr, 1) == 3);
    CHECK(arr_remove(arr, 5) == NULL);             /* edge: bad index */

    for (int i = 0; i < 1000; i++) CHECK(arr_push(arr, &a));   /* stress growth */
    CHECK(arr_size(arr) == 1002);

    arr_free(arr);
    arr_free(NULL);                                /* edge: NULL safe */
    REPORT("test_array");
}
