#!/bin/bash

# Download real GPU drivers and firmware for Orange Pi 5 Plus
# This script downloads from actual available sources

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Base directories
BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GPU_DIR="${BASE_DIR}/gpu"
FIRMWARE_DIR="${BASE_DIR}/firmware"

echo -e "${GREEN}Orange Pi 5 Plus Driver & Firmware Downloader${NC}"
echo "================================================"

# Function to download with curl (fallback to wget)
download_file() {
    local url=$1
    local dest=$2
    local desc=$3
    
    echo -e "${YELLOW}Downloading ${desc}...${NC}"
    
    # Create destination directory if it doesn't exist
    mkdir -p "$(dirname "$dest")"
    
    if command -v curl &> /dev/null; then
        if curl -L --fail --progress-bar -o "${dest}" "${url}"; then
            echo -e "${GREEN}✓ Downloaded ${desc}${NC}"
            return 0
        fi
    elif command -v wget &> /dev/null; then
        if wget -q --show-progress -O "${dest}" "${url}"; then
            echo -e "${GREEN}✓ Downloaded ${desc}${NC}"
            return 0
        fi
    fi
    
    echo -e "${RED}✗ Failed to download ${desc}${NC}"
    rm -f "${dest}" # Remove partial download
    return 1
}

# Function to clone git repositories
clone_repo() {
    local url=$1
    local dest=$2
    local desc=$3
    
    echo -e "${YELLOW}Cloning ${desc}...${NC}"
    
    if [ -d "${dest}" ]; then
        echo -e "${YELLOW}Repository already exists, updating...${NC}"
        cd "${dest}" && git pull && cd - > /dev/null
    else
        if git clone --depth 1 "${url}" "${dest}"; then
            echo -e "${GREEN}✓ Cloned ${desc}${NC}"
        else
            echo -e "${RED}✗ Failed to clone ${desc}${NC}"
            return 1
        fi
    fi
}

# Create directory structure
mkdir -p "${GPU_DIR}/proprietary/mali-g610"
mkdir -p "${GPU_DIR}/proprietary/libmali"
mkdir -p "${GPU_DIR}/mesa/builds"
mkdir -p "${GPU_DIR}/opensource/panfork"
mkdir -p "${FIRMWARE_DIR}/mali"
mkdir -p "${FIRMWARE_DIR}/rockchip"
mkdir -p "${FIRMWARE_DIR}/linux-firmware"

echo -e "\n${GREEN}=== Downloading ARM Proprietary Drivers ===${NC}"

# Clone Rockchip libmali repository
echo -e "\n${YELLOW}1. Rockchip LibMali Repository${NC}"
clone_repo \
    "https://github.com/JeffyCN/rockchip_mirrors.git" \
    "${GPU_DIR}/proprietary/libmali/rockchip-libmali" \
    "Rockchip LibMali repository"

# JeffyCN's mirror with pre-built libraries
echo -e "\n${YELLOW}2. Pre-built Mali Libraries${NC}"
clone_repo \
    "https://github.com/JeffyCN/mirrors.git" \
    "${GPU_DIR}/proprietary/libmali/jeffycn-mirrors" \
    "JeffyCN Mali mirrors"

# Download specific Mali G610 libraries
echo -e "\n${YELLOW}3. Mali G610 Valhall Libraries${NC}"
MALI_LIBS=(
    "libmali-valhall-g610-g13p0-gbm.so"
    "libmali-valhall-g610-g13p0-wayland-gbm.so"
    "libmali-valhall-g610-g13p0-x11-gbm.so"
    "libmali-valhall-g610-g13p0-x11-wayland-gbm.so"
)

for lib in "${MALI_LIBS[@]}"; do
    download_file \
        "https://github.com/JeffyCN/mirrors/raw/libmali/lib/aarch64-linux-gnu/${lib}" \
        "${GPU_DIR}/proprietary/mali-g610/${lib}" \
        "${lib}"
done

echo -e "\n${GREEN}=== Downloading Mesa Drivers ===${NC}"

# Clone Mesa repository with Panfrost
echo -e "\n${YELLOW}4. Mesa Main Repository${NC}"
clone_repo \
    "https://gitlab.freedesktop.org/mesa/mesa.git" \
    "${GPU_DIR}/mesa/mesa-main" \
    "Mesa main repository"

# Download pre-built Mesa packages from Ubuntu ports
echo -e "\n${YELLOW}5. Pre-built Mesa Packages${NC}"
MESA_PACKAGES=(
    "libgl1-mesa-dri_23.2.1-1ubuntu3_arm64.deb"
    "libegl-mesa0_23.2.1-1ubuntu3_arm64.deb"
    "libgbm1_23.2.1-1ubuntu3_arm64.deb"
    "libglx-mesa0_23.2.1-1ubuntu3_arm64.deb"
)

