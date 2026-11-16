/* util.c — timestamp maths (no timezone dependence) + enum <-> text. Owner: M1. */
#include "util.h"
#include <stdio.h>
#include <string.h>

/* Civil-date algorithms (Howard Hinnant). Days since 1970-01-01. */
long util_days_from_civil(int y, int m, int d) {
    y -= (m <= 2);
    long era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned mp  = (unsigned)(m > 2 ? m - 3 : m + 9);
    unsigned doy = (153u * mp + 2u) / 5u + (unsigned)d - 1u;
    unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
    return era * 146097L + (long)doe - 719468L;
}

void util_civil_from_days(long z, int *y, int *m, int *d) {
    z += 719468L;
    long era = (z >= 0 ? z : z - 146096) / 146097;
    unsigned doe = (unsigned)(z - era * 146097);
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long yy = (long)yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp  = (5 * doy + 2) / 153;
    unsigned dd  = doy - (153 * mp + 2) / 5 + 1;
    unsigned mm  = mp < 10 ? mp + 3 : mp - 9;
    *y = (int)(yy + (mm <= 2));
    *m = (int)mm;
    *d = (int)dd;
}

long util_make_ts(int y, int m, int d, int hh, int mm) {
    return util_days_from_civil(y, m, d) * 1440L + hh * 60L + mm;
}

int util_parse_ts(const char *s, long *out) {
    int y, m, d, hh, mm;
    if (!s || !out) return 0;
    if (sscanf(s, "%d-%d-%d %d:%d", &y, &m, &d, &hh, &mm) != 5) return 0;
    if (m < 1 || m > 12 || d < 1 || d > 31 || hh < 0 || hh > 23 || mm < 0 || mm > 59) return 0;
    *out = util_make_ts(y, m, d, hh, mm);
    return 1;
}

void util_format_ts(long ts, char *buf, size_t n) {
    int y, m, d;
    long day = ts / 1440L, rem = ts % 1440L;
    if (rem < 0) { rem += 1440; day -= 1; }
    util_civil_from_days(day, &y, &m, &d);
    snprintf(buf, n, "%04d-%02d-%02d %02ld:%02ld", y, m, d, rem / 60, rem % 60);
}

void util_format_day(long day, char *buf, size_t n) {
    int y, m, d;
    util_civil_from_days(day, &y, &m, &d);
    snprintf(buf, n, "%04d-%02d-%02d", y, m, d);
}

long util_ts_day(long ts)  { return ts >= 0 ? ts / 1440L : (ts - 1439) / 1440L; }
int  util_ts_hour(long ts) { long rem = ts % 1440L; if (rem < 0) rem += 1440; return (int)(rem / 60); }
int  util_is_after_hours(long ts) { int h = util_ts_hour(ts); return h < 8 || h >= 20; }

static const char *EVENT_NAMES[EV_COUNT] = {
    "LOGIN", "LOGIN_FAIL", "FILE_ACCESS", "FILE_DOWNLOAD", "USB_INSERT", "LOGOUT"
};

const char *util_event_name(EventType t) {
    return (t >= 0 && t < EV_COUNT) ? EVENT_NAMES[t] : "UNKNOWN";
}

int util_event_from_name(const char *s, EventType *out) {
    for (int i = 0; i < EV_COUNT; i++)
        if (strcmp(s, EVENT_NAMES[i]) == 0) { *out = (EventType)i; return 1; }
    return 0;
}

const char *util_band_name(RiskBand b) {
    switch (b) { case BAND_LOW: return "LOW"; case BAND_MEDIUM: return "MEDIUM"; default: return "HIGH"; }
}

const char *util_action_name(AnalystAction a) {
    switch (a) { case ACT_REVIEWED: return "REVIEWED"; case ACT_ESCALATED: return "ESCALATED"; default: return "DISMISSED"; }
}
