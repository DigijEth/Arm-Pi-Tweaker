#!/bin/bash

echo "Cleaning Arm-Pi Tweaker build files..."

# Clean CMake build files
if [ -d "build" ]; then
    echo "Removing CMake build directory..."
    rm -rf build/
fi

# Clean qmake build files
if [ -f "Makefile" ]; then
    echo "Cleaning qmake build files..."
    make clean 2>/dev/null || true
    rm -f Makefile
fi

# Clean executable and object files
echo "Removing generated files..."
rm -f arm-pi-tweaker
rm -f *.o
rm -f moc_*.cpp
rm -f moc_*.h
rm -f qrc_*.cpp

# Clean Qt auto-generated files
if [ -d "*_autogen" ]; then
    rm -rf *_autogen/
fi

echo "✅ Build files cleaned successfully!"