# DEMO & VIVA GUIDE — Insider Threat Detection (Team Sentinels)

Read this once before the evaluation. Sections: (1) setup, (2) 4-minute demo script,
(3) how to explain the code, (4) expected viva questions with answers, (5) common mistakes.

---

## 1. Setup on your PC (do this the day before, not in front of the teacher)

**Windows — easiest path: WSL or Git Bash + MinGW**
1. Install MinGW-w64 (e.g. via MSYS2: `pacman -S mingw-w64-ucrt-x86_64-gcc make`) or use WSL Ubuntu
   (`sudo apt install build-essential`).
2. Unzip the project. Open a terminal **in the project root** (the folder containing `Makefile`).
3. `make` → produces `insider_detect`. `make test` → 7 suites PASS.
   (No make? run `build.bat` then `tests\run_tests.bat`.)
4. `./insider_detect` (Windows: `insider_detect.exe`). You should see
   `Loaded 8 users, 2654 events, 6 baselines, 0 pending alerts.`

**Linux / Mac:** `make && make test && ./insider_detect`.

**Reset before every demo** so the alert gets raised live:
`rm -f data/alerts.txt` (baselines can stay). If you added events while practising,
regenerate the log: `python3 data/gen_data.py`.

Open a second terminal with the source code (VS Code) so you can jump to files when asked.

---

## 2. The 4-minute live demo (menu numbers in brackets)

Say the one-line story first:
> "Every employee has a 30-day 'normal'. Today we compare each person against *their own* normal,
> score the deviation 0–100, and send HIGH cases to a human analyst. Nothing is a black box."

| Step | Do | Say |
|------|----|-----|
| 1 | `[1]` Display users | "One loop over a C dynamic array of `User*`. Employees and Analysts print different roles — that's runtime polymorphism via `virtual role()`." |
| 2 | `[2]` → `3` → `E104` | "Activity log is a C doubly linked list. Watch E104: login at 06:10, then 25 downloads after 20:30 and two USB inserts at 23:40 — all marked *after hours*." |
| 3 | `[3]` → `E104` | "Summary = hash-table lookup of her baseline: ~14 events/day, ~2 downloads/day. Today: 38 events, 25 downloads, 30 after-hours. That gap is the signal." |
| 4 | `[6]` Compute risk | "This is the engine. z-score per metric, four rules, weighted to 0–100. E104 = 72 → HIGH and is auto-enqueued. E102 = 48 MEDIUM (4 failed logins). Others LOW." |
| 5 | `[7]` View queue | "FIFO queue — alerts are reviewed in the order raised." |
| 6 | `[8]` → `2` (escalate) | "Analyst dequeues and decides. The decision is pushed on a stack." |
| 7 | `[9]` Undo | "Stack pop → alert back at the front of the queue. Mistakes are reversible." |
| 8 | `[10]` Ranked view | "Hand-written quicksort, descending by score." |
| 9 | `[5]` → `20` `30` → `E104` | "Quicksort today's events by time, then binary search for 20:30 — O(log n) — finds E104's evening login; then linear search by employee." |
| 10 | `[4]` → `E101` `5` `23` `30`, then `[6]` again | "Add a USB insert at 23:30 for a normal employee — rerun scoring: E101 moves from 7.8 LOW to 36.2 MEDIUM. Proves it's live, not hard-coded." |
| 11 | `[12]` Save & exit, then restart | "Pipe-delimited text persistence — the alert survives restart." |

If short on time: do steps 2, 3, 4, 6, 7 only (≈90 s).

Close with: *"A score is a risk indicator, not proof. The system ranks and explains; the human decides."*

---

## 3. How to explain the code (viva order)

**Golden sentence:** *"include/ has the contracts, src/c/ has the tools, src/cpp/ has the managers,
tests/ and data/ are the proof."*

### 3.1 The bridge — most important thing to show
Open `include/queue.h`: every header has
```c
#ifdef __cplusplus
extern "C" {
#endif
```
"C++ mangles function names; `extern "C"` tells g++ to use C linkage so `enqueue()` compiled by gcc
links with the C++ caller. Same struct layout on both sides because `types.h` is plain C."

Then open `src/cpp/alert_manager.cpp` → `processNext()`:
`dequeue()` (C) → `stack_push()` (C) called from a C++ method. **One menu option = C++ manager +
bridge + C queue + C stack + C scoring engine.** That is genuine integration.

### 3.2 One data structure, one job
| Structure | File | Job | Why this one |
|-----------|------|-----|--------------|
| Dynamic array | `array.c` | registry of `User*` | O(1) index, grows ×2 (amortised O(1) push) |
| Doubly linked list | `linkedlist.c` | activity log | O(1) append at tail, O(1) unlink |
| Hash table (chaining, djb2) | `hashtable.c` | emp_id → Baseline | O(1) average lookup per event |
| Queue (FIFO) | `queue.c` | alert review line | fairness: first raised, first reviewed |
| Stack (LIFO) | `stack.c` | undo analyst actions | last action undone first |
| Quicksort | `sort.c` | rank by risk / sort by time | O(n log n) average, in place |
| Binary search | `search.c` | find event by time | O(log n) on sorted array |

### 3.3 The scoring engine — `src/c/score.c` (be able to do this on paper)
1. `compute_zscore(value, mean, std)` = (today − mean)/std, std floored at 0.5.
2. `apply_rules()` → bitmask: AFTER_HOURS=1, FAILED_LOGINS=2, USB=4, MASS_DOWNLOAD=8.
3. `weighted_score()`: clip each z to [0,4]; weighted = 0.15·z_ev + 0.30·z_dl + 0.25·z_ah + 0.30·z_fl;
   z-part = weighted/4 × 60 (max 60); rule-part = 10 × number of flags (max 40); sum, cap 100.
