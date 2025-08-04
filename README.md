I will be moving to a self hosted gitea server where the LLM model for Rocky will be saved and running the remote server for. The reason for this is everytime I try and do a new commit I have to fight with github and --force everything as it always says my repo is newer than my codebase no matter what. My local code can be 2 weeks ahead, but git hub still says that its version is newer and I have to pull and "become current" with the remote version. Im tired of the accident "updates" vs code runs my copilot and decides it knows better then me and runs things without permissiom. That has set me back 3 times devloping this app. I would be much furthur into writing the Rocky drivers that will intergrate a more advanced version the Ai into the OS and will allow us to collect data that will make better drivers.  

# Arm-Pi Tweaker

A comprehensive optimization and management tool for the Orange Pi 5 Plus, featuring advanced system tweaking capabilities and AI-powered development assistance.

## Major Features

### System Optimization
- **GPU Management**: Advanced GPU driver configuration and optimization for RK3588 with Mali-specific tweaks
  - Mesa driver optimizations and tuning
  - Valhall driver tweaks and performance enhancements
  - Complete GPU menu overhaul (coming soon)
- **Kernel Management**: Custom kernel building, patching, and performance tuning
- **Storage Management**: F2FS optimization, partition management, and backup/restore functionality
- **UEFI Configuration**: Boot management and UEFI settings optimization
- **Custom Image Builder**: Create optimized Orange Pi 5 Plus images with custom configurations

### AI-Powered Development Assistant
Arm-Pi Tweaker introduces **Rocky**, an AI assistant specifically trained on Orange Pi 5 Plus and RK3588 hardware documentation, driver specifications, and optimization techniques. Rocky is designed to help developers with:

- Hardware-specific troubleshooting and optimization guidance
- Driver development assistance and debugging
- Performance tuning recommendations
- Custom configuration suggestions
- Real-time hardware analysis and insights

#### Rocky AI Features (Coming Soon)
- **Integrated AI Chat**: Direct integration within the application for real-time assistance
- **Driver Development Helper**: AI-guided driver creation and debugging tools
- **Data Collection System**: Automated hardware data capture to improve driver development
- **Performance Analytics**: AI-powered analysis of system performance and optimization suggestions

> **Note**: The Rocky model is currently in training and will be available via download or remote connection. Detailed integration information will be provided upon release.

## Building the Application

### Prerequisites
- Qt6 development packages
- CMake 3.16 or higher
- Build essentials (gcc, make)
- OpenMP support
- F2FS tools (optional, for F2FS partition support)

### Build with CMake (Recommended)
```bash
# Clone the repository
git clone <repository-url>
cd arm-pi-tweaker

# Build the application
chmod +x build.sh
./build.sh
```

### Build with qmake (Alternative)
```bash
# Build with qmake
chmod +x build_qmake.sh
./build_qmake.sh
```

### Manual Build
```bash
# Create build directory and configure
mkdir build && cd build
cmake ..
make -j$(nproc)

# The executable will be in build/bin/arm-pi-tweaker
```

## Installation Requirements

### Automatic Installation
The build script will automatically install required dependencies:
- Qt6 base development packages
- CMake and build tools
- F2FS utilities

### Manual Installation
```bash
sudo apt update
sudo apt install -y qtbase5-dev qtbase5-dev-tools cmake build-essential f2fs-tools
```

## Development Status & Upcoming Features

### Current Development
- **TweakerUEFI + TweakerKernel**: New kernel support with enhanced functionality (final testing phase)
- **GPU Menu Overhaul**: Complete redesign with Mali-specific optimizations for Mesa and Valhall drivers
- **Image Creator & Kernel Builder**: Race condition issues being addressed (lower priority)
- **Rust CLI**: Undergoing major overhaul, temporarily removed for restructuring

### Coming Soon
- Enhanced GPU management with driver-specific optimizations
- Expanded kernel compatibility and tuning options
- Improved build stability and performance
- **Rust CLI Return**: Redesigned command-line interface with improved architecture (next build)

## Project Goals

### Hardware Optimization
- Maximize Orange Pi 5 Plus performance through intelligent system tuning
- Provide comprehensive hardware management tools for developers and enthusiasts
- Enable easy custom image creation with optimized configurations

### AI-Enhanced Development
- Revolutionize Orange Pi 5 Plus development with AI-powered assistance
- Create a comprehensive knowledge base for RK3588 hardware development
- Develop intelligent driver creation and debugging tools
- Establish automated data collection systems to improve future hardware support

### Future Vision
The ultimate goal is to create an ecosystem where:
- Developers can leverage AI assistance for complex hardware development tasks
- System optimization is guided by AI recommendations based on real-world data
- Driver development is accelerated through intelligent analysis and suggestions
- The Orange Pi 5 Plus community benefits from collective knowledge and AI insights

## Contributing

This project aims to advance Orange Pi 5 Plus development through both traditional optimization techniques and cutting-edge AI assistance. Contributions are welcome in all areas, from system optimization features to AI integration development.

## License

This project is open source and available under the appropriate license terms.

---

*Arm-Pi Tweaker with Rocky AI - Bringing intelligent hardware optimization to the Orange Pi 5 Plus community.*
