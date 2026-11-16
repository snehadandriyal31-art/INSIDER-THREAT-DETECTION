@echo off
REM run_tests.bat — Windows: build and run all C unit tests (needs MinGW gcc). Run from project root.
setlocal
if not exist tests\bin mkdir tests\bin
for %%f in (src\c\*.c) do (
    gcc -std=c11 -Wall -Wextra -Iinclude -O2 -c %%f -o %%~dpnf.o || exit /b 1
)
for %%t in (test_array test_list test_stack test_queue test_hash test_sortsearch test_score) do (
    gcc -std=c11 -Wall -Wextra -Iinclude -Itests -O2 -o tests\bin\%%t.exe tests\%%t.c src\c\*.o || exit /b 1
    tests\bin\%%t.exe || exit /b 1
)
echo ALL TEST SUITES PASSED
endlocal
