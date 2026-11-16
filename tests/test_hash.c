/* test_hash.c — put/get/replace/remove, collisions, missing key, foreach. Owner: M4. */
#include "hashtable.h"
#include "test_util.h"

static Baseline bl(const char *id, double ev_mean) {
    Baseline b; memset(&b, 0, sizeof b);
    strncpy(b.emp_id, id, ID_LEN - 1); b.ev_mean = ev_mean; b.days = 30; return b;
}
static void sum_cb(const Baseline *b, void *ctx) { *(double *)ctx += b->ev_mean; }

int main(void) {
    HashTable *h = ht_create(4);                 /* tiny capacity -> guaranteed collisions */
    CHECK(h && ht_count(h) == 0);
    CHECK(ht_get(h, "E104") == NULL);            /* edge: missing key on empty table */

    Baseline b1 = bl("E101", 10), b2 = bl("E102", 20), b3 = bl("E103", 30), b4 = bl("E104", 40), b5 = bl("E105", 50);
    CHECK(ht_put(h, &b1) && ht_put(h, &b2) && ht_put(h, &b3) && ht_put(h, &b4) && ht_put(h, &b5));
    CHECK(ht_count(h) == 5);                     /* 5 keys in 4 buckets => chaining works */

    Baseline *g = ht_get(h, "E104");
    CHECK(g && g->ev_mean == 40 && g->days == 30);
    CHECK(ht_get(h, "E999") == NULL);            /* edge: missing key */

    Baseline b4b = bl("E104", 44);
    CHECK(ht_put(h, &b4b) && ht_count(h) == 5);  /* replace, not duplicate */
    CHECK(ht_get(h, "E104")->ev_mean == 44);

    double sum = 0; ht_foreach(h, sum_cb, &sum); CHECK(sum == 10 + 20 + 30 + 44 + 50);

    CHECK(ht_remove(h, "E102") && ht_count(h) == 4 && ht_get(h, "E102") == NULL);
    CHECK(!ht_remove(h, "E102"));                /* edge: remove twice */
    CHECK(ht_hash("E101", 4) < 4 && ht_hash("E101", 4) == ht_hash("E101", 4));   /* deterministic */

    ht_free(h);
    ht_free(NULL);
    REPORT("test_hash");
}
