/* test_util.h — tiny assert framework shared by all C test suites. Owner: M4. */
#ifndef TEST_UTIL_H
#define TEST_UTIL_H
#include <stdio.h>
#include <string.h>

static int tests_run = 0, tests_failed = 0;

#define CHECK(cond) do { tests_run++; if (!(cond)) { tests_failed++; \
    printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

#define REPORT(name) do { \
    printf("%-18s %d checks, %d failed -> %s\n", name, tests_run, tests_failed, tests_failed ? "FAIL" : "PASS"); \
    return tests_failed ? 1 : 0; } while (0)

#endif
