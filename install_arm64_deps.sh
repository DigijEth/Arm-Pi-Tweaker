#!/bin/bash

# Orange Pi Builder - ARM64 Dependencies Installation Script
# This script installs all required dependencies for building the Orange Pi Builder on ARM64
# Designed to run on Orange Pi 5/5+ or other ARM64 systems running Debian/Ubuntu

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_section() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Check if running on ARM64
check_architecture() {
    log_section "Checking System Architecture"
    
    ARCH=$(uname -m)
    if [[ "$ARCH" != "aarch64" ]]; then
        log_error "This script is designed for ARM64 (aarch64) systems. Detected: $ARCH"
        log_error "Please run this on an Orange Pi 5/5+ or other ARM64 device"
        exit 1
    fi
    
    log_info "Architecture check passed: $ARCH"
}

# Check if running as root or with sudo
check_privileges() {
    log_section "Checking Privileges"
    
    if [[ $EUID -eq 0 ]]; then
        log_info "Running as root"
        SUDO=""
    elif command -v sudo >/dev/null 2>&1; then
        log_info "Using sudo for privileged operations"
        SUDO="sudo"
    else
        log_error "This script requires root privileges or sudo"
        log_error "Please run as root or install sudo"
        exit 1
    fi
}

# Detect distribution
detect_distro() {
    log_section "Detecting Distribution"
    
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
        log_info "Detected: $PRETTY_NAME"
    else
        log_error "Cannot detect distribution"
        exit 1
    fi
    
    # Check if supported
    case $DISTRO in
        ubuntu|debian)
            log_info "Supported distribution detected"
            ;;
        *)
            log_warn "Unsupported distribution: $DISTRO"
            log_warn "This script is designed for Ubuntu/Debian, but will attempt to continue"
            ;;
    esac
}

# Update system packages
update_system() {
    log_section "Updating System Packages"
    
    log_info "Updating package lists..."
    $SUDO apt-get update
    
    log_info "Upgrading existing packages..."
    $SUDO apt-get upgrade -y
    
    log_info "Installing essential build tools..."
    $SUDO apt-get install -y \
        curl \
        wget \
        git \
        build-essential \
        pkg-config \
        software-properties-common \
        apt-transport-https \
        ca-certificates \
        gnupg \
        lsb-release
}

# Install Rust toolchain
install_rust() {
    log_section "Installing Rust Toolchain"
    
    if command -v rustc >/dev/null 2>&1; then
        RUST_VERSION=$(rustc --version)
        log_info "Rust already installed: $RUST_VERSION"
        
        # Check if it's recent enough (1.70+)
        RUST_MAJOR=$(rustc --version | cut -d' ' -f2 | cut -d'.' -f1)
        RUST_MINOR=$(rustc --version | cut -d' ' -f2 | cut -d'.' -f2)
        
        if [[ $RUST_MAJOR -gt 1 ]] || [[ $RUST_MAJOR -eq 1 && $RUST_MINOR -ge 70 ]]; then
            log_info "Rust version is sufficient"
        else
            log_warn "Rust version may be too old, updating..."
            rustup update
        fi
    else
        log_info "Installing Rust via rustup..."
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable
        
        # Source the environment
        export PATH="$HOME/.cargo/bin:$PATH"
        source ~/.cargo/env 2>/dev/null || true
        
        log_info "Rust installed successfully"
    fi
    
    # Install additional Rust components for cross-compilation
    log_info "Installing Rust components..."
    rustup component add clippy rustfmt
    
    # Verify installation
    if command -v cargo >/dev/null 2>&1; then
        CARGO_VERSION=$(cargo --version)
        log_info "Cargo installed: $CARGO_VERSION"
    else
        log_error "Failed to install Rust/Cargo"
        exit 1
    fi
}

