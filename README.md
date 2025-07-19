








![Project Logo](https://ibb.co/RG431NVk)



























Note:LCCM / LCCE is the GUI version of the Arm-Pi Tweaker, it is not safe for public release yet. It is more likely to damage your system so it is not included. I have noticed that some of the tools are giving the wrong information, I have to investigate this, im guessing claude opus was daydreaming again while helping me debug errors. I dont know what happened, but not only has opus become dumber than usual over the last week, Anthropic nerfed our enterprise accounts so updates and fixes may be slower than usual as I am the only human on this entire project.

# Orange-Pi Builder

## Warning

### Experimental Software
This software is experimental and provided "as is." It may cause unexpected behavior or damage to your device. Use it at your own risk.

### Disclaimer
I am not responsible for any issues, including hardware damage, data loss, or other problems caused by using this software. Ensure you understand the risks before proceeding.

## Overview
Orange-Pi Builder is a comprehensive toolset designed to simplify the process of building, configuring, and deploying software for Orange Pi devices. It provides a streamlined workflow for developers and enthusiasts working with Orange Pi hardware, offering support for device-specific drivers, firmware, and bootloader configurations.

### Supported Distros
This builder creates two specialized Orange Pi 5 Plus distros:

1. **GameScope Desktop Distro**
   - Debian 12.11 base system
   - LXQt desktop environment
   - Wayland-only (no X11)
   - GameScope as the compositor
   - Full desktop experience with gaming optimization

2. **GameScope RetroArch Distro**
   - Debian 12.11 minimal base
   - Wayland-only (no X11)
   - No desktop environment
   - GameScope auto-launches RetroArch on boot
   - Dedicated gaming/emulation system

## Features
- **Device-Specific Drivers**: Includes open-source and proprietary drivers for GPU and other hardware components.
- **Firmware Management**: Tools for downloading, updating, and managing firmware.
- **Bootloader Configuration**: Support for U-Boot and other bootloader setups.
- **Burner and Flasher Utilities**: Simplifies the process of burning and flashing images to devices.
- **UI Components**: Customizable themes, error dialogs, and status bars.
- **Updates and Recovery**: Mechanisms for handling updates and recovering from errors.
- **Wizard Interface**: Step-by-step guidance for new users.
- **Arm-Pi Tweaker**: Complete Orange Pi 5 Plus system configuration and performance optimization.

## Arm-Pi Tweaker Module

The Arm-Pi Tweaker is a comprehensive system configuration and performance optimization tool specifically designed for the Orange Pi 5 Plus. It combines all the functionality from both the original Armbian Config and the newer Armbian configng tools, providing a complete solution for system management and optimization.

### Performance & Hardware Features

#### GPU Driver Management
- **Driver Installation**: Support for Mali proprietary, Mesa/Panfrost open-source, and experimental drivers
- **Driver Switching**: Easy switching between different GPU driver configurations
- **Performance Testing**: Built-in GPU performance benchmarks and tests
- **Backup & Recovery**: Automatic backup of previous drivers before installation

#### Hardware Acceleration
- **Video Acceleration**: H.264, H.265, VP8, VP9 hardware decode/encode support
- **Audio Acceleration**: Hardware audio processing optimization
- **Graphics Acceleration**: OpenGL ES, EGL, and Vulkan optimization
- **AI/ML Acceleration**: NPU (Neural Processing Unit) configuration and testing
- **Network Acceleration**: Hardware network offloading features

#### CPU Scheduler & Overclocking
- **Governor Configuration**: Performance, powersave, ondemand, conservative, schedutil governors
- **Frequency Scaling**: Manual CPU frequency control with safety limits
- **Core Management**: Individual core control and big.LITTLE optimization
- **Thermal Management**: Temperature monitoring and thermal throttling configuration
- **Performance Profiles**: Pre-configured profiles for gaming, productivity, and power saving

#### MPP & Performance Tweaks
- **Media Processing Platform**: Rockchip MPP framework configuration
- **Video Codec Optimization**: Hardware codec acceleration setup
- **Memory Optimization**: Memory bandwidth and latency improvements
- **I/O Performance**: Storage and network I/O optimizations
- **System Tweaks**: Kernel parameters and system-level performance enhancements

### System Management Features (Armbian configng)

#### Kernel Management
- **Kernel Installation**: Support for stable, edge, development, and vendor kernels
- **Kernel Switching**: Easy kernel version management and rollback
- **Module Management**: Kernel module loading/unloading and configuration
- **Parameter Configuration**: Kernel boot parameter editing
- **Custom Compilation**: Build kernels from source with custom patches

#### SSH Server Configuration
- **Service Management**: Enable/disable/restart SSH service
- **Security Configuration**: Key-only authentication, Fail2Ban integration
- **Access Control**: User-specific SSH access and port configuration
- **Key Management**: SSH key generation and authorized key management

#### System Services
- **Service Control**: systemd service management and monitoring
- **Boot Services**: Configure services that start at boot
- **Gaming Services**: Gaming-specific service optimization
- **AI/ML Services**: Machine learning framework services
- **Monitoring Services**: System monitoring and logging services

#### Network Configuration
- **Wi-Fi Management**: Wireless network configuration and optimization
- **Ethernet Settings**: Wired network configuration and bonding
- **Bluetooth Configuration**: Bluetooth device management
- **VPN Setup**: OpenVPN and WireGuard configuration
- **Network Optimization**: Bandwidth and latency optimization

#### Software Installation
- **Media Servers**: Plex, Jellyfin, Emby installation and configuration
- **Development Tools**: Programming language runtimes and IDEs
- **Gaming Software**: Steam, RetroArch, emulators
- **AI/ML Frameworks**: TensorFlow, PyTorch, ONNX runtime
- **Productivity Tools**: Office suites, media editors, utilities

### Configuration & Info Features

#### Storage Management
- **Disk Management**: Partition management and filesystem operations
- **RAID Configuration**: Software RAID setup and monitoring
- **LVM Management**: Logical volume management
- **NVMe Optimization**: NVMe SSD performance tuning
- **SD Card Management**: SD card optimization and health monitoring

#### Localization
- **Timezone Configuration**: Time zone selection and NTP setup
- **Locale Settings**: Language and regional settings
- **Keyboard Layout**: Keyboard mapping configuration
- **Character Encoding**: UTF-8 and regional encoding support

#### Security Configuration
- **Firewall Management**: UFW and iptables configuration
- **System Hardening**: Security best practices implementation
- **User Management**: User account creation and privilege management
- **Encryption Settings**: Disk encryption and secure boot configuration
- **Intrusion Detection**: Security monitoring and alerting

#### System Information
- **Hardware Information**: Detailed CPU, GPU, memory, and storage information
- **Performance Monitoring**: Real-time system performance metrics
- **Temperature Monitoring**: CPU, GPU, and board temperature tracking
- **Network Information**: Interface configuration and statistics
- **Driver Status**: Current driver versions and compatibility information

### Advanced Settings

#### Boot Configuration
- **U-Boot Parameters**: Bootloader configuration and optimization
- **Kernel Parameters**: Advanced kernel command line configuration
- **Device Tree Overlays**: Hardware interface enable/disable
- **Boot Environment**: Boot priority and recovery options

#### Thermal Management
- **Thermal Policies**: Conservative, balanced, performance, and custom policies
- **Cooling Configuration**: Fan control and thermal throttling
- **Temperature Limits**: Custom temperature thresholds
- **Performance vs Thermal Balance**: Automatic thermal-performance optimization

#### Power Management
- **Power Profiles**: Battery and AC power optimization
- **Sleep/Wake Configuration**: Suspend and resume settings
- **Power Consumption Monitoring**: Real-time power usage tracking
- **Energy Saving**: Automatic power optimization features

### Usage

Access the Arm-Pi Tweaker through the main menu after launching the Orange Pi Builder application. The interface is organized into logical sections:

1. **Performance & Hardware**: GPU drivers, hardware acceleration, CPU tuning, MPP optimization
2. **System Management**: Kernel, SSH, services, updates, boot environment
3. **Configuration & Info**: Localization, security, storage, system information

Each section provides both basic and advanced configuration options, with built-in safety checks and backup mechanisms to prevent system damage.

### Safety Features

- **Automatic Backups**: Critical system files are backed up before changes
- **Temperature Monitoring**: Overheating prevention during stress tests
- **Rollback Capability**: Easy restoration of previous configurations
- **Validation Checks**: Input validation and compatibility verification
- **Safe Defaults**: Conservative default settings for new installations

## Arm-Pi Tweaker: Live Custom Creation Edition (LCCE)

The **Live Custom Creation Edition (LCCE)** is a standalone GUI application built with Slint that allows users to install their current SD card system to NVMe/eMMC storage with custom modifications. Unlike the terminal-based Arm-Pi Tweaker, LCCE provides a modern graphical interface for live system customization and installation.

### Overview

LCCE enables users to:
- Install the currently running SD card system to internal storage (NVMe/eMMC)
- Apply custom modifications during the installation process
- Create optimized installations with performance enhancements
- Use all Arm-Pi Tweaker functionality through a modern GUI interface

### Core Features

#### Live Installation System
- **Source Detection**: Automatically detects the current SD card system
- **Target Selection**: Choose between NVMe SSD or eMMC storage
- **Live Modification**: Apply customizations during the installation process
- **Progress Monitoring**: Real-time installation progress with detailed feedback
- **Backup Creation**: Optional backup of source system before installation

#### GUI Modules

##### 🐧 Kernel Modifications
- **Kernel Selection**: Choose between Armbian, mainline, vendor, or custom kernels
- **Module Management**: Enable/disable kernel modules with live preview
- **Device Tree Configuration**: Customize device tree overlays and parameters
- **Boot Parameters**: Edit kernel command line parameters
- **Custom Patches**: Apply custom kernel patches during compilation
- **Performance Tuning**: Kernel-level performance optimizations

##### 🎮 Video Driver Management
- **Driver Selection**: Install Mali proprietary, Panfrost open-source, or custom drivers
- **MPP Integration**: Configure Rockchip Media Processing Platform
- **Hardware Acceleration**: Setup video decode/encode acceleration
- **Performance Profiles**: Gaming, productivity, and power-saving GPU profiles
- **Vulkan & OpenCL**: Enable modern graphics and compute APIs
- **Testing Suite**: Built-in GPU performance validation

##### 🎬 Emulation & Multimedia
- **Frontend Installation**: 
  - **Kodi**: Full media center with plugins and addons
  - **RetroPie**: Complete retro gaming platform
  - **EmulationStation**: Standalone emulation frontend
  - **Custom Frontends**: Support for custom applications
- **Auto-Start Configuration**: Boot directly into chosen frontend
- **Media Center Setup**: Jellyfin, Plex, or custom media server installation
- **Game Configuration**: Pre-configured emulators and game settings
- **Performance Optimization**: Emulation-specific system tuning

##### 💾 Storage Installation
- **Target Preparation**: Automatic partitioning and filesystem creation
- **Installation Modes**:
  - **Clone Mode**: Direct copy of current system
  - **Fresh Mode**: Clean installation with configurations
  - **Hybrid Mode**: Clone with live modifications
- **Partition Schemes**: Simple, advanced, or custom partitioning
- **Encryption Support**: Optional full-disk encryption
- **Compression**: Reduce installation size with filesystem compression

##### 🔧 Arm-Pi Tweaker Integration
- Complete integration of all Arm-Pi Tweaker functionality
- Performance optimization and system tuning
- Network configuration and security hardening
- Service management and monitoring
- All features accessible through modern GUI interface

### Technical Architecture

#### GUI Framework
- **Slint**: Modern, efficient GUI framework for Rust
- **Cross-Platform**: Native performance on Linux desktop environments
- **Responsive Design**: Adaptive layout for different screen sizes
- **Professional Styling**: Modern flat design with intuitive navigation

#### System Integration
- **Hardware Detection**: Automatic Orange Pi 5 Plus hardware identification
- **Real-Time Monitoring**: Live system information and performance metrics
- **Background Operations**: Async operations with progress feedback
- **Error Handling**: Comprehensive error recovery and user feedback

#### Configuration Management
- **JSON Configuration**: Human-readable configuration files
- **Profile System**: Save and load custom configuration profiles
- **Validation**: Automatic configuration validation and conflict detection
- **Backup**: Configuration backup and restore functionality

### Installation & Usage

#### Prerequisites
- Orange Pi 5 Plus with desktop environment
- Current system running from SD card
- Target storage: NVMe SSD or eMMC (internal storage)
- At least 4GB RAM recommended for installation process

#### Running LCCE
```bash
# Clone the repository
git clone <repository-url>
cd lcce

# Build the application
cargo build --release

# Run LCCE
cargo run --release
```

#### Typical Workflow
1. **System Detection**: LCCE automatically detects your Orange Pi 5 Plus and current system
2. **Module Configuration**: Configure kernel, video drivers, and multimedia options
3. **Target Selection**: Choose NVMe or eMMC as installation target
4. **Installation Review**: Review all configurations before installation
5. **Live Installation**: Monitor progress as system is installed with modifications
6. **Verification**: Automatic verification of installed system
7. **Reboot**: Reboot into newly installed and customized system

### Safety Features

#### Pre-Installation Checks
- **Hardware Compatibility**: Verify Orange Pi 5 Plus compatibility
- **Storage Validation**: Check target storage health and capacity
- **Configuration Validation**: Verify all settings are compatible
- **Backup Verification**: Ensure source system backup is possible

#### Installation Safety
- **Progress Monitoring**: Real-time installation progress with error detection
- **Rollback Capability**: Automatic rollback on critical errors
- **Checksum Verification**: Verify data integrity during transfer
- **Emergency Stop**: Ability to safely abort installation if needed

#### Post-Installation
- **Boot Verification**: Ensure new system boots correctly
- **Hardware Testing**: Verify all hardware functions properly
- **Performance Validation**: Confirm performance optimizations are active
- **Recovery Options**: Multiple recovery methods if issues arise

### File Structure
```
/lcce/
├── Cargo.toml              # Project dependencies and configuration
├── src/
│   ├── main.rs             # Main application entry point
│   ├── system.rs           # Hardware detection and system info
│   ├── config.rs           # Configuration management
│   └── modules/            # Individual GUI modules
│       ├── kernel.rs       # Kernel modification interface
│       ├── video_driver.rs # Video driver configuration
│       ├── emulation.rs    # Multimedia and emulation setup
│       ├── storage.rs      # Storage installation logic
│       └── armpi_tweaker.rs# Arm-Pi Tweaker integration
├── ui/
│   └── main.slint         # Main GUI layout and components
└── assets/                # Icons, images, and resources
```

### Development Status

#### ✅ Completed
- Project structure and build system
- Core system detection capabilities
- Configuration management framework
- Basic GUI layout and navigation
- Module framework and integration points

#### 🚧 In Development
- Individual module GUI implementations
- Storage installation logic
- Real-time progress monitoring
- Configuration profile system

#### 📋 Planned
- Advanced customization options
- Network-based remote installation
- Automated testing and validation
- Plugin system for custom modules

### Comparison: LCCE vs Arm-Pi Tweaker

| Feature | Arm-Pi Tweaker | LCCE |
|---------|----------------|------|
| **Interface** | Terminal-based (Cursive TUI) | Modern GUI (Slint) |
| **Target Use** | Live system configuration | System installation + config |
| **Installation** | Modifies existing system | Installs new system to storage |
| **Platform** | SSH/Terminal compatible | Desktop environment required |
| **Workflow** | Immediate changes | Plan → Install → Boot |
| **Backup** | Manual backup recommended | Automatic backup integration |
| **Customization** | Real-time system changes | Installation-time modifications |

Both tools complement each other:
- **Arm-Pi Tweaker**: Perfect for tweaking and optimizing existing installations
- **LCCE**: Ideal for creating new, customized installations from scratch

## Directory Structure

### Root Directory
- `Cargo.toml` and `Cargo.lock`: Rust project configuration files.
- Markdown files (`*.md`): Documentation and guides.
- `change_log.txt`: Complete development history and change log.
- `gpu/`: Contains GPU-related resources such as drivers, firmware, and documentation.
- `src/`: Source code for the main Orange Pi Builder project.
- `lcce/`: Live Custom Creation Edition standalone GUI application.
- `target/`: Build artifacts.

### `gpu/`
- `download_drivers.sh`: Script for downloading GPU drivers.
- `docs/`: Documentation related to GPU drivers.
- `firmware/`: Firmware files for GPU.
- `mesa/`: Mesa drivers.
- `opensource/`: Open-source GPU drivers.
- `proprietary/`: Proprietary GPU drivers.

### `src/`
- `main.rs`: Entry point for the application.
- `devicetree.rs`: Device tree management.
- `error.rs`: Error handling utilities.
- Subdirectories:
  - `armpi_tweaker/`: Complete Orange Pi 5 Plus system configuration and optimization.
  - `bootloader/`: Bootloader-related code.
  - `burner/`: Code for burning images.
  - `config/`: Configuration management.
  - `flasher/`: Code for flashing images.
  - `ui/`: User interface components.
  - `updates/`: Update management.
  - `utils/`: Utility functions.
  - `wizard/`: Wizard interface.

### `lcce/`
- `Cargo.toml`: LCCE project dependencies and configuration.
- `build.rs`: Build script for Slint UI compilation.
- `src/`: LCCE source code.
  - `main.rs`: Main application entry point with GUI initialization.
  - `system.rs`: Hardware detection and system information.
  - `config.rs`: Configuration management and serialization.
  - `modules/`: Individual GUI modules for each feature.
- `ui/`: Slint user interface files.
  - `main.slint`: Complete GUI layout and components.
  - `simple.slint`: Simplified test layout.
- `assets/`: Application icons, images, and resources.

## Installation

### Prerequisites
- Rust programming language installed.
- Compatible Orange Pi device (Orange Pi 5 Plus).
- Git for downloading repositories.

### Required Files
The application expects Orange Pi 5 Plus specific files in `~/Orange-Pi/gamescope/`:
- GameScope ARM64 package
- Mali GPU drivers (g13p0 and g6p0 variants)
- Rockchip MPP package
- Device tree files for Orange Pi 5 Plus

The application will automatically download other required components:
- Armbian Rockchip kernel (rk-6.1-rkr5.1)
- Rockchip U-Boot and firmware
- Development tools (rkdeveloptool)
- RetroArch and cores (for gaming distro)
- Orange Pi build system

### Steps
1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```
2. Navigate to the project directory:
   ```bash
   cd Orange-Pi-Builder
   ```
3. Build the project:
   ```bash
   cargo build
   ```
4. Run the application:
   ```bash
   cargo run
   ```

## Usage

### Driver Management
- Use `gpu/download_drivers.sh` to download and install GPU drivers.
- Refer to `gpu/docs/DRIVER_MATRIX.md` for compatibility information.

### Firmware Updates
- Place firmware files in `gpu/firmware/`.
- Use the `src/flasher/` module to flash firmware to the device.

### Bootloader Configuration
- Modify `src/bootloader/uboot.rs` for U-Boot configurations.

### Error Handling
- Refer to `src/error/` for logging and recovery mechanisms.

### UI Customization
- Edit `src/ui/theme.rs` for theme customization.

### Wizard Interface
- Follow the step-by-step guide provided by the wizard in `src/wizard/`.

### Arm-Pi Tweaker Module
- Access comprehensive system configuration through the "Arm-Pi Tweaker" option in the main menu.
- Configure GPU drivers, hardware acceleration, CPU governors, and system services.
- Manage kernel installations, SSH settings, network configuration, and security.
- Monitor system performance and temperature in real-time.

### Live Custom Creation Edition (LCCE)
- Navigate to the `lcce/` directory and run `cargo run` to launch the GUI application.
- Use the visual interface to configure kernel modifications, video drivers, and multimedia options.
- Install your current SD card system to NVMe/eMMC storage with custom modifications.
- Monitor installation progress through the real-time progress interface.
- Access all Arm-Pi Tweaker functionality through the modern GUI.

### Advanced Usage
#### Custom Driver Installation
- For advanced users, manually place drivers in the `gpu/opensource/` or `gpu/proprietary/` directories.
- Update the `gpu/docs/DRIVER_MATRIX.md` file to reflect changes.

#### Debugging
- Use the `src/error/logging.rs` module to enable detailed logging.
- Run the application in debug mode:
  ```bash
  cargo run --debug
  ```

#### Extending Functionality
- Add new modules under `src/utils/` for additional utilities.
- Ensure new modules are registered in `main.rs`.

## Technical Details

### Supported Devices
- Orange Pi 5
- Orange Pi 3
- Orange Pi Zero 2

### Dependencies
- **Rust**: The project is built using Rust. Ensure the latest stable version is installed.
- **Drivers**: GPU drivers are sourced from both open-source and proprietary repositories.
- **Firmware**: Compatible firmware files are required for device-specific functionality.

### Architecture
The project is structured into modular components:
- **Bootloader**: Handles device booting and initialization.
- **Burner**: Manages image burning processes.
- **Flasher**: Responsible for flashing firmware and updates.
- **UI**: Provides a user-friendly interface for configuration and monitoring.
- **Wizard**: Guides users through setup and configuration.

### Build Process
1. Compile the project using Cargo:
   ```bash
   cargo build --release
   ```
2. Generate documentation:
   ```bash
   cargo doc --open
   ```

### Testing
Run unit tests:
```bash
cargo test
```

### Debugging
Enable verbose logging:
```bash
cargo run -- --verbose
```

## Development Status & TODO

### ✅ Recently Completed
- [x] **Arm-Pi Tweaker Module**: Complete system configuration tool with 12 sub-modules
- [x] **Navigation System**: Fixed all menu navigation issues
- [x] **LCCE Project Structure**: Standalone GUI application foundation
- [x] **Slint GUI Framework**: Working user interface with module layout
- [x] **System Detection**: Hardware identification and configuration management
- [x] **Documentation**: Comprehensive README and change log

### 🚧 In Progress (LCCE Development)
- [ ] **Kernel Module GUI**: Visual interface for kernel modifications
- [ ] **Video Driver GUI**: Graphics driver configuration interface  
- [ ] **Emulation Module GUI**: Multimedia and emulation setup
- [ ] **Storage Installation Logic**: SD to NVMe/eMMC cloning system
- [ ] **Progress Monitoring**: Real-time installation feedback

### 📋 Planned Features
- [ ] **LCCE Advanced Features**:
  - [ ] Configuration profile save/load system
  - [ ] Advanced partition management
  - [ ] Network-based remote installation
  - [ ] Automated system validation and testing
- [ ] **Integration Improvements**:
  - [ ] Add support for additional Orange Pi models
  - [ ] Enhanced GPU driver compatibility checks
  - [ ] Advanced performance profiling tools
- [ ] **User Experience**:
  - [ ] Interactive tutorials and help system
  - [ ] Backup and restore functionality
  - [ ] Custom theme support

### 🔧 Technical Improvements
- [ ] **Testing**:
  - [ ] Write integration tests for burner and flasher modules
  - [ ] Improve test coverage for error handling
  - [ ] Add GUI automated testing for LCCE
- [ ] **Optimization**:
  - [ ] Optimize firmware flashing speed
  - [ ] Reduce memory usage in UI components
  - [ ] Improve LCCE startup time
- [ ] **Documentation**:
  - [ ] Expand technical documentation for each module
  - [ ] Add examples for common use cases
  - [ ] Create video tutorials for LCCE usage

### 🌐 Community & Distribution
- [ ] Create a community forum for user discussions
- [ ] Add a contribution guide to the repository
- [ ] Package LCCE for easy distribution
- [ ] Create installation scripts for different platforms

## Troubleshooting

### Common Issues
#### Build Errors
- Ensure Rust is installed and updated to the latest version.
- Verify dependencies in `Cargo.toml`.

#### Driver Compatibility
- Check `gpu/docs/DRIVER_MATRIX.md` for supported devices.
- Ensure the correct driver version is placed in the `gpu/firmware/` directory.

#### Flashing Issues
- Verify the device is properly connected.
- Check logs in `src/error/recovery.rs` for detailed error messages.

### Support
- For additional help, contact [support@example.com](mailto:support@example.com).

## Contributing

### Guidelines
- Follow Rust coding standards.
- Document your code thoroughly.
- Submit pull requests with detailed descriptions.

### Reporting Issues
- Use the issue tracker to report bugs or request features.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments
- Open-source contributors.
- Orange Pi community.
- Special thanks to the Rust community for their support.

## Contact
For questions or support, contact [support@example.com](mailto:support@example.com).

## Devicetree Files

### Overview
Devicetree files are used to describe the hardware components of Orange Pi devices. These files provide a standardized way to define the configuration and capabilities of the device, enabling the software to interact with the hardware effectively.

### Location
- The devicetree files are managed in `src/devicetree.rs`.

### Usage
- Modify the devicetree file to add or update hardware configurations.
- Ensure compatibility with the device's firmware and drivers.

### Example
Here is a snippet of a typical devicetree configuration:
```rust
// Example devicetree configuration
compatible = "orange-pi,opi5";
model = "Orange Pi 5";
...
```

## Mali GPU Information

### Overview
The Mali GPU is a high-performance graphics processor used in Orange Pi devices. This project supports both open-source and proprietary drivers for the Mali GPU.

### Supported Drivers
- **Open-Source Drivers**: Located in `gpu/opensource/`.
- **Proprietary Drivers**: Located in `gpu/proprietary/`.
  - Examples:
    - `libmali-valhall-g610-g13p0-wayland-gbm_1.9-1_arm64.deb`
    - `libmali-valhall-g610-g6p0-wayland-gbm_1.9-1_arm64.deb`

### Driver Installation
- Use the script `gpu/download_drivers.sh` to download and install drivers.
- Refer to `gpu/docs/DRIVER_MATRIX.md` for compatibility information.

### Troubleshooting
- Ensure the correct driver version is installed for your device.
- Check logs for errors during driver installation or usage.
