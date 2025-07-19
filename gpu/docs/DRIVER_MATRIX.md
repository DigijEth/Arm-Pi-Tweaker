# Orange Pi 5 Plus GPU Driver Compatibility Matrix

## Hardware Specifications
- **SoC**: RK3588S (ARM Cortex-A76 + A55)
- **GPU**: Mali-G610 MP4 (4 cores, Valhall architecture)
- **Memory**: Up to 32GB LPDDR5-5500
- **Display**: Dual HDMI 2.1 (8K@60Hz), USB-C DP (8K@30Hz)

## Driver Categories

### 1. Proprietary Mali Drivers (Recommended for Performance)

| Driver Package | Version | Architecture | API Support | Use Case |
|---------------|---------|--------------|-------------|----------|
| libmali-valhall-g610-g13p0-wayland-gbm | 1.9-1 | ARM64 | OpenGL ES 3.2, EGL 1.5, Vulkan 1.2 | GameScope, High Performance |
| libmali-valhall-g610-g6p0-wayland-gbm | 1.9-1 | ARM64 | OpenGL ES 3.2, EGL 1.5, Vulkan 1.2 | Compatibility, Older Software |
| libmali-valhall-g610-g13p0-x11-gbm | 1.9-1 | ARM64 | OpenGL ES 3.2, EGL 1.5 | X11 Desktop |
| libmali-valhall-g610-g13p0-only-gbm | 1.9-1 | ARM64 | OpenGL ES 3.2, EGL 1.5 | Minimal Installation |

### 2. Open Source Mesa Drivers

| Driver | Kernel Support | API Support | Performance | Stability |
|--------|---------------|-------------|-------------|-----------|
| Panfrost | 5.0+ | OpenGL ES 3.1, EGL 1.5 | Good | Excellent |
| Panthor | 6.2+ | OpenGL ES 3.2, EGL 1.5, Vulkan 1.1 | Very Good | Good |

### 3. Firmware Requirements

| Component | File | Location | Required For |
|-----------|------|----------|--------------|
| Mali CSF Firmware | mali_csffw.bin | /lib/firmware/ | Proprietary drivers |
| GPU Firmware | Various | /lib/firmware/arm/mali/ | Open source drivers |

## Build Configuration Matrix

### GameScope-Pi v1 (Gaming Focus)
- **Primary Driver**: libmali-valhall-g610-g13p0-wayland-gbm ✅
- **Display Backend**: Wayland + GBM
- **Vulkan Support**: Full (1.2)
- **Performance**: Excellent (95%+ hardware capability)
- **Compatibility**: Good (proprietary limitations)

### OpenScope-Pi v1 (Open Source Focus)
- **Primary Driver**: Mesa Panthor (kernel 6.2+)
- **Fallback Driver**: Mesa Panfrost (kernel 5.0+)
- **Display Backend**: Wayland + GBM
- **Vulkan Support**: Limited (1.1, Panthor only)
- **Performance**: Good (80-90% hardware capability)
- **Compatibility**: Excellent (fully open source)

### Kodi Media Center
- **Primary Driver**: libmali-valhall-g610-g13p0-wayland-gbm ✅
- **Display Backend**: GBM (direct to framebuffer)
- **Hardware Decode**: Rockchip MPP
- **HDR Support**: HDR10 (not HDR10+/Dolby Vision)
- **Audio**: AC3/DTS passthrough (no lossless)

## Performance Benchmarks

### 3D Graphics (GLmark2-ES2)
| Driver | Score | Notes |
|--------|-------|-------|
| Mali g13p0 | 1200-1400 | Full hardware acceleration |
| Mali g6p0 | 1100-1300 | Slightly lower but more stable |
| Panfrost | 800-1000 | Good open source performance |
| Panthor | 900-1200 | Better than Panfrost, improving |

### Video Decode Performance
| Codec | Mali (HW) | Mesa (SW) | Notes |
|-------|-----------|-----------|-------|
| H.264 4K | 60fps | 15-20fps | Hardware decode essential |
| H.265 4K | 60fps | 8-12fps | HEVC requires hardware |
| VP9 4K | 60fps | 10-15fps | Hardware decode recommended |
| AV1 4K | 30fps | 2-5fps | Limited hardware support |

## Kernel Compatibility

### BSP Kernels (Recommended)
- **5.10.160 (Rockchip BSP)**: Full hardware support, all drivers
- **5.10.x (Armbian)**: Optimized for ARM SBCs, stable

### Mainline Kernels
- **6.2+**: Panthor driver support, improved hardware support
- **6.6+**: Latest features, active development
- **6.1 LTS**: Long-term support, good stability

## Installation Priority

### For Maximum Performance (Gaming/Media)
1. **libmali-valhall-g610-g13p0-wayland-gbm** ✅ (Primary)
2. **libmali-valhall-g610-g6p0-wayland-gbm** ✅ (Fallback)
3. Mali CSF firmware (if available)
4. Rockchip MPP for video decode

### For Open Source Compatibility
1. **Mesa Panthor** (kernel 6.2+)
2. **Mesa Panfrost** (kernel 5.0+)
3. linux-firmware package
4. Standard Mesa stack

### For Development/Testing
1. Both proprietary and open source drivers
2. Multiple kernel versions
3. Comprehensive testing tools
4. Debug symbols and headers

## Known Issues and Limitations

### Proprietary Drivers
- ❌ Source code not available
- ❌ Limited to specific kernel versions
- ❌ May have licensing restrictions
- ❌ Debugging capabilities limited

### Open Source Drivers
- ❌ Performance gap vs proprietary
- ❌ Vulkan support incomplete
- ❌ Some advanced features missing
- ❌ Power management not optimal

### Video Decode
- ❌ HDR10+ and Dolby Vision not supported
- ❌ Lossless audio (TrueHD/DTS-HD) limited
- ❌ Some codecs require software fallback
- ❌ Hardware encode support limited

## Recommended Configurations

### High Performance Gaming
```
Driver: libmali-valhall-g610-g13p0-wayland-gbm
Kernel: 5.10.160 BSP
Display: Wayland + GBM
Governor: performance
CMA: 512MB
```

### Balanced Performance/Compatibility
```
Driver: Mesa Panthor
Kernel: 6.6 mainline
Display: Wayland + GBM
Governor: ondemand
CMA: 256MB
```

### Media Center Optimized
```
Driver: libmali-valhall-g610-g13p0-wayland-gbm
Kernel: 5.10.160 BSP
Display: GBM direct
Hardware Decode: Rockchip MPP
CMA: 512MB
```

## Testing and Verification

### Essential Test Commands
```bash
# Check driver loading
lsmod | grep -E "mali|panfrost|panthor"

# Test OpenGL ES
es2_info
glmark2-es2-wayland

# Test Vulkan
vulkaninfo --summary
vkcube-wayland

# Test video decode
ffmpeg -hwaccel rkmpp -i test.mp4 -f null -
```

### Performance Monitoring
```bash
# GPU frequency
cat /sys/class/devfreq/fb000000.gpu/cur_freq

# Memory usage
cat /proc/meminfo | grep -E "CmaFree|CmaTotal"

# Thermal monitoring
cat /sys/class/thermal/thermal_zone*/temp
```

This matrix provides comprehensive guidance for selecting and configuring GPU drivers for the Orange Pi 5 Plus based on specific use cases and requirements.