# Install development libraries
install_dev_libraries() {
    log_section "Installing Development Libraries"
    
    log_info "Installing system development libraries..."
    $SUDO apt-get install -y \
        libc6-dev \
        libssl-dev \
        libdbus-1-dev \
        libsystemd-dev \
        libgtk-3-dev \
        libglib2.0-dev \
        libcairo2-dev \
        libpango1.0-dev \
        libatk1.0-dev \
        libgdk-pixbuf2.0-dev \
        libsoup2.4-dev \
        libwebkit2gtk-4.0-dev \
        libudev-dev \
        libhidapi-dev \
        libusb-1.0-0-dev
    
    log_info "Installing terminal UI libraries..."
    $SUDO apt-get install -y \
        libncurses5-dev \
        libncursesw5-dev \
        libtinfo-dev
    
    log_info "Installing compression libraries..."
    $SUDO apt-get install -y \
        zlib1g-dev \
        liblzma-dev \
        libbz2-dev \
        libzstd-dev
}

# Install cross-compilation tools
install_cross_compilation_tools() {
    log_section "Installing Cross-Compilation Tools"
    
    log_info "Installing GCC cross-compilation toolchain..."
    $SUDO apt-get install -y \
        gcc-aarch64-linux-gnu \
        g++-aarch64-linux-gnu \
        gcc-arm-linux-gnueabihf \
        g++-arm-linux-gnueabihf \
        libc6-dev-arm64-cross \
        libc6-dev-armhf-cross
    
    log_info "Installing binutils..."
    $SUDO apt-get install -y \
        binutils-aarch64-linux-gnu \
        binutils-arm-linux-gnueabihf
}

# Install kernel build dependencies
install_kernel_deps() {
    log_section "Installing Kernel Build Dependencies"
    
    log_info "Installing kernel build tools..."
    $SUDO apt-get install -y \
        bc \
        bison \
        flex \
        libelf-dev \
        kmod \
        cpio \
        rsync \
        tar \
        xz-utils \
        lz4 \
        make \
        gcc \
        libc6-dev \
        libssl-dev \
        device-tree-compiler
    
    log_info "Installing bootloader build dependencies..."
    $SUDO apt-get install -y \
        swig \
        python3-dev \
        python3-setuptools \
        python3-pip \
        uuid-dev
}

# Install image creation tools
install_image_tools() {
    log_section "Installing Image Creation Tools"
    
    log_info "Installing filesystem tools..."
    $SUDO apt-get install -y \
        parted \
        gdisk \
        dosfstools \
        mtools \
        e2fsprogs \
        btrfs-progs \
        f2fs-tools \
        ntfs-3g \
        exfat-fuse \
        exfat-utils
    
    log_info "Installing debootstrap and chroot tools..."
    $SUDO apt-get install -y \
        debootstrap \
        schroot \
        systemd-container \
        qemu-user-static \
        binfmt-support
    
    log_info "Installing archive and compression tools..."
    $SUDO apt-get install -y \
        zip \
        unzip \
        p7zip-full \
        gzip \
        pigz \
        pbzip2 \
        pxz
}

# Install hardware-specific tools
install_hardware_tools() {
    log_section "Installing Hardware-Specific Tools"
    
    log_info "Installing Rockchip tools..."
    $SUDO apt-get install -y \
        mtd-utils \
        android-tools-adb \
        android-tools-fastboot
    
    # Install rkdeveloptool from source if not available
    if ! command -v rkdeveloptool >/dev/null 2>&1; then
        log_info "Building rkdeveloptool from source..."
        
        # Install dependencies for rkdeveloptool
        $SUDO apt-get install -y \
            libusb-1.0-0-dev \
            autotools-dev \
            autoconf \
            automake \
            libtool
        
        # Create temporary build directory
        TEMP_DIR=$(mktemp -d)
        cd "$TEMP_DIR"
        
        # Clone and build rkdeveloptool
        git clone https://github.com/rockchip-linux/rkdeveloptool.git
        cd rkdeveloptool
        autoreconf -i
        ./configure
        make -j$(nproc)
        $SUDO make install
        
        # Cleanup
        cd ~
        rm -rf "$TEMP_DIR"
        
        log_info "rkdeveloptool installed successfully"
    else
        log_info "rkdeveloptool already available"
    fi
}

# Install Python dependencies
install_python_deps() {
    log_section "Installing Python Dependencies"
    
    log_info "Installing Python and pip..."
    $SUDO apt-get install -y \
        python3 \
        python3-pip \
        python3-venv \
        python3-dev
    
    log_info "Installing Python packages for build scripts..."
    python3 -m pip install --user --upgrade \
        setuptools \
        wheel \
        pycryptodome \
        requests \
        pyserial
}

