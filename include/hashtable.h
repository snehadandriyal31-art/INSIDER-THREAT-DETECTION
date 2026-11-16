/* hashtable.h — emp_id -> Baseline, separate chaining, O(1) average. Owner: M1/M2. */
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HTEntry {
    Baseline        value;      /* key is value.emp_id */
    struct HTEntry *next;
} HTEntry;

typedef struct {
    HTEntry **buckets;
    size_t    capacity;
    size_t    count;
} HashTable;

typedef void (*ht_visit_fn)(const Baseline *b, void *ctx);

HashTable     *ht_create(size_t capacity);
unsigned long  ht_hash(const char *key, size_t capacity);   /* djb2 % capacity */
int            ht_put(HashTable *h, const Baseline *b);     /* insert or replace, 1 ok */
Baseline      *ht_get(const HashTable *h, const char *emp_id);
int            ht_remove(HashTable *h, const char *emp_id);
size_t         ht_count(const HashTable *h);
void           ht_foreach(const HashTable *h, ht_visit_fn fn, void *ctx);
void           ht_free(HashTable *h);

#ifdef __cplusplus
}
#endif
#endif
