# Orange Pi 5 Plus GPU Drivers Collection

This directory contains GPU drivers and firmware for the Orange Pi 5 Plus (RK3588S) with Mali-G610 MP4 GPU.

## Successfully Downloaded Drivers

### Mali-G610 Valhall Proprietary Drivers
- **libmali-valhall-g610-g13p0-wayland-gbm_1.9-1_arm64.deb** (12.0 MB)
  - Latest stable proprietary Mali driver for Wayland/GBM
  - Recommended for GameScope builds
  - Provides full hardware acceleration
  - Version: 1.9-1 (g13p0 revision)

- **libmali-valhall-g610-g6p0-wayland-gbm_1.9-1_arm64.deb** (11.2 MB)
  - Alternative Mali driver version (g6p0 revision)
  - May have better compatibility with older software
  - Same feature set as g13p0

## Driver Installation Instructions

### GameScope Build (Proprietary Drivers)
```bash
# Install g13p0 version (recommended for stability)
sudo dpkg -i libmali-valhall-g610-g13p0-wayland-gbm_1.9-1_arm64.deb

# Create necessary symlinks
cd /usr/lib/aarch64-linux-gnu/
sudo ln -sf libmali-valhall-g610-g13p0-wayland-gbm.so libmali.so.1
sudo ln -sf libmali.so.1 libEGL.so.1
sudo ln -sf libmali.so.1 libEGL.so
sudo ln -sf libmali.so.1 libGLESv2.so.2
sudo ln -sf libmali.so.1 libGLESv2.so
sudo ln -sf libmali.so.1 libgbm.so.1
sudo ln -sf libmali.so.1 libgbm.so
```

### Alternative Installation (g6p0 version)
```bash
# Install g6p0 version if g13p0 has issues
sudo dpkg -i libmali-valhall-g610-g6p0-wayland-gbm_1.9-1_arm64.deb

# Create symlinks for g6p0
cd /usr/lib/aarch64-linux-gnu/
sudo ln -sf libmali-valhall-g610-g6p0-wayland-gbm.so libmali.so.1
# ... (same symlinks as above)
```

## Additional Drivers to Download

### Mesa Open Source Drivers (for OpenScope builds)
The following Mesa packages should be downloaded from Ubuntu repositories:
- mesa-vulkan-drivers
- mesa-va-drivers
- mesa-vdpau-drivers
- mesa-utils
- libgl1-mesa-dri
- libglx-mesa0
- libgbm1
- libegl-mesa0
- libgles2-mesa
- libvulkan1
- vulkan-tools

### Linux Firmware Package
- firmware-misc-nonfree (contains Mali GPU firmware)
- linux-firmware (comprehensive firmware collection)

### Rockchip Multimedia Stack
- rockchip-multimedia-config
- mali-g610-firmware
- ffmpeg with rockchip patches
- va-driver-all
- libva2
- libvdpau1

## Driver Selection Guide

### For GameScope-Pi Build
- **Primary**: libmali-valhall-g610-g13p0-wayland-gbm (✅ Downloaded)
- **Alternative**: libmali-valhall-g610-g6p0-wayland-gbm (✅ Downloaded)
- **Firmware**: Mali CSF firmware (⚠️ Need to locate source)

### For OpenScope-Pi Build (Open Source)
- **Primary**: Mesa Panfrost driver (📦 Available in distro repos)
- **Alternative**: Mesa Panthor driver (kernel 6.2+)
- **Firmware**: Usually included in linux-firmware package

### For Kodi Media Center Build
- **Primary**: libmali-valhall-g610-g13p0-wayland-gbm (✅ Downloaded)
- **Multimedia**: Rockchip MPP integration
- **Hardware Acceleration**: VA-API and VDPAU support

## Performance Comparison

| Driver Type | 3D Performance | Video Decode | Compatibility | Power Usage |
|-------------|----------------|--------------|---------------|-------------|
| Mali g13p0  | Excellent      | Excellent    | Good          | Low         |
| Mali g6p0   | Excellent      | Excellent    | Better        | Low         |
| Mesa Panfrost | Good         | Good         | Excellent     | Medium      |
| Mesa Panthor  | Very Good    | Very Good    | Very Good     | Medium      |

## Troubleshooting

### Common Issues
1. **Driver conflicts**: Ensure only one Mali driver is installed
2. **Missing firmware**: Check `/lib/firmware/` for mali_csffw.bin
3. **Wrong symlinks**: Verify symlinks point to correct driver version
4. **Permissions**: Ensure video group has proper access to /dev/mali*

### Verification Commands
```bash
# Check driver loading
lsmod | grep mali
dmesg | grep -i mali

# Test OpenGL ES
es2_info
glmark2-es2-wayland

# Test Vulkan
vulkaninfo | grep -A5 "GPU id"
vkcube-wayland
```

## Source Information

### Downloaded Files Sources
- **Mali Drivers**: https://github.com/tsukumijima/libmali-rockchip
- **Firmware**: Various Linux kernel firmware repositories
- **Mesa**: Ubuntu/Debian official repositories

### Referenced Build Guides
- GameScope_BuildGuide.txt
- emulation_opensource_drivers.md
- Kodi optimized.md

## Notes

- All drivers are ARM64 architecture specific
- Tested with Orange Pi 5 Plus (RK3588S) only
- Requires proper kernel support (5.10+ BSP or 6.2+ mainline)
- Some drivers may require specific kernel modules