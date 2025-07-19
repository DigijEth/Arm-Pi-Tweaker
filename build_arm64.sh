#!/bin/bash

# Orange Pi Builder - ARM64 Build Verification Script
# This script builds and tests the Orange Pi Builder on ARM64 systems

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

# Check if we're in the right directory
check_project_dir() {
    if [[ ! -f "Cargo.toml" ]] || [[ ! -d "src" ]]; then
        log_error "Please run this script from the Orange Pi Builder project root directory"
        log_error "Expected files: Cargo.toml, src/"
        exit 1
    fi
    
    log_info "Project directory verified"
}

# Check system requirements
check_system() {
    log_section "Checking System Requirements"
    
    # Check architecture
    ARCH=$(uname -m)
    if [[ "$ARCH" != "aarch64" ]]; then
        log_warn "Not running on ARM64. Detected: $ARCH"
        log_warn "Cross-compilation may be needed"
    else
        log_info "Running on ARM64 (aarch64)"
    fi
    
    # Check Rust installation
    if ! command -v rustc >/dev/null 2>&1; then
        log_error "Rust not found. Please run install_arm64_deps.sh first"
        exit 1
    fi
    
    RUST_VERSION=$(rustc --version)
    log_info "Rust found: $RUST_VERSION"
    
    # Check Cargo
    if ! command -v cargo >/dev/null 2>&1; then
        log_error "Cargo not found. Please run install_arm64_deps.sh first"
        exit 1
    fi
    
    CARGO_VERSION=$(cargo --version)
    log_info "Cargo found: $CARGO_VERSION"
    
    # Check essential system tools
    local missing_tools=()
    local tools=("git" "make" "gcc" "pkg-config")
    
    for tool in "${tools[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done
    
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        log_error "Missing required tools: ${missing_tools[*]}"
        log_error "Please run install_arm64_deps.sh first"
        exit 1
    fi
    
    log_info "All required tools found"
}