for pkg in "${MESA_PACKAGES[@]}"; do
    download_file \
        "http://ports.ubuntu.com/pool/main/m/mesa/${pkg}" \
        "${GPU_DIR}/mesa/builds/${pkg}" \
        "Mesa package ${pkg}"
done

echo -e "\n${GREEN}=== Downloading Open Source Drivers ===${NC}"

# Panfork - Community Mali driver with CSF support
echo -e "\n${YELLOW}6. Panfork Mesa Fork${NC}"
clone_repo \
    "https://gitlab.com/panfork/mesa.git" \
    "${GPU_DIR}/opensource/panfork/mesa" \
    "Panfork Mesa (with CSF support)"

# Collabora's Panfrost work
echo -e "\n${YELLOW}7. Collabora Panfrost${NC}"
clone_repo \
    "https://gitlab.collabora.com/bbrezillon/mesa.git" \
    "${GPU_DIR}/opensource/panfork/collabora-mesa" \
    "Collabora Mesa improvements"

echo -e "\n${GREEN}=== Downloading Firmware Files ===${NC}"

# Clone linux-firmware repository
echo -e "\n${YELLOW}8. Linux Firmware Repository${NC}"
clone_repo \
    "https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git" \
    "${FIRMWARE_DIR}/linux-firmware" \
    "Linux firmware repository"

# Clone Rockchip rkbin repository
echo -e "\n${YELLOW}9. Rockchip Binary Repository${NC}"
clone_repo \
    "https://github.com/rockchip-linux/rkbin.git" \
    "${FIRMWARE_DIR}/rockchip/rkbin" \
    "Rockchip binary repository"

# Copy specific firmware files
echo -e "\n${YELLOW}10. Copying Required Firmware${NC}"

# Mali firmware
if [ -f "${FIRMWARE_DIR}/rockchip/rkbin/firmware/mali/mali_csffw.bin" ]; then
    cp "${FIRMWARE_DIR}/rockchip/rkbin/firmware/mali/mali_csffw.bin" \
       "${FIRMWARE_DIR}/mali/mali_csffw.bin"
    echo -e "${GREEN}✓ Copied Mali CSF firmware${NC}"
fi

# RK3588 specific files
RK3588_FILES=(
    "bin/rk35/rk3588_bl31_v1.45.elf"
    "bin/rk35/rk3588_bl32_v1.15.bin"
    "bin/rk35/rk3588_ddr_lp4_2112MHz_lp5_2736MHz_v1.11.bin"
)

for file in "${RK3588_FILES[@]}"; do
    if [ -f "${FIRMWARE_DIR}/rockchip/rkbin/${file}" ]; then
        filename=$(basename "${file}")
        cp "${FIRMWARE_DIR}/rockchip/rkbin/${file}" \
           "${FIRMWARE_DIR}/rockchip/${filename}"
        echo -e "${GREEN}✓ Copied ${filename}${NC}"
    fi
done

# Download Armbian firmware collection
echo -e "\n${YELLOW}11. Armbian Firmware${NC}"
clone_repo \
    "https://github.com/armbian/firmware.git" \
    "${FIRMWARE_DIR}/armbian-firmware" \
    "Armbian firmware collection"

# Create info script
cat > "${GPU_DIR}/driver_info.sh" << 'EOF'
#!/bin/bash

echo "GPU Driver Information"
echo "====================="

# Check for Mali libraries
echo -e "\nMali Proprietary Libraries:"
find proprietary -name "*.so" -type f | while read lib; do
    echo "  - $(basename $lib)"
done

# Check Mesa status
echo -e "\nMesa Drivers:"
if [ -d "mesa/mesa-main" ]; then
    cd mesa/mesa-main
    latest_tag=$(git describe --tags --abbrev=0 2>/dev/null || echo "no tags")
    echo "  - Mesa version: $latest_tag"
    cd - > /dev/null
fi

# Check Panfork status
echo -e "\nPanfork Status:"
if [ -d "opensource/panfork/mesa" ]; then
    cd opensource/panfork/mesa
    branch=$(git branch --show-current)
    echo "  - Panfork branch: $branch"
    cd - > /dev/null
fi
EOF

chmod +x "${GPU_DIR}/driver_info.sh"

# Create installation helper script
cat > "${GPU_DIR}/install_mali.sh" << 'EOF'
#!/bin/bash

# Mali driver installation helper
# Usage: ./install_mali.sh [wayland|x11|gbm]

BACKEND=${1:-gbm}
MALI_DIR="/usr/lib/aarch64-linux-gnu/mali"

