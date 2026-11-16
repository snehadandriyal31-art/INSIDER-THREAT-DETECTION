/* test_score.c — z-score, rules, weighting, bands, full pipeline. Owner: M4. */
#include "score.h"
#include "util.h"
#include "test_util.h"
#include <math.h>

static int near(double a, double b) { return fabs(a - b) < 1e-6; }

int main(void) {
    /* ---- z-score ---- */
    CHECK(near(compute_zscore(12, 10, 1), 2.0));
    CHECK(near(compute_zscore(10, 10, 1), 0.0));
    CHECK(near(compute_zscore(5, 10, 0.0), -10.0));      /* std floor 0.5 kicks in: (5-10)/0.5 */

    /* ---- rules ---- */
    DayStats quiet = { 10, 2, 0, 0, 0 };
    DayStats bad   = { 40, 25, 6, 4, 2 };
    CHECK(apply_rules(&quiet) == 0);
    CHECK(apply_rules(&bad) == (RULE_AFTER_HOURS | RULE_FAILED_LOGINS | RULE_USB | RULE_MASS_DOWNLOAD));
    DayStats two_fail = { 10, 2, 0, 2, 0 };
    CHECK((apply_rules(&two_fail) & RULE_FAILED_LOGINS) == 0);   /* below limit of 3 */
    CHECK(apply_rules(NULL) == 0);

    /* ---- weighted score ---- */
    double zp, rp;
    CHECK(near(weighted_score(0, 0, 0, 0, 0, &zp, &rp), 0.0));
    CHECK(near(weighted_score(4, 4, 4, 4, 0, &zp, &rp), 60.0) && near(rp, 0));        /* z maxes at 60 */
    CHECK(near(weighted_score(99, 99, 99, 99, 15, &zp, &rp), 100.0) && near(rp, 40)); /* clipped + 4 rules */
    CHECK(near(weighted_score(-5, -5, -5, -5, 0, &zp, &rp), 0.0));                    /* below-normal is not risky */
    CHECK(near(weighted_score(0, 4, 0, 0, 0, &zp, &rp), 0.30 * 4 / 4 * 60));          /* downloads weight 0.30 */

    /* ---- bands ---- */
    CHECK(classify_band(0) == BAND_LOW && classify_band(34.99) == BAND_LOW);
    CHECK(classify_band(35) == BAND_MEDIUM && classify_band(64.99) == BAND_MEDIUM);
    CHECK(classify_band(65) == BAND_HIGH && classify_band(100) == BAND_HIGH);

    /* ---- full pipeline ---- */
    Baseline b; memset(&b, 0, sizeof b);
    b.ev_mean = 12; b.ev_std = 3; b.dl_mean = 2; b.dl_std = 1.2; b.ah_mean = 0.1; b.ah_std = 0.3; b.fl_mean = 0.2; b.fl_std = 0.4;
    RiskReport r = compute_risk(&bad, &b);
    CHECK(r.band == BAND_HIGH && r.score >= 65);
    CHECK(r.flags == 15);
    RiskReport q = compute_risk(&quiet, &b);
    CHECK(q.band == BAND_LOW && q.score < 35);
    RiskReport n = compute_risk(NULL, &b);
    CHECK(n.score == 0 && n.band == BAND_LOW);

    char buf[64];
    explain_flags(RULE_USB | RULE_MASS_DOWNLOAD, buf, sizeof buf);
    CHECK(strcmp(buf, "USB,MASS_DOWNLOAD") == 0);
    explain_flags(0, buf, sizeof buf);
    CHECK(strcmp(buf, "none") == 0);

    /* ---- util round trip ---- */
    long ts; char out[32];
    CHECK(util_parse_ts("2026-11-09 23:47", &ts));
    util_format_ts(ts, out, sizeof out);
    CHECK(strcmp(out, "2026-11-09 23:47") == 0);
    CHECK(util_is_after_hours(ts) && !util_is_after_hours(ts - 15 * 60));
    CHECK(!util_parse_ts("garbage", &ts) && !util_parse_ts("2026-13-01 00:00", &ts));
    REPORT("test_score");
}
