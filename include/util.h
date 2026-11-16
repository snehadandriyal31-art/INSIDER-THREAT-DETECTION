/* util.h — time + name helpers used by both layers. Owner: M1. */
#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

long  util_days_from_civil(int y, int m, int d);
void  util_civil_from_days(long days, int *y, int *m, int *d);
long  util_make_ts(int y, int m, int d, int hh, int mm);
int   util_parse_ts(const char *s, long *out);            /* "YYYY-MM-DD HH:MM" -> 1 ok / 0 bad */
void  util_format_ts(long ts, char *buf, size_t n);       /* -> "YYYY-MM-DD HH:MM" */
void  util_format_day(long day, char *buf, size_t n);     /* -> "YYYY-MM-DD" */
long  util_ts_day(long ts);
int   util_ts_hour(long ts);
int   util_is_after_hours(long ts);                       /* before 08:00 or 20:00+ */

const char *util_event_name(EventType t);
int         util_event_from_name(const char *s, EventType *out);
const char *util_band_name(RiskBand b);
const char *util_action_name(AnalystAction a);

#ifdef __cplusplus
}
#endif
#endif
