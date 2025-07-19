#!/bin/bash

# Download GPU drivers and firmware for Orange Pi 5 Plus
# This script downloads various GPU drivers and firmware files

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
GPU_DIR="${SCRIPT_DIR}/gpu"
FIRMWARE_DIR="${SCRIPT_DIR}/firmware"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Function to download file with retry
download_file() {
    local url=$1
    local output=$2
    local description=$3
    
    print_status "Downloading ${description}..."
    if wget -q --show-progress -O "${output}" "${url}"; then
        print_status "Successfully downloaded ${description}"
        return 0
    else
        print_error "Failed to download ${description}"
        return 1
    fi
}

# Function to clone git repository
clone_repo() {
    local repo_url=$1
    local target_dir=$2
    local description=$3
    
    print_status "Cloning ${description}..."
    if [ -d "${target_dir}" ]; then
        print_warning "${description} already exists, pulling latest changes..."
        cd "${target_dir}" && git pull
    else
        git clone --depth 1 "${repo_url}" "${target_dir}"
    fi
}

# Create directories if they don't exist
mkdir -p "${GPU_DIR}/proprietary/mali-g610"
mkdir -p "${GPU_DIR}/proprietary/mali-g610/valhall"
mkdir -p "${GPU_DIR}/proprietary/mali-g610/bifrost"
mkdir -p "${GPU_DIR}/mesa/panfrost"
mkdir -p "${GPU_DIR}/mesa/lima"
mkdir -p "${GPU_DIR}/opensource/panfork"
mkdir -p "${FIRMWARE_DIR}/mali"
mkdir -p "${FIRMWARE_DIR}/rockchip"
mkdir -p "${FIRMWARE_DIR}/linux-firmware"

print_status "Starting driver and firmware downloads..."

# Download ARM Mali proprietary drivers
print_status "Downloading ARM Mali proprietary drivers..."

# Mali G610 Valhall DDK drivers
cat > "${GPU_DIR}/proprietary/mali-g610/valhall/README.md" << 'EOF'
# Mali G610 Valhall DDK Drivers

## Driver Information
- GPU: Mali-G610 MP4
- Architecture: Valhall (4th Gen)
- Compatible with: RK3588/RK3588S

## Installation
The Valhall DDK drivers require:
1. Kernel module: mali_kbase.ko
2. User-space libraries: libmali.so

## Versions Available
- DDK r41p0 (latest stable)
- DDK r40p0
- DDK r38p1

## Features
- OpenGL ES 3.2
- Vulkan 1.2
- OpenCL 2.2
- Hardware ray tracing support
EOF

# Download libmali packages from different sources
print_status "Downloading libmali packages..."

# Rockchip official libmali
download_file "https://github.com/JeffyCN/rockchip_mirrors/raw/libmali/lib/aarch64-linux-gnu/libmali-valhall-g610-g13p0-wayland-gbm.so" \
    "${GPU_DIR}/proprietary/mali-g610/valhall/libmali-valhall-g610-g13p0-wayland-gbm.so" \
    "Mali Valhall G610 Wayland/GBM library"

download_file "https://github.com/JeffyCN/rockchip_mirrors/raw/libmali/lib/aarch64-linux-gnu/libmali-valhall-g610-g13p0-x11-gbm.so" \
    "${GPU_DIR}/proprietary/mali-g610/valhall/libmali-valhall-g610-g13p0-x11-gbm.so" \
    "Mali Valhall G610 X11/GBM library"

# Create driver info file
cat > "${GPU_DIR}/proprietary/mali-g610/valhall/driver_info.json" << 'EOF'
{
    "driver_name": "Mali-G610 Valhall DDK",
    "version": "r41p0",
    "architecture": "valhall",
    "features": {
        "opengl_es": "3.2",
        "vulkan": "1.2",
        "opencl": "2.2",
        "compute_units": 4,
        "frequency_mhz": 1000
    },
    "supported_platforms": [
        "wayland",
        "x11",
        "gbm",
        "fbdev"
    ]
}
EOF

# Download Mesa/Panfrost drivers
print_status "Downloading Mesa/Panfrost drivers..."

# Create Mesa build script
cat > "${GPU_DIR}/mesa/build_mesa.sh" << 'EOF'
#!/bin/bash
# Build Mesa with Panfrost driver for Mali G610

MESA_VERSION="24.0.0"
BUILD_DIR="/tmp/mesa-build"

# Install build dependencies
sudo apt-get update
sudo apt-get install -y \
    python3-mako \
    libexpat1-dev \
    libdrm-dev \
    libx11-dev \
    libxext-dev \
    libxdamage-dev \
    libxfixes-dev \
    libxxf86vm-dev \
    libxcb-glx0-dev \
    libxcb-dri2-0-dev \
    libxcb-dri3-dev \
    libxcb-present-dev \
    libxcb-sync-dev \
    libxshmfence-dev \
    libxrandr-dev \
    meson \
    ninja-build

