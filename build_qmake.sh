#!/bin/bash

# Change to the script's directory so that paths are relative to the script
cd "$(dirname "$0")"

echo "Building Arm-Pi Tweaker with qmake..."

# Check if Qt6 development packages are installed
echo "Checking Qt6 development packages..."
if ! dpkg -l | grep -q qt6-base-dev 2>/dev/null; then
    echo "Installing Qt6 development packages..."
    sudo apt update
    sudo apt install -y qt6-base-dev qt6-base-dev-tools qmake6 build-essential
    if [ $? -ne 0 ]; then
        echo "❌ Failed to install Qt6 development packages!"
        exit 1
    fi
fi

# Check for F2FS tools (optional but recommended)
echo "Checking for F2FS tools..."
if ! command -v mkfs.f2fs &> /dev/null; then
    echo "⚠️  F2FS tools not found. Installing f2fs-tools for F2FS partition support..."
    sudo apt install -y f2fs-tools
    if [ $? -ne 0 ]; then
        echo "⚠️  Failed to install F2FS tools. F2FS partition option will not work."
    fi
fi

# Clean previous build
echo "Cleaning previous build..."
make clean 2>/dev/null || true
rm -f Makefile

# Generate Makefile with qmake
echo "Generating Makefile with qmake..."
qmake6 arm-pi-tweaker.pro

if [ $? -ne 0 ]; then
    echo "❌ Failed to generate Makefile with qmake!"
    exit 1
fi

# Build the application
echo "Building application..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "Executable location: $(pwd)/arm-pi-tweaker"
    echo ""
    echo "To run the application:"
    echo "  cd $(pwd)"
    echo "  ./arm-pi-tweaker"
else
    echo "❌ Build failed!"
    exit 1
fi