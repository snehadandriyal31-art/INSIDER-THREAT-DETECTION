#!/usr/bin/env python3
"""gen_data.py — regenerates data/employees.txt and data/activity.txt (synthetic, deterministic).
Run from project root:  python3 data/gen_data.py
History: 30 normal days (2026-10-10 .. 2026-11-08) for 6 employees.
Today (2026-11-09): E104 behaves like an insider, E102 is mildly suspicious, rest normal.
"""
import random, datetime, os

random.seed(42)
HERE = os.path.dirname(os.path.abspath(__file__))
TODAY = datetime.date(2026, 11, 9)
DAYS = 30

employees = [
    ("E101", "Aarav Sharma",  "Finance",     "EMPLOYEE"),
    ("E102", "Priya Nair",    "Engineering", "EMPLOYEE"),
    ("E103", "Rohan Mehta",   "HR",          "EMPLOYEE"),
    ("E104", "Sana Khan",     "Finance",     "EMPLOYEE"),
    ("E105", "Vikram Iyer",   "Engineering", "EMPLOYEE"),
    ("E106", "Neha Joshi",    "Sales",       "EMPLOYEE"),
]
analysts = [
    ("A201", "Kabir Rao",  "SecOps", "ANALYST", 2),
    ("A202", "Meera Das",  "SecOps", "ANALYST", 1),
]

with open(os.path.join(HERE, "employees.txt"), "w") as f:
    f.write("# id|name|department|role|clearance(analysts only)\n")
    for e in employees:
        f.write("|".join(e) + "\n")
    for a in analysts:
        f.write("|".join(map(str, a)) + "\n")

events = []  # (datetime, emp, type)

def t(day, h, m):
    return datetime.datetime(day.year, day.month, day.day, h, m)

def normal_day(day, emp):
    login_h = random.choice([8, 9, 9, 9, 10])
    login_m = random.randint(0, 59)
    ev = [(t(day, login_h, login_m), emp, "LOGIN")]
    if random.random() < 0.10:
        ev.append((t(day, login_h, max(0, login_m - 2)), emp, "LOGIN_FAIL"))
    for _ in range(random.randint(7, 13)):
        ev.append((t(day, random.randint(login_h, 17), random.randint(0, 59)), emp, "FILE_ACCESS"))
    for _ in range(random.randint(1, 3)):
        ev.append((t(day, random.randint(login_h, 17), random.randint(0, 59)), emp, "FILE_DOWNLOAD"))
    if random.random() < 0.04:
        ev.append((t(day, 20, random.randint(0, 59)), emp, "FILE_ACCESS"))   # rare late work
    ev.append((t(day, random.choice([17, 18, 18, 19]), random.randint(0, 59)), emp, "LOGOUT"))
    return ev

for i in range(DAYS, 0, -1):
    day = TODAY - datetime.timedelta(days=i)
    for emp, *_ in employees:
        events += normal_day(day, emp)

# ---- today ----
for emp in ("E101", "E103", "E105"):
    events += normal_day(TODAY, emp)

# E106: normal day plus one late-evening file access -> should stay LOW/borderline
events += normal_day(TODAY, "E106")
events.append((t(TODAY, 20, 15), "E106", "FILE_ACCESS"))

# E102: 4 failed logins in the morning + a few extra downloads -> MEDIUM
events += [(t(TODAY, 9, 1 + k), "E102", "LOGIN_FAIL") for k in range(4)]
events.append((t(TODAY, 9, 6), "E102", "LOGIN"))
events += [(t(TODAY, 10 + k, 20), "E102", "FILE_ACCESS") for k in range(6)]
events += [(t(TODAY, 11 + k, 5), "E102", "FILE_DOWNLOAD") for k in range(5)]
events.append((t(TODAY, 18, 10), "E102", "LOGOUT"))

# E104: the insider. Logs in at 06:10, normal-looking day, then 20:30-23:50 mass download + USB -> HIGH
events.append((t(TODAY, 6, 10), "E104", "LOGIN"))
events += [(t(TODAY, random.randint(9, 17), random.randint(0, 59)), "E104", "FILE_ACCESS") for _ in range(8)]
events.append((t(TODAY, 20, 30), "E104", "LOGIN"))
events += [(t(TODAY, 20 + k // 8, 32 + (k % 8) * 3), "E104", "FILE_DOWNLOAD") for k in range(25)]
events.append((t(TODAY, 23, 40), "E104", "USB_INSERT"))
events.append((t(TODAY, 23, 47), "E104", "USB_INSERT"))
events.append((t(TODAY, 23, 55), "E104", "LOGOUT"))

events.sort()
with open(os.path.join(HERE, "activity.txt"), "w") as f:
    f.write("# emp_id|event_type|YYYY-MM-DD HH:MM\n")
    for dt, emp, typ in events:
        f.write(f"{emp}|{typ}|{dt.strftime('%Y-%m-%d %H:%M')}\n")

print(f"wrote {len(events)} events for {len(employees)} employees over {DAYS}+1 days")