# Clone Mesa repository
git clone --depth 1 --branch mesa-${MESA_VERSION} https://gitlab.freedesktop.org/mesa/mesa.git ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure build with Panfrost
meson setup build \
    -Dgallium-drivers=panfrost \
    -Dvulkan-drivers=panfrost \
    -Dplatforms=x11,wayland \
    -Ddri3=enabled \
    -Degl=enabled \
    -Dgbm=enabled \
    -Dgles1=disabled \
    -Dgles2=enabled \
    -Dopengl=true \
    -Dshared-glapi=enabled \
    -Dllvm=disabled

# Build
ninja -C build

# Install
sudo ninja -C build install
EOF

chmod +x "${GPU_DIR}/mesa/build_mesa.sh"

# Download Panfork (community Mali driver)
print_status "Downloading Panfork drivers..."
clone_repo "https://gitlab.com/panfork/mesa.git" \
    "${GPU_DIR}/opensource/panfork/mesa" \
    "Panfork Mesa"

# Create Panfork info
cat > "${GPU_DIR}/opensource/panfork/README.md" << 'EOF'
# Panfork - Community Mali Driver

Panfork is a community-driven fork of Mesa's Panfrost driver with additional features and optimizations for Mali GPUs.

## Features
- Improved performance for Mali-G610
- Additional OpenGL extensions
- Better Vulkan support
- Gaming optimizations

## Building
See mesa/docs/panfrost.rst for build instructions
EOF

# Download firmware files
print_status "Downloading firmware files..."

# Clone linux-firmware repository (selective download)
print_status "Downloading Rockchip firmware files..."

# Download specific Rockchip firmware files
ROCKCHIP_FW_BASE="https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git/plain/rockchip"

firmware_files=(
    "dptx.bin"
)

for fw in "${firmware_files[@]}"; do
    download_file "${ROCKCHIP_FW_BASE}/${fw}" \
        "${FIRMWARE_DIR}/rockchip/${fw}" \
        "Rockchip ${fw}"
done

# Download Mali firmware
print_status "Creating Mali firmware information..."
cat > "${FIRMWARE_DIR}/mali/mali_firmware_info.txt" << 'EOF'
Mali G610 Firmware Information
==============================

The Mali G610 GPU firmware is typically embedded in the kernel driver.
No separate firmware files are required for basic operation.

For advanced features, the following may be needed:
- CSF (Command Stream Frontend) firmware
- MCU firmware for power management

These are usually provided by the SoC vendor (Rockchip) as part of their BSP.
EOF

# Download additional GPU tools
print_status "Downloading GPU tools and utilities..."

# Create GPU utilities directory
mkdir -p "${GPU_DIR}/tools"

# Create Mali GPU performance monitoring script
cat > "${GPU_DIR}/tools/mali_monitor.sh" << 'EOF'
#!/bin/bash
# Monitor Mali GPU performance

while true; do
    clear
    echo "Mali GPU Monitor - $(date)"
    echo "================================"
    
    # GPU frequency
    if [ -f /sys/class/devfreq/fb000000.gpu/cur_freq ]; then
        echo "GPU Frequency: $(cat /sys/class/devfreq/fb000000.gpu/cur_freq) Hz"
    fi
    
    # GPU load
    if [ -f /sys/class/devfreq/fb000000.gpu/load ]; then
        echo "GPU Load: $(cat /sys/class/devfreq/fb000000.gpu/load)%"
    fi
    
    # GPU temperature
    if [ -f /sys/class/thermal/thermal_zone1/temp ]; then
        temp=$(cat /sys/class/thermal/thermal_zone1/temp)
        echo "GPU Temperature: $((temp/1000))°C"
    fi
    
    # GPU power
    if [ -f /sys/class/devfreq/fb000000.gpu/power/runtime_active_time ]; then
        echo "GPU Active Time: $(cat /sys/class/devfreq/fb000000.gpu/power/runtime_active_time) ms"
    fi
    
    sleep 1
done
EOF

chmod +x "${GPU_DIR}/tools/mali_monitor.sh"

# Create driver selection script
cat > "${GPU_DIR}/select_driver.sh" << 'EOF'
#!/bin/bash
# Select and configure GPU driver for Orange Pi 5 Plus

echo "Orange Pi 5 Plus GPU Driver Selector"
echo "===================================="
echo ""
echo "Available drivers:"
echo "1. Mali Valhall DDK (Proprietary) - Best performance"
echo "2. Panfrost (Mesa) - Open source, good compatibility"
echo "3. Panfork - Community fork with gaming optimizations"
echo ""
read -p "Select driver (1-3): " choice

case $choice in
    1)
        echo "Configuring Mali Valhall DDK..."
        # Link proprietary driver
        sudo ln -sf /usr/lib/aarch64-linux-gnu/libmali-valhall-g610-g13p0-wayland-gbm.so /usr/lib/aarch64-linux-gnu/libmali.so
        echo "Valhall DDK configured"
        ;;
    2)
        echo "Configuring Panfrost driver..."
        # Mesa/Panfrost is usually the default
        echo "Panfrost configured"
        ;;
    3)
        echo "Configuring Panfork driver..."
        echo "Please build and install Panfork from source"
        ;;
    *)
        echo "Invalid selection"
        ;;
