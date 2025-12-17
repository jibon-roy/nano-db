#!/bin/bash
# Run script for nanoDB

echo ""
echo "================================"
echo "    nanoDB - Running..."
echo "================================"
echo ""

if [ ! -f "main" ]; then
    echo "Error: main executable not found!"
    echo "Please build the project first using: ./build.sh"
    echo ""
    exit 1
fi

./main