echo "Installing Mali drivers for ${BACKEND} backend..."

# Create mali directory
sudo mkdir -p ${MALI_DIR}

# Find appropriate library
case ${BACKEND} in
    wayland)
        LIB=$(find proprietary -name "*wayland-gbm.so" | head -1)
        ;;
    x11)
        LIB=$(find proprietary -name "*x11-gbm.so" | grep -v wayland | head -1)
        ;;
    gbm)
        LIB=$(find proprietary -name "*-gbm.so" | grep -v wayland | grep -v x11 | head -1)
        ;;
    *)
        echo "Unknown backend: ${BACKEND}"
        exit 1
        ;;
esac

if [ -z "${LIB}" ]; then
    echo "No library found for ${BACKEND} backend"
    exit 1
fi

echo "Installing ${LIB}..."

# Copy library
sudo cp "${LIB}" "${MALI_DIR}/libmali.so"

# Create symlinks
cd ${MALI_DIR}
sudo ln -sf libmali.so libEGL.so.1
sudo ln -sf libmali.so libGLESv2.so.2
sudo ln -sf libmali.so libgbm.so.1

# Update library cache
sudo ldconfig

echo "Mali drivers installed successfully!"
EOF

chmod +x "${GPU_DIR}/install_mali.sh"

echo -e "\n${GREEN}=== Creating Documentation ===${NC}"

# Create comprehensive README
cat > "${BASE_DIR}/DRIVERS_AND_FIRMWARE.md" << 'EOF'
# Orange Pi 5 Plus Drivers and Firmware

## GPU Drivers

### 1. ARM Proprietary Mali Drivers
Located in `gpu/proprietary/`

- **Mali Valhall G610**: Official ARM GPU drivers
- **Performance**: Best performance for 3D applications
- **Compatibility**: Works with specific kernel versions
- **Installation**: Use `gpu/install_mali.sh` script

### 2. Mesa Panfrost Drivers
Located in `gpu/mesa/`

- **Open Source**: Fully open source implementation
- **Compatibility**: Works with mainline kernels
- **Features**: OpenGL ES 3.1, Vulkan (experimental)
- **Installation**: Install mesa packages or build from source

### 3. Panfork Drivers
Located in `gpu/opensource/panfork/`

- **Community Fork**: Enhanced Panfrost with CSF support
- **Features**: Better RK3588 support
- **Status**: Experimental but actively developed

## Firmware Files

### Mali GPU Firmware
- `mali_csffw.bin`: Command Stream Frontend firmware
- Required for Mali G610 GPU operation

### Rockchip Platform Firmware
- `rk3588_bl31.elf`: ARM Trusted Firmware
- `rk3588_bl32.bin`: Secure OS
- `rk3588_ddr*.bin`: DDR initialization

### Wireless Firmware
- Broadcom BCM43455 (WiFi/BT)
- Realtek RTL8822CS (alternative)

## Installation Guide

### GPU Driver Installation

1. **Proprietary Mali**:
   ```bash
   cd gpu
   ./install_mali.sh wayland  # or x11, gbm
   ```

2. **Mesa Panfrost**:
   ```bash
   sudo apt install libgl1-mesa-dri
   ```

3. **Build from Source**:
   ```bash
   cd gpu/mesa/mesa-main
   meson build -Dgallium-drivers=panfrost -Dvulkan-drivers=panfrost
   ninja -C build
   sudo ninja -C build install
   ```

### Firmware Installation

```bash
# Copy firmware to system
sudo cp -r firmware/mali/* /lib/firmware/
sudo cp -r firmware/rockchip/*.bin /lib/firmware/rockchip/
sudo update-initramfs -u
```

## Driver Selection Guide

- **Gaming**: Use proprietary Mali drivers
- **Desktop**: Use Mesa Panfrost (better compatibility)
- **Development**: Use Panfork (latest features)

## Troubleshooting

1. Check driver status:
   ```bash
   glxinfo | grep "OpenGL renderer"
   ```

2. Verify firmware loading:
   ```bash
   dmesg | grep -i mali
   ```

3. Test performance:
   ```bash
   glmark2-es2
   ```
EOF

echo -e "\n${GREEN}=== Download Complete ===${NC}"
echo -e "Drivers downloaded to: ${GPU_DIR}"
echo -e "Firmware downloaded to: ${FIRMWARE_DIR}"
echo -e "\nRun ${GREEN}cd gpu && ./driver_info.sh${NC} to see driver information"
echo -e "Run ${GREEN}cd gpu && ./install_mali.sh${NC} to install Mali drivers"
echo -e "\nRefer to ${YELLOW}DRIVERS_AND_FIRMWARE.md${NC} for detailed documentation"