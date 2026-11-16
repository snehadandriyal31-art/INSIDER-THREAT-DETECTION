/* score.h — explainable risk-scoring engine (statistics + rules, no ML). Owner: M1. */
#ifndef SCORE_H
#define SCORE_H
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Rule flags (bitmask). Each matched rule adds RULE_POINTS to the score. */
#define RULE_AFTER_HOURS    1
#define RULE_FAILED_LOGINS  2
#define RULE_USB            4
#define RULE_MASS_DOWNLOAD  8

#define RULE_POINTS          10.0   /* per rule, max 40 */
#define Z_CAP                 4.0   /* z-scores are clipped to [0, 4] */
#define Z_MAX_POINTS         60.0   /* z component scaled to max 60 */
#define STD_FLOOR             0.5   /* avoid division by ~0 for very stable users */

#define THRESH_MEDIUM        35.0
#define THRESH_HIGH          65.0

#define FAILED_LOGIN_LIMIT    3
#define MASS_DOWNLOAD_LIMIT  15

typedef struct {
    double   z_events, z_downloads, z_after_hours, z_failed;
    int      flags;
    double   z_component;     /* 0..60 */
    double   rule_component;  /* 0..40 */
    double   score;           /* 0..100 */
    RiskBand band;
} RiskReport;

double     compute_zscore(double value, double mean, double std_dev);
int        apply_rules(const DayStats *today);
double     weighted_score(double z_ev, double z_dl, double z_ah, double z_fl, int flags,
                          double *z_part_out, double *rule_part_out);
RiskBand   classify_band(double score);
RiskReport compute_risk(const DayStats *today, const Baseline *baseline);   /* full pipeline */
void       explain_flags(int flags, char *buf, size_t n);                   /* "AFTER_HOURS,USB" */

#ifdef __cplusplus
}
#endif
#endif