4. `classify_band()`: <35 LOW, <65 MEDIUM, else HIGH.

Worked example for E104: all four z's except failed-logins exceed 4 → clipped to 4 →
weighted = 0.15·4 + 0.30·4 + 0.25·4 + 0 = 2.8 → z-part = 2.8/4·60 = **42**. Three rules → **30**. Score **72 → HIGH**.

### 3.4 OOP checklist (point at the line)
- **Abstraction:** `User::generateSummary() = 0` (`user.h`)
- **Inheritance:** `class Employee : public User`, `class Analyst : public User`
- **Polymorphism:** `displayUsers()` in `ui.cpp` calls `u->displayLine()` → `role()` resolves at runtime
- **Encapsulation:** private `id_`, `name_`, `dept_` + getters
- **Overloading:** `ActivityManager::searchActivity(string)` / `(EventType)` / `(long day)`
- **RAII:** `ActivityManager()` calls `list_create()`, `~ActivityManager()` calls `list_free()`
- **Virtual destructor:** `virtual ~User()` so `delete static_cast<User*>(...)` is safe

### 3.5 Memory discipline
"Strict rule: malloc'd in C → freed by the C `*_free()`; new'd in C++ → `delete` in C++.
Never cross. `make asan` runs tests under AddressSanitizer/LeakSanitizer — 0 errors, 0 leaks."
Show `main.cpp` cleanup block (bottom).

### 3.6 Persistence
`persistence.cpp`: `loadAll()` on startup, `saveAll()` on exit; 4 pipe-delimited files.
Malformed lines are skipped and counted, not crashed on. If `baselines.txt` is missing it is
rebuilt from history automatically.

### 3.7 Tests
`tests/` — one suite per structure + `test_score.c`. Each checks normal path **and** edge cases:
pop from empty stack, dequeue from empty queue, missing hash key, duplicate timestamps in binary
search, already-sorted input to quicksort, clipping in the score. `make test` runs all 7 (~1 100 checks).

---

## 4. Expected viva questions → short answers

**Why C for DSA and C++ for the app?** Course requirement + a real industry pattern (C core, C++ layer). It forces us to understand linkage, ownership and memory across a language boundary.

**What does `extern "C"` actually do?** Disables C++ name mangling for those declarations so the symbol `enqueue` matches what gcc emitted. Guarded by `#ifdef __cplusplus` so the same header compiles as C.

**Why not machine learning?** A viva must be able to trace every number. Z-scores + rules are transparent, need no training set, and work per-employee from day 31. Industry UEBA tools (Splunk UBA, Microsoft Purview) use the same baseline→deviation→score→review pipeline, just with ML on top.

**Why z-score?** It normalises each metric by that person's own variability. 5 downloads is alarming for someone who averages 1, normal for someone who averages 6.

**Why clip z to [0, 4] and floor std at 0.5?** Clipping stops one metric from dominating and keeps the score bounded; negative z (less activity than usual) is not suspicious. The floor prevents division by ~0 for very regular users — one extra event would otherwise give z = 100.

**How were weights/thresholds chosen?** Design constants in `score.h`, chosen so that known-bad synthetic behaviour (mass download + USB + after-hours) lands in HIGH and ordinary noise stays LOW. Tunable; in production they would be calibrated on labelled incidents.

**Why hash table for baselines, not the array?** Lookup happens for every scored event; hash gives O(1) average vs O(n) scan. Collisions handled by chaining (test uses capacity 4 with 5 keys to prove it).

**Why a queue for alerts and a stack for undo?** Queue = fairness (oldest alert first). Stack = the most recent decision is the one you want to reverse. Undo uses `enqueue_front` so the alert returns to its original position.

**Quicksort worst case?** O(n²) on adversarial input; we pick the middle element as pivot so sorted/reverse-sorted input (common for timestamps) stays O(n log n). Tested with reverse and already-sorted arrays.

**Binary search precondition?** Array must be sorted by the same key — we quicksort by timestamp immediately before.

**Where does RAII appear?** Managers: resource acquired in constructor (`list_create`), released in destructor (`list_free`). Copy constructor/assignment deleted so two managers can never free the same list.

**How do you prove there are no leaks?** `make asan` — AddressSanitizer + LeakSanitizer over all tests; the full menu workflow was also run under ASan with zero reports.

**What is out of scope?** Real log ingestion, networking, SIEM integration, multi-user. Data is synthetic (`data/gen_data.py`), as stated in the proposal's assumptions.

**What is the main limitation?** Baseline needs ~30 days of history; a user who is malicious from day 1 has a "normal" that includes bad behaviour. Mitigation: rules still fire regardless of baseline.

**Complexity of scoring all employees?** `dayStats` scans the list once per employee: O(E·N). Fine for a console prototype; a per-day index would make it O(N).

---

## 5. Common mistakes to avoid
- Running the binary from the wrong folder → "cannot open employees.txt". Always run from project root.
- Forgetting to reset `data/alerts.txt` → option 6 says "0 new alerts" because E104 is already queued.
- Typing `e104` — IDs are case-sensitive (`E104`).
- Claiming "AI" — it is statistics + rules. Say so proudly; it is the point.
- Update your progress slides: the deck still says 45–50% / "scoring not implemented". It is now 100%
  — Milestones 3, 4 and 5 are done (engine, analyst workflow with undo, tests + sanitizer).
