# Orange Pi 5 Plus Device Tree Files

This directory contains Device Tree Source (DTS) files for the Orange Pi 5 Plus board.

## File Structure

- `rk3588s.dtsi` - Base RK3588S SoC device tree include
- `rk3588s-orangepi-5.dtsi` - Orange Pi 5 specific hardware configuration
- `rk3588s-pinctrl.dtsi` - Pin control definitions for RK3588S
- `rk3588s-orangepi-5-plus-*.dts` - Build-specific device tree files

## Build Types

### GameScope-Pi Edition
- `rk3588s-orangepi-5-plus-ubuntu-gamescope.dts`
- Optimized for gaming with GPU overclocking
- AV1 decode support
- High-performance CPU governors
- GameScope compositor configuration

### Kodi Media Center Edition
- `rk3588s-orangepi-5-plus-kodi.dts`
- 4K HDR video support
- CEC enabled for TV remote control
- Hardware video decoding (HEVC, VP9, AV1)
- Optimized for media playback

### Server Edition
- `rk3588s-orangepi-5-plus-server.dts`
- Headless configuration
- Dual Gigabit Ethernet
- Multiple PCIe slots for storage
- Power-efficient settings
- Hardware monitoring support

## Compiling Device Trees

To compile a DTS file to DTB:
```bash
dtc -I dts -O dtb -o output.dtb input.dts
```

Example:
```bash
dtc -I dts -O dtb -o rk3588s-orangepi-5-plus-ubuntu-gamescope.dtb rk3588s-orangepi-5-plus-ubuntu-gamescope.dts
```

## Custom Configurations

The builder will automatically generate custom DTS files based on your selections in the wizard, including:
- Distribution and version
- Kernel version
- GPU driver selection
- Desktop environment
- Special features (AV1, GPU OC, etc.)

## Hardware Features

The Orange Pi 5 Plus includes:
- RK3588S SoC (4x Cortex-A76 + 4x Cortex-A55)
- Mali-G610 MP4 GPU
- Up to 32GB RAM
- eMMC storage
- NVMe M.2 slot
- Dual Gigabit Ethernet
- HDMI 2.1 output
- Multiple USB 3.0/2.0 ports
- 40-pin GPIO header