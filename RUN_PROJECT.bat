@echo off
setlocal
cd /d "%~dp0"
title Insider Threat Detection - Project Runner

echo ==============================================
echo       INSIDER THREAT DETECTION PROJECT
echo ==============================================
echo.
where gcc >nul 2>nul
if errorlevel 1 (
  echo ERROR: GCC compiler nahi mila.
  echo Pehle MSYS2 / MinGW-w64 install karke UCRT64 bin ko PATH me add karein.
  echo Guide: https://www.msys2.org/
  echo.
  pause
  exit /b 1
)
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: G++ compiler nahi mila. MSYS2 UCRT64 compiler setup check karein.
  echo.
  pause
  exit /b 1
)

echo [1/2] Project compile ho raha hai...
call build.bat
if errorlevel 1 (
  echo.
  echo BUILD FAILED. Upar diya error check karein.
  pause
  exit /b 1
)
echo.
echo [2/2] Project start ho raha hai...
echo.
insider_detect.exe

echo.
echo Project band ho gaya.
pause
endlocal