esac
EOF

chmod +x "${GPU_DIR}/select_driver.sh"

# Download kernel modules source references
print_status "Creating kernel module build scripts..."

cat > "${GPU_DIR}/proprietary/mali-g610/build_kernel_module.sh" << 'EOF'
#!/bin/bash
# Build Mali kernel module for RK3588

KERNEL_VERSION=$(uname -r)
MALI_VERSION="r41p0"

echo "Building Mali kernel module ${MALI_VERSION} for kernel ${KERNEL_VERSION}"

# The actual kernel module source would need to be obtained from:
# 1. Rockchip BSP kernel sources
# 2. ARM's Mali DDK (requires license)

echo "To build the Mali kernel module:"
echo "1. Obtain Mali DDK sources from ARM (requires license agreement)"
echo "2. Or use Rockchip's BSP kernel which includes the driver"
echo "3. Build with: make -C /lib/modules/${KERNEL_VERSION}/build M=\$PWD"
EOF

chmod +x "${GPU_DIR}/proprietary/mali-g610/build_kernel_module.sh"

# Create firmware download script for linux-firmware
cat > "${FIRMWARE_DIR}/download_linux_firmware.sh" << 'EOF'
#!/bin/bash
# Download full linux-firmware for all devices

echo "Downloading complete linux-firmware package..."
git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git

echo "Installing firmware files..."
sudo mkdir -p /lib/firmware
sudo cp -r linux-firmware/* /lib/firmware/

echo "Firmware installation complete"
EOF

chmod +x "${FIRMWARE_DIR}/download_linux_firmware.sh"

# Create driver compatibility matrix
cat > "${GPU_DIR}/compatibility_matrix.md" << 'EOF'
# GPU Driver Compatibility Matrix

## Orange Pi 5 Plus (RK3588S) - Mali G610 MP4

| Driver | OpenGL ES | Vulkan | OpenCL | Wayland | X11 | Performance | Stability |
|--------|-----------|---------|---------|----------|------|-------------|-----------|
| Valhall DDK | 3.2 | 1.2 | 2.2 | ✓ | ✓ | Excellent | Excellent |
| Panfrost | 3.1 | 1.0 | ✗ | ✓ | ✓ | Good | Good |
| Panfork | 3.1 | 1.1 | ✗ | ✓ | ✓ | Very Good | Good |
| Bifrost Legacy | 3.2 | ✗ | 2.0 | ✓ | ✓ | Good | Fair |

## Kernel Requirements

- Valhall DDK: Kernel 5.10+ with Rockchip patches
- Panfrost: Mainline kernel 5.2+
- Panfork: Kernel 5.10+ recommended

## Use Cases

### Gaming
- Recommended: Valhall DDK or Panfork
- Panfork offers better Linux gaming optimizations

### Desktop Use
- Recommended: Panfrost (included in Mesa)
- Best open-source support

### Compute Workloads
- Recommended: Valhall DDK
- Only driver with OpenCL support

### Media Playback
- All drivers support hardware video decode
- Valhall DDK has best codec support
EOF

# Create summary
print_status "Creating download summary..."
cat > "${GPU_DIR}/DOWNLOADS_SUMMARY.md" << 'EOF'
# GPU Drivers Download Summary

## Downloaded Components

### Proprietary Drivers (ARM Mali)
- Mali G610 Valhall DDK libraries (Wayland/GBM and X11/GBM variants)
- Driver information and build scripts

### Open Source Drivers
- Mesa/Panfrost build scripts
- Panfork repository clone instructions

### Tools and Utilities
- GPU performance monitor
- Driver selection utility
- Kernel module build scripts

### Firmware
- Rockchip firmware files
- Linux firmware download script

## Next Steps

1. **For Proprietary Driver**:
   - Install libmali libraries to system
   - Build kernel module if needed

2. **For Open Source Driver**:
   - Run Mesa build script
   - Or install from distribution packages

3. **For Firmware**:
   - Run firmware download script if additional firmware needed

## Directory Structure
```
gpu/
├── proprietary/
│   └── mali-g610/
│       ├── valhall/
│       └── bifrost/
├── mesa/
│   └── panfrost/
├── opensource/
│   └── panfork/
└── tools/

firmware/
├── mali/
├── rockchip/
└── linux-firmware/
```
EOF

print_status "Driver and firmware download complete!"
print_status "Check ${GPU_DIR}/DOWNLOADS_SUMMARY.md for details"

# Final status
echo ""
echo "Download Summary:"
echo "================="
ls -la "${GPU_DIR}/proprietary/mali-g610/valhall/" | grep -E "\.so$" | wc -l | xargs echo "Proprietary drivers:"
ls -la "${FIRMWARE_DIR}/rockchip/" | grep -E "\.bin$" | wc -l | xargs echo "Firmware files:"
echo ""
print_status "Run individual build scripts to compile drivers from source"