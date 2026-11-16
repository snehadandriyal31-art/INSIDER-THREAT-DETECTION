/* score.c — deviation -> 0..100 risk score. Owner: M1.
 *
 * Pipeline:  today's DayStats  vs  employee's 30-day Baseline
 *            -> z-score per metric (how many std-devs above normal)
 *            -> rule flags (after-hours, failed logins, USB, mass download)
 *            -> weighted score: 60 pts from z-scores + 10 pts per rule (max 40)
 *            -> band: LOW < 35 <= MEDIUM < 65 <= HIGH
 * Every number is traceable — no black box.
 */
#include "score.h"
#include <stdio.h>
#include <string.h>

/* Weights of the four z-scores (sum = 1.0). Downloads and failed logins matter most. */
#define W_EVENTS       0.15
#define W_DOWNLOADS    0.30
#define W_AFTER_HOURS  0.25
#define W_FAILED       0.30

double compute_zscore(double value, double mean, double std_dev) {
    if (std_dev < STD_FLOOR) std_dev = STD_FLOOR;
    return (value - mean) / std_dev;
}

int apply_rules(const DayStats *t) {
    int flags = 0;
    if (!t) return 0;
    if (t->after_hours   >= 1)                   flags |= RULE_AFTER_HOURS;
    if (t->failed_logins >= FAILED_LOGIN_LIMIT)  flags |= RULE_FAILED_LOGINS;
    if (t->usb_events    >= 1)                   flags |= RULE_USB;
    if (t->downloads     >= MASS_DOWNLOAD_LIMIT) flags |= RULE_MASS_DOWNLOAD;
    return flags;
}

static double clip_z(double z) {
    if (z < 0)     return 0;       /* being *below* normal is not suspicious */
    if (z > Z_CAP) return Z_CAP;
    return z;
}

double weighted_score(double z_ev, double z_dl, double z_ah, double z_fl, int flags,
                      double *z_part_out, double *rule_part_out) {
    double weighted = W_EVENTS      * clip_z(z_ev)
                    + W_DOWNLOADS   * clip_z(z_dl)
                    + W_AFTER_HOURS * clip_z(z_ah)
                    + W_FAILED      * clip_z(z_fl);           /* 0 .. Z_CAP */
    double z_part = weighted / Z_CAP * Z_MAX_POINTS;          /* 0 .. 60 */

    int nrules = 0;
    for (int f = flags; f; f >>= 1) nrules += (f & 1);
    double rule_part = nrules * RULE_POINTS;                  /* 0 .. 40 */

    double score = z_part + rule_part;
    if (score > 100.0) score = 100.0;
    if (score < 0.0)   score = 0.0;
    if (z_part_out)    *z_part_out = z_part;
    if (rule_part_out) *rule_part_out = rule_part;
    return score;
}

RiskBand classify_band(double score) {
    if (score >= THRESH_HIGH)   return BAND_HIGH;
    if (score >= THRESH_MEDIUM) return BAND_MEDIUM;
    return BAND_LOW;
}

RiskReport compute_risk(const DayStats *today, const Baseline *b) {
    RiskReport r;
    memset(&r, 0, sizeof r);
    if (!today || !b) return r;
    r.z_events      = compute_zscore(today->total_events,  b->ev_mean, b->ev_std);
    r.z_downloads   = compute_zscore(today->downloads,     b->dl_mean, b->dl_std);
    r.z_after_hours = compute_zscore(today->after_hours,   b->ah_mean, b->ah_std);
    r.z_failed      = compute_zscore(today->failed_logins, b->fl_mean, b->fl_std);
    r.flags         = apply_rules(today);
    r.score         = weighted_score(r.z_events, r.z_downloads, r.z_after_hours, r.z_failed,
                                     r.flags, &r.z_component, &r.rule_component);
    r.band          = classify_band(r.score);
    return r;
}

void explain_flags(int flags, char *buf, size_t n) {
    if (!buf || n == 0) return;
    buf[0] = '\0';
    const char *names[] = { "AFTER_HOURS", "FAILED_LOGINS", "USB", "MASS_DOWNLOAD" };
    int first = 1;
    for (int i = 0; i < 4; i++) {
        if (flags & (1 << i)) {
            if (!first) strncat(buf, ",", n - strlen(buf) - 1);
            strncat(buf, names[i], n - strlen(buf) - 1);
            first = 0;
        }
    }
    if (first) strncpy(buf, "none", n - 1), buf[n - 1] = '\0';
}
