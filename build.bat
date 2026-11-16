@echo off
REM build.bat — Windows build without make (needs MinGW-w64 gcc/g++ on PATH).
REM Run from the project root:  build.bat        then:  insider_detect.exe
setlocal
echo [1/3] compiling C DSA layer (gcc, C11)...
for %%f in (src\c\*.c) do (
    gcc -std=c11 -Wall -Wextra -Iinclude -O2 -c %%f -o %%~dpnf.o || exit /b 1
)
echo [2/3] compiling C++ OOP layer (g++, C++17)...
for %%f in (src\cpp\*.cpp) do (
    g++ -std=c++17 -Wall -Wextra -Iinclude -Isrc\cpp -O2 -c %%f -o %%~dpnf.o || exit /b 1
)
echo [3/3] linking...
g++ -o insider_detect.exe src\c\*.o src\cpp\*.o || exit /b 1
echo.
echo Build OK -> insider_detect.exe
echo To run the unit tests:  tests\run_tests.bat
endlocal
