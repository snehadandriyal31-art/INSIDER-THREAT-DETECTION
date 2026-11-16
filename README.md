# Insider Threat Detection using Behavioral Analysis

**Team Sentinels — PBL project (C data structures + C++ OOP)**

A console application that detects insider threats from *behavioural deviation*: every employee's
activity for today is compared against their own 30-day baseline, turned into an explainable 0–100
risk score, and HIGH-risk cases are queued for a human analyst to review (with undo).

No libraries, no machine learning: every data structure is hand-written in C, the application layer
is C++ OOP, and the two are joined by a real `extern "C"` bridge.

```
C++ APPLICATION LAYER  (g++, C++17)
  ui.cpp -> ActivityManager / AlertManager -> User(abstract) -> Employee | Analyst
                 |  extern "C"  (include/*.h)
C DSA LAYER  (gcc, C11)
  dynamic array | linked list | stack | queue | hash table | quicksort | binary search | score engine

Pipeline:  event logged -> baseline lookup (hash) -> z-scores + rule flags -> score 0-100
           -> LOW / MEDIUM / HIGH -> HIGH auto-enqueued (queue) -> analyst decision -> undo (stack)
```

## Build & run

**Linux / WSL / macOS / Git Bash (recommended)**
```bash
make            # builds ./insider_detect  (-Wall -Wextra, zero warnings)
make test       # builds + runs the 7 unit-test suites
make asan       # rebuilds with AddressSanitizer + LeakSanitizer, runs tests (Linux/WSL)
./insider_detect
```

**Windows without make** (MinGW-w64 gcc/g++ on PATH):
```bat
build.bat
insider_detect.exe
tests\run_tests.bat
```

Run from the project root — the program reads and writes the text files in `data/`.
Optional: `./insider_detect path/to/other/data/dir`.

## Menu

| # | Option | What it demonstrates |
|---|--------|----------------------|
| 1 | Display users | abstract `User`, `Employee`/`Analyst`, virtual dispatch |
| 2 | View activity log | C doubly linked list, overloaded `searchActivity()` |
| 3 | Employee summary | O(1) hash-table baseline lookup, `generateSummary()` override |
| 4 | Add simulated activity | `list_append`, input validation |
| 5 | Sort + search events | hand-written quicksort, binary search, linear search |
| 6 | Compute risk scores | z-score + rules -> weighted score -> band; HIGH auto-enqueued |
| 7 | View alert queue | FIFO queue |
| 8 | Process next alert | `dequeue` + push `ActionRecord` on undo stack |
| 9 | Undo last action | `stack_pop` + `enqueue_front` |
| 10 | Risk-ranked view | quicksort by score, descending |
| 11 | Rebuild baselines | mean/std over the 30 days before today |
| 12 | Save & exit | pipe-delimited persistence of `data/*.txt` |

## Scoring (fully explainable)

For each of 4 daily metrics (events, downloads, after-hours events, failed logins):
`z = (today - mean) / std` against the employee's own 30-day baseline (std floored at 0.5, z clipped to [0, 4]).

`score = 60 * (0.15·z_ev + 0.30·z_dl + 0.25·z_ah + 0.30·z_fl) / 4  +  10 per rule flag (max 40)`

Rules: any after-hours event (before 08:00 / 20:00+), ≥3 failed logins, any USB insert, ≥15 downloads.
Bands: `LOW < 35 ≤ MEDIUM < 65 ≤ HIGH`. All constants live in `include/score.h`.

A high score is a **risk indicator, not proof of misconduct** — the final decision is the analyst's.

## Repository layout (one folder = one member)

```
include/        M1  headers = contracts between layers (types.h, array.h ... score.h, util.h)
src/c/          M2  hand-written C data structures + util.c; score.c (M1) = scoring engine
src/cpp/        M3  OOP classes, managers, persistence, ui, main
tests/          M4  one C test suite per structure + test_score; run_tests.sh / .bat
data/           M4  employees.txt, activity.txt, baselines.txt, alerts.txt, gen_data.py
```

## Data files

| File | Format |
|------|--------|
| `employees.txt` | `id\|name\|department\|ROLE[\|clearance]` |
| `activity.txt`  | `emp_id\|EVENT_TYPE\|YYYY-MM-DD HH:MM` (2 654 synthetic events, 30 days + today) |
| `baselines.txt` | `emp_id\|days\|ev_mean\|ev_std\|dl_mean\|dl_std\|ah_mean\|ah_std\|fl_mean\|fl_std` |
| `alerts.txt`    | pending review queue in FIFO order |

`python3 data/gen_data.py` regenerates the synthetic log (deterministic, seed 42). Delete
`baselines.txt` / `alerts.txt` to reset the demo; baselines are rebuilt automatically on startup.

## Memory ownership rule

Memory `malloc`'d in C is freed only by the matching C `*_free()` function; objects `new`'d in C++
are deleted only in C++ (virtual destructor on `User`). Managers acquire their C structure in the
constructor and release it in the destructor (RAII). Verified with `make asan`: 0 errors, 0 leaks.
