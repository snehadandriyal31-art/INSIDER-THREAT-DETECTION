/* hashtable.c — chained hash table for per-employee baselines. Owner: M2. */
#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

HashTable *ht_create(size_t capacity) {
    if (capacity == 0) capacity = 16;
    HashTable *h = (HashTable *)malloc(sizeof(HashTable));
    if (!h) return NULL;
    h->buckets = (HTEntry **)calloc(capacity, sizeof(HTEntry *));
    if (!h->buckets) { free(h); return NULL; }
    h->capacity = capacity;
    h->count = 0;
    return h;
}

unsigned long ht_hash(const char *key, size_t capacity) {
    unsigned long hash = 5381;                    /* djb2 */
    for (const unsigned char *p = (const unsigned char *)key; *p; p++)
        hash = ((hash << 5) + hash) + *p;         /* hash * 33 + c */
    return hash % capacity;
}

int ht_put(HashTable *h, const Baseline *b) {
    if (!h || !b) return 0;
    unsigned long idx = ht_hash(b->emp_id, h->capacity);
    for (HTEntry *e = h->buckets[idx]; e; e = e->next)
        if (strcmp(e->value.emp_id, b->emp_id) == 0) { e->value = *b; return 1; }  /* replace */
    HTEntry *e = (HTEntry *)malloc(sizeof(HTEntry));
    if (!e) return 0;
    e->value = *b;
    e->next = h->buckets[idx];                    /* insert at chain head */
    h->buckets[idx] = e;
    h->count++;
    return 1;
}

Baseline *ht_get(const HashTable *h, const char *emp_id) {
    if (!h || !emp_id) return NULL;
    unsigned long idx = ht_hash(emp_id, h->capacity);
    for (HTEntry *e = h->buckets[idx]; e; e = e->next)
        if (strcmp(e->value.emp_id, emp_id) == 0) return &e->value;
    return NULL;
}

int ht_remove(HashTable *h, const char *emp_id) {
    if (!h || !emp_id) return 0;
    unsigned long idx = ht_hash(emp_id, h->capacity);
    HTEntry *prev = NULL;
    for (HTEntry *e = h->buckets[idx]; e; prev = e, e = e->next) {
        if (strcmp(e->value.emp_id, emp_id) == 0) {
            if (prev) prev->next = e->next; else h->buckets[idx] = e->next;
            free(e);
            h->count--;
            return 1;
        }
    }
    return 0;
}

size_t ht_count(const HashTable *h) { return h ? h->count : 0; }

void ht_foreach(const HashTable *h, ht_visit_fn fn, void *ctx) {
    if (!h || !fn) return;
    for (size_t i = 0; i < h->capacity; i++)
        for (HTEntry *e = h->buckets[i]; e; e = e->next) fn(&e->value, ctx);
}

void ht_free(HashTable *h) {
    if (!h) return;
    for (size_t i = 0; i < h->capacity; i++) {
        HTEntry *e = h->buckets[i];
        while (e) { HTEntry *next = e->next; free(e); e = next; }
    }
    free(h->buckets);
    free(h);
}
