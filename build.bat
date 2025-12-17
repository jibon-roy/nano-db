@echo off
REM Build script for nanoDB on Windows
REM This script compiles main.c using MinGW GCC

echo.
echo ================================
echo    nanoDB Build Script
echo ================================
echo.

REM Check if main.c exists
if not exist "main.c" (
    echo Error: main.c not found in current directory!
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

REM Kill any running instances of main.exe
echo Cleaning up previous instances...
taskkill /F /IM main.exe >nul 2>&1

REM Compile
echo.
echo Compiling...
"C:\MinGW\bin\gcc.exe" -Wall -Wextra -g main.c -o main.exe

REM Check if compilation was successful
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ================================
    echo Build successful!
    echo ================================
    echo.
    echo Executable: main.exe
    echo.
    echo To run the application:
    echo   main.exe
    echo.
) else (
    echo.
    echo ================================
    echo Build FAILED!
    echo ================================
    echo.
    echo Please check the errors above.
    echo.
    pause
    exit /b 1
)

pause
