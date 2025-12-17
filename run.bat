@echo off
REM Run script for nanoDB

echo.
echo ================================
echo    nanoDB - Running...
echo ================================
echo.

if not exist "main.exe" (
    echo Error: main.exe not found!
    echo Please build the project first using: build.bat
    echo.
    pause
    exit /b 1
)

main.exe

pause
