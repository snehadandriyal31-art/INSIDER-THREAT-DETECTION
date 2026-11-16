/*
 * types.h — Common structs shared by the C DSA layer and the C++ OOP layer.
 * Owner: M1 (Lead). Every other header builds on these "contracts".
 */
#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define ID_LEN     16
#define NAME_LEN   64
#define DEPT_LEN   32
#define REASON_LEN 160

/* Kinds of employee activity we log. */
typedef enum {
    EV_LOGIN = 0,
    EV_LOGIN_FAIL,
    EV_FILE_ACCESS,
    EV_FILE_DOWNLOAD,
    EV_USB_INSERT,
    EV_LOGOUT,
    EV_COUNT
} EventType;

/* One timestamped activity record.
 * ts = minutes since 1970-01-01 00:00 (timezone-free, easy to sort/compare). */
typedef struct {
    char      emp_id[ID_LEN];
    EventType type;
    long      ts;
} Event;

/* Aggregated behaviour of ONE employee on ONE day. */
typedef struct {
    int total_events;
    int downloads;
    int after_hours;   /* events outside 08:00-20:00 */
    int failed_logins;
    int usb_events;
} DayStats;

/* The employee's "normal": mean + std-dev of daily metrics over a 30-day window. */
typedef struct {
    char   emp_id[ID_LEN];
    double ev_mean, ev_std;   /* total events / day   */
    double dl_mean, dl_std;   /* downloads / day      */
    double ah_mean, ah_std;   /* after-hours / day    */
    double fl_mean, fl_std;   /* failed logins / day  */
    int    days;              /* window size used     */
} Baseline;

typedef enum { BAND_LOW = 0, BAND_MEDIUM, BAND_HIGH } RiskBand;

/* An alert raised for a HIGH-risk employee; lives in the review queue. */
typedef struct {
    char     emp_id[ID_LEN];
    double   score;
    RiskBand band;
    long     raised_at;
    char     reason[REASON_LEN];
} Alert;

typedef enum { ACT_REVIEWED = 0, ACT_ESCALATED, ACT_DISMISSED } AnalystAction;

/* Snapshot of an analyst decision — pushed on the undo stack. */
typedef struct {
    Alert         alert;
    AnalystAction action;
    char          analyst_id[ID_LEN];
} ActionRecord;

#ifdef __cplusplus
}
#endif
#endif /* TYPES_H */