# Setup environment
setup_environment() {
    log_section "Setting Up Build Environment"
    
    # Add Rust to PATH if not already there
    if ! echo "$PATH" | grep -q ".cargo/bin"; then
        log_info "Adding Rust to PATH..."
        echo 'export PATH="$HOME/.cargo/bin:$PATH"' >> ~/.bashrc
        export PATH="$HOME/.cargo/bin:$PATH"
    fi
    
    # Create build directories
    log_info "Creating build directories..."
    mkdir -p ~/Orange-Pi-Builder/{cache,downloads,builds,logs}
    
    # Set up udev rules for hardware access
    log_info "Setting up udev rules for hardware access..."
    
    cat << 'EOF' | $SUDO tee /etc/udev/rules.d/99-rockchip.rules > /dev/null
# Rockchip devices
SUBSYSTEM=="usb", ATTR{idVendor}=="2207", MODE="0666", GROUP="plugdev"
# Orange Pi devices
SUBSYSTEM=="usb", ATTR{idVendor}=="1f3a", MODE="0666", GROUP="plugdev"
EOF
    
    # Add user to plugdev group
    $SUDO usermod -a -G plugdev $USER || true
    
    # Reload udev rules
    $SUDO udevadm control --reload-rules
    $SUDO udevadm trigger
    
    log_info "Environment setup completed"
}

# Verify installation
verify_installation() {
    log_section "Verifying Installation"
    
    local errors=0
    
    # Check Rust
    if command -v rustc >/dev/null 2>&1 && command -v cargo >/dev/null 2>&1; then
        log_info "✓ Rust toolchain: $(rustc --version)"
    else
        log_error "✗ Rust toolchain not found"
        errors=$((errors + 1))
    fi
    
    # Check cross-compilation
    if command -v aarch64-linux-gnu-gcc >/dev/null 2>&1; then
        log_info "✓ ARM64 cross-compiler: $(aarch64-linux-gnu-gcc --version | head -1)"
    else
        log_error "✗ ARM64 cross-compiler not found"
        errors=$((errors + 1))
    fi
    
    # Check essential tools
    local tools=("git" "make" "parted" "debootstrap" "python3")
    for tool in "${tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            log_info "✓ $tool available"
        else
            log_error "✗ $tool not found"
            errors=$((errors + 1))
        fi
    done
    
    # Check libraries
    if pkg-config --exists dbus-1 >/dev/null 2>&1; then
        log_info "✓ D-Bus development libraries"
    else
        log_error "✗ D-Bus development libraries missing"
        errors=$((errors + 1))
    fi
    
    if [[ $errors -eq 0 ]]; then
        log_info "All dependencies verified successfully!"
        return 0
    else
        log_error "Found $errors missing dependencies"
        return 1
    fi
}

# Main installation function
main() {
    log_section "Orange Pi Builder - ARM64 Dependencies Installer"
    log_info "This script will install all dependencies required to build Orange Pi Builder on ARM64"
    
    # Ask for confirmation
    echo -n "Continue with installation? [y/N]: "
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        log_info "Installation cancelled"
        exit 0
    fi
    
    # Run installation steps
    check_architecture
    check_privileges
    detect_distro
    update_system
    install_rust
    install_dev_libraries
    install_cross_compilation_tools
    install_kernel_deps
    install_image_tools
    install_hardware_tools
    install_python_deps
    setup_environment
    
    # Verify everything is working
    if verify_installation; then
        log_section "Installation Complete!"
        log_info "All dependencies have been installed successfully."
        log_info ""
        log_info "Next steps:"
        log_info "1. Clone the Orange Pi Builder repository:"
        log_info "   git clone <repository-url>"
        log_info "2. Enter the directory and build:"
        log_info "   cd Orange-Pi-Builder"
        log_info "   cargo build --release"
        log_info "3. Run the application:"
        log_info "   ./target/release/orange-pi-builder"
        log_info ""
        log_warn "Note: You may need to log out and back in for group changes to take effect"
        log_warn "Or run: newgrp plugdev"
    else
        log_error "Installation completed with errors. Please check the output above."
        exit 1
    fi
}

# Run main function
main "$@"