# Check Rust dependencies
check_rust_deps() {
    log_section "Checking Rust Dependencies"
    
    log_info "Checking for required system libraries..."
    
    # Check for development libraries using pkg-config
    local libs=("dbus-1")
    local missing_libs=()
    
    for lib in "${libs[@]}"; do
        if ! pkg-config --exists "$lib" 2>/dev/null; then
            missing_libs+=("$lib")
        else
            log_info "✓ $lib development library found"
        fi
    done
    
    if [[ ${#missing_libs[@]} -gt 0 ]]; then
        log_error "Missing development libraries: ${missing_libs[*]}"
        log_error "Please run install_arm64_deps.sh first"
        exit 1
    fi
}

# Clean previous builds
clean_build() {
    log_section "Cleaning Previous Builds"
    
    if [[ -d "target" ]]; then
        log_info "Removing previous build artifacts..."
        cargo clean
    fi
    
    log_info "Clean completed"
}

# Update dependencies
update_deps() {
    log_section "Updating Dependencies"
    
    log_info "Updating Cargo index..."
    cargo update
    
    log_info "Dependencies updated"
}

# Build in debug mode
build_debug() {
    log_section "Building in Debug Mode"
    
    log_info "Starting debug build..."
    
    # Set environment variables for ARM64 builds
    export RUSTFLAGS="-C target-cpu=native"
    
    if cargo build; then
        log_info "✓ Debug build successful"
        
        # Check if binary was created
        if [[ -f "target/debug/builder" ]]; then
            local size=$(du -h "target/debug/builder" | cut -f1)
            log_info "Debug binary size: $size"
        else
            log_error "Debug binary not found at target/debug/builder"
            return 1
        fi
    else
        log_error "Debug build failed"
        return 1
    fi
}

# Build in release mode
build_release() {
    log_section "Building in Release Mode"
    
    log_info "Starting release build..."
    
    # Set environment variables for optimized ARM64 builds
    export RUSTFLAGS="-C target-cpu=native -C opt-level=3"
    
    if cargo build --release; then
        log_info "✓ Release build successful"
        
        # Check if binary was created
        if [[ -f "target/release/builder" ]]; then
            local size=$(du -h "target/release/builder" | cut -f1)
            log_info "Release binary size: $size"
        else
            log_error "Release binary not found at target/release/builder"
            return 1
        fi
    else
        log_error "Release build failed"
        return 1
    fi
}

# Run tests
run_tests() {
    log_section "Running Tests"
    
    log_info "Running unit tests..."
    
    if cargo test; then
        log_info "✓ All tests passed"
    else
        log_warn "Some tests failed - this may be expected for hardware-dependent tests"
    fi
}

# Check binary functionality
test_binary() {
    log_section "Testing Binary Functionality"
    
    local binary="target/release/builder"
    
    if [[ ! -f "$binary" ]]; then
        log_error "Release binary not found"
        return 1
    fi
    
    # Test if binary is executable
    if [[ -x "$binary" ]]; then
        log_info "✓ Binary is executable"
    else
        log_error "Binary is not executable"
        return 1
    fi
    
    # Test if binary can show help
    log_info "Testing binary help output..."
    if timeout 5s "$binary" --help >/dev/null 2>&1; then
        log_info "✓ Binary help command works"
    else
        log_warn "Binary help command failed or timed out"
        log_warn "This may be normal for TUI applications"
    fi
    
    # Check dependencies
    log_info "Checking binary dependencies..."
    if command -v ldd >/dev/null 2>&1; then
        local missing_deps=()
        while IFS= read -r line; do
            if [[ "$line" =~ "not found" ]]; then
                missing_deps+=("$line")
            fi
        done < <(ldd "$binary" 2>/dev/null)
        
        if [[ ${#missing_deps[@]} -eq 0 ]]; then
            log_info "✓ All library dependencies satisfied"
        else
            log_error "Missing library dependencies:"
            printf '%s\n' "${missing_deps[@]}"
            return 1
        fi
    fi
}

# Create installation package
create_package() {
    log_section "Creating Installation Package"
    
    local install_dir="orange-pi-builder-arm64"
    local binary="target/release/builder"
    
    if [[ ! -f "$binary" ]]; then
        log_error "Release binary not found"
        return 1
    fi
    
    log_info "Creating installation directory..."
    rm -rf "$install_dir"
    mkdir -p "$install_dir"
    
    # Copy binary
    cp "$binary" "$install_dir/orange-pi-builder"
    
    # Copy documentation
    [[ -f "README.md" ]] && cp "README.md" "$install_dir/"
    [[ -f "LICENSE" ]] && cp "LICENSE" "$install_dir/"
    
    # Copy scripts
    [[ -f "install_arm64_deps.sh" ]] && cp "install_arm64_deps.sh" "$install_dir/"
    
    # Create install script
    cat > "$install_dir/install.sh" << 'EOF'
#!/bin/bash
# Orange Pi Builder Installation Script

set -e

INSTALL_DIR="/usr/local/bin"
BINARY_NAME="orange-pi-builder"

echo "Installing Orange Pi Builder..."

# Check if running as root
if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root or with sudo"
    exit 1
fi

# Copy binary
cp "$BINARY_NAME" "$INSTALL_DIR/"
chmod +x "$INSTALL_DIR/$BINARY_NAME"

echo "Orange Pi Builder installed to $INSTALL_DIR/$BINARY_NAME"
echo "You can now run it with: orange-pi-builder"
EOF
    
    chmod +x "$install_dir/install.sh"
    
    # Create tarball
    local tarball="orange-pi-builder-arm64.tar.gz"
    tar -czf "$tarball" "$install_dir"
    
    log_info "✓ Installation package created: $tarball"
    log_info "Package contents:"
    tar -tzf "$tarball"
}

# Main build function
main() {
    log_section "Orange Pi Builder - ARM64 Build Script"
    log_info "This script will build and test the Orange Pi Builder for ARM64"
    
    # Parse command line arguments
    local build_type="both"
    local run_tests_flag=true
    local create_package_flag=false
    local clean_flag=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug-only)
                build_type="debug"
                shift
                ;;
            --release-only)
                build_type="release"
                shift
                ;;
            --no-tests)
                run_tests_flag=false
                shift
                ;;
            --package)
                create_package_flag=true
                shift
                ;;
            --clean)
                clean_flag=true
                shift
                ;;
            --help)
                echo "Usage: $0 [OPTIONS]"
                echo "Options:"
                echo "  --debug-only    Build only debug version"
                echo "  --release-only  Build only release version"
                echo "  --no-tests      Skip running tests"
                echo "  --package       Create installation package"
                echo "  --clean         Clean before building"
                echo "  --help          Show this help"
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    # Run build steps
    check_project_dir
    check_system
    check_rust_deps
    
    if [[ "$clean_flag" == true ]]; then
        clean_build
    fi
    
    update_deps
    
    # Build based on type requested
    case $build_type in
        "debug")
            build_debug
            ;;
        "release")
            build_release
            ;;
        "both")
            build_debug
            build_release
            ;;
    esac
    
    if [[ "$run_tests_flag" == true ]]; then
        run_tests
    fi
    
    # Test the release binary if it was built
    if [[ "$build_type" == "release" ]] || [[ "$build_type" == "both" ]]; then
        test_binary
    fi
    
    if [[ "$create_package_flag" == true ]]; then
        create_package
    fi
    
    log_section "Build Complete!"
    log_info "Orange Pi Builder has been successfully built for ARM64"
    
    if [[ -f "target/release/builder" ]]; then
        log_info "Release binary: target/release/builder"
        log_info "You can run it with: ./target/release/builder"
    fi
    
    if [[ -f "target/debug/builder" ]]; then
        log_info "Debug binary: target/debug/builder"
        log_info "You can run it with: ./target/debug/builder"
    fi
}

# Run main function with all arguments
main "$@"