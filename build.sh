#!/bin/bash
# Build script for nanoDB on Linux
# This script compiles main.c using GCC

echo ""
echo "================================"
echo "    nanoDB Build Script"
echo "================================"
echo ""

# Check if main.c exists
if [ ! -f "main.c" ]; then
    echo "Error: main.c not found in current directory!"
    echo "Please run this script from the project root directory."
    exit 1
fi

# Kill any running instances of main
echo "Cleaning up previous instances..."
pkill -f "./main" 2>/dev/null || true

# Compile
echo ""
echo "Compiling..."
gcc -Wall -Wextra -g main.c -o main

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "================================"
    echo "Build successful!"
    echo "================================"
    echo ""
    echo "Executable: main"
    echo ""
    echo "To run the application:"
    echo "  ./main"
    echo ""
else
    echo ""
    echo "================================"
    echo "Build FAILED!"
    echo "================================"
    echo ""
    echo "Please check the errors above."
    echo ""
    exit 1
fi
