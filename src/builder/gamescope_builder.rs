use std::fs;
use std::path::PathBuf;
use std::process::{Command, Output};
use std::io::Write;
use log::{info, warn};
use crate::error::BuilderError;
use std::env;
use chrono;

/// Desktop environment choice for GameScope
#[derive(Debug, Clone)]
pub enum DesktopChoice {
    /// LXQt desktop + Valve's GameScope compositor
    LXQtWithGameScope,
    /// Minimal system with GameScope launching RetroArch at boot
    GameScopeRetroArch,
}

/// Configuration for GameScope build
#[derive(Debug, Clone)]
pub struct GameScopeConfig {
    pub suite: String,
    pub arch: String,
    pub mirror: String,
    pub build_dir: PathBuf,
    pub output_path: String,
    pub hostname: String,
    pub username: String,
    pub password: String,
    pub root_password: String,
    pub locale: String,
    pub timezone: String,
    pub kernel_choice: KernelChoice,
    pub desktop_choice: DesktopChoice,
    pub write_to_device: bool,
}

#[derive(Debug, Clone)]
pub enum KernelChoice {
    Rockchip51,  // 5.1 kernel
    Rockchip61,  // 6.1 kernel
}

pub struct GameScopeBuilder {
    config: GameScopeConfig,
    orange_pi_dir: PathBuf,
    rootfs_dir: PathBuf,
    kernel_dir: PathBuf,
    cross_compile: String,
    jobs: usize,
}

impl GameScopeBuilder {
    pub fn from_gamescope_config(config: GameScopeConfig) -> Result<Self, BuilderError> {
        // Get user's home directory and create Orange-Pi path
        let home_dir = env::var("HOME")
            .or_else(|_| env::var("USERPROFILE"))
            .map_err(|_| BuilderError::ConfigLoadError("Cannot determine home directory".to_string()))?;
        
        let orange_pi_dir = PathBuf::from(home_dir).join("Orange-Pi");
        let rootfs_dir = config.build_dir.join("rootfs");
        
        // Determine kernel directory based on choice
        let kernel_dir = match config.kernel_choice {
            KernelChoice::Rockchip51 => orange_pi_dir.join("kernel").join("rockchip-5.1"),
            KernelChoice::Rockchip61 => orange_pi_dir.join("kernel").join("rockchip-6.1"),
        };

        Ok(Self {
            config,
            orange_pi_dir,
            rootfs_dir,
            kernel_dir,
            cross_compile: "aarch64-linux-gnu-".to_string(),
            jobs: num_cpus::get(),
        })
    }

    /// Main build function
    pub fn build(&self) -> Result<(), BuilderError> {
        info!("Starting GameScope-Pi build process");
        self.log_step("Starting GameScope-Pi build process");
        
        // Check and create Orange-Pi directory structure
        self.log_step("Ensuring Orange-Pi directory structure");
        self.ensure_orange_pi_structure().map_err(|e| {
            self.log_error("Failed to create Orange-Pi directory structure", &e);
            e
        })?;
        
        // Download missing files
        self.log_step("Downloading missing files");
        self.download_missing_files().map_err(|e| {
            self.log_error("Failed to download missing files", &e);
            e
        })?;
        
        // Prepare kernel
        self.log_step("Preparing kernel configuration");
        self.prepare_kernel().map_err(|e| {
            self.log_error("Failed to prepare kernel", &e);
            e
        })?;
        
        // Run debootstrap
        self.log_step("Running debootstrap for Debian 12.11");
        self.run_debootstrap().map_err(|e| {
            self.log_error("Failed to run debootstrap", &e);
            e
        })?;
        
        // Build and install kernel
        self.log_step("Building and installing kernel");
        self.build_kernel().map_err(|e| {
            self.log_error("Failed to build kernel", &e);
            e
        })?;
        
        // Install multimedia, gaming, and additional packages via apt
        self.log_step("Installing multimedia, gaming, and additional packages via apt");
        self.install_multimedia_and_gaming_packages().map_err(|e| {
            self.log_error("Failed to install multimedia and gaming packages", &e);
            e
        })?;
        
        // Install desktop environment based on choice
        match self.config.desktop_choice {
            DesktopChoice::LXQtWithGameScope => {
                self.log_step("Installing LXQt desktop environment");
                self.install_lxqt_desktop().map_err(|e| {
                    self.log_error("Failed to install LXQt desktop", &e);
                    e
                })?;
                
                self.log_step("Installing Valve's GameScope compositor");
                self.install_gamescope().map_err(|e| {
                    self.log_error("Failed to install GameScope", &e);
                    e
                })?;
            },
            DesktopChoice::GameScopeRetroArch => {
                self.log_step("Installing minimal system with Valve's GameScope and RetroArch");
                self.install_gamescope().map_err(|e| {
                    self.log_error("Failed to install GameScope", &e);
                    e
                })?;
                
                self.install_retroarch().map_err(|e| {
                    self.log_error("Failed to install RetroArch", &e);
                    e
                })?;
            },
        }
        
        // Build and install U-Boot
        self.log_step("Building and installing U-Boot");
        self.build_uboot().map_err(|e| {
            self.log_error("Failed to build U-Boot", &e);
            e
        })?;
        
        // Configure audio system
        self.log_step("Configuring audio system");
        self.configure_audio_system().map_err(|e| {
            self.log_error("Failed to configure audio system", &e);
            e
        })?;
        
        // Configure GameScope system
        self.log_step("Configuring GameScope system");
        self.configure_gamescope_system().map_err(|e| {
            self.log_error("Failed to configure GameScope system", &e);
            e
        })?;
        
        // Create final image or write to device
        if self.config.write_to_device {
            self.log_step("Writing to device");
            self.write_to_device().map_err(|e| {
                self.log_error("Failed to write to device", &e);
                e
            })?;
        } else {
            self.log_step("Creating final image");
            self.create_image().map_err(|e| {
                self.log_error("Failed to create image", &e);
                e
            })?;
        }
        
        info!("GameScope-Pi build completed successfully!");
        self.log_step("GameScope-Pi build completed successfully!");
        Ok(())
    }
    
    /// Execute a command and log all output to /logs directory
    fn execute_command_with_logging(&self, command: &str, args: &[&str], log_name: &str) -> Result<Output, BuilderError> {
        let timestamp = chrono::Utc::now().format("%Y-%m-%d_%H-%M-%S");
        let log_file_name = format!("{}_{}.log", log_name, timestamp);
        let log_path = PathBuf::from("/logs").join(&log_file_name);
        
        // Ensure /logs directory exists
        fs::create_dir_all("/logs")
            .map_err(|e| BuilderError::IoError(format!("Failed to create /logs directory: {}", e)))?;
        
        info!("Executing command: {} {}", command, args.join(" "));
        info!("Logging output to: {}", log_path.display());
        
        // Execute the command
        let mut cmd = Command::new(command);
        cmd.args(args);
        
        // Add environment variables for debootstrap
        if command == "debootstrap" {
            cmd.env("DEBOOTSTRAP_DIR", "/usr/share/debootstrap");
        }
        
        let output = cmd.output()
            .map_err(|e| {
                let error_msg = format!("Failed to execute {}: {}", command, e);
                // Log the error
                let _ = fs::write(&log_path, format!("EXECUTION ERROR: {}\n", error_msg));
                BuilderError::BuildFailed(error_msg)
            })?;
        
        // Create detailed log content
        let mut log_content = String::new();
        log_content.push_str(&format!("Command: {} {}\n", command, args.join(" ")));
        log_content.push_str(&format!("Timestamp: {}\n", chrono::Utc::now().format("%Y-%m-%d %H:%M:%S UTC")));
        log_content.push_str(&format!("Exit Code: {}\n", output.status.code().unwrap_or(-1)));
        log_content.push_str("\n=== STDOUT ===\n");
        log_content.push_str(&String::from_utf8_lossy(&output.stdout));
        log_content.push_str("\n\n=== STDERR ===\n");
        log_content.push_str(&String::from_utf8_lossy(&output.stderr));
        log_content.push_str("\n\n=== END OF LOG ===\n");
        
        // Write to log file
        fs::write(&log_path, &log_content)
            .map_err(|e| BuilderError::IoError(format!("Failed to write log file: {}", e)))?;
        
        // Also log to the main gamescope_build.log
        let main_log_path = PathBuf::from("/logs").join("gamescope_build.log");
        if let Ok(mut file) = fs::OpenOptions::new()
            .create(true)
            .append(true)
            .open(&main_log_path)
        {
            let _ = writeln!(file, "\n--- {} ---", timestamp);
            let _ = writeln!(file, "Command: {} {}", command, args.join(" "));
            let _ = writeln!(file, "Exit Code: {}", output.status.code().unwrap_or(-1));
            let _ = writeln!(file, "Log File: {}", log_file_name);
            if !output.status.success() {
                let _ = writeln!(file, "ERROR: Command failed");
                let _ = writeln!(file, "STDERR Preview: {}", 
                    String::from_utf8_lossy(&output.stderr).lines().take(5).collect::<Vec<_>>().join("\n"));
            }
        }
        
        Ok(output)
    }
    
    /// Log a build step - ensure logs are written to /logs directory
    fn log_step(&self, message: &str) {
        // Write to global logger if available
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_info(
                message,
                "GAMESCOPE_BUILDER",
                "build_step"
            );
        }
        
        // Also write to /logs directory
        let log_dir = std::path::Path::new("/logs");
        if let Err(e) = std::fs::create_dir_all(log_dir) {
            eprintln!("Failed to create /logs directory: {}", e);
        } else {
            let log_file = log_dir.join("gamescope_build.log");
            let timestamp = chrono::Utc::now().format("%Y-%m-%d %H:%M:%S UTC");
            let log_entry = format!("[{}] INFO: {}\n", timestamp, message);
            let _ = std::fs::OpenOptions::new()
                .create(true)
                .append(true)
                .open(log_file)
                .and_then(|mut f| std::io::Write::write_all(&mut f, log_entry.as_bytes()));
        }
    }
    
    /// Log an error with context - ensure errors are written to /logs directory
    fn log_error(&self, context: &str, error: &BuilderError) {
        // Write to global logger if available
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_error(
                error,
                "GAMESCOPE_BUILDER",
                context
            );
        }
        
        // Also write to /logs directory
        let log_dir = std::path::Path::new("/logs");
        if let Err(e) = std::fs::create_dir_all(log_dir) {
            eprintln!("Failed to create /logs directory: {}", e);
        } else {
            let log_file = log_dir.join("gamescope_build.log");
            let timestamp = chrono::Utc::now().format("%Y-%m-%d %H:%M:%S UTC");
            let log_entry = format!("[{}] ERROR [{}]: {}\n", timestamp, context, error);
            let _ = std::fs::OpenOptions::new()
                .create(true)
                .append(true)
                .open(log_file)
                .and_then(|mut f| std::io::Write::write_all(&mut f, log_entry.as_bytes()));
        }
    }

    /// Ensure Orange-Pi directory structure exists
    fn ensure_orange_pi_structure(&self) -> Result<(), BuilderError> {
        info!("Checking Orange-Pi directory structure: {}", self.orange_pi_dir.display());
        
        let required_dirs = [
            "gamescope",
            "kernel", 
            "uboot",
            "firmware",
            "desktop",  // Add desktop directory for LXQt packages
        ];
        
        for dir in &required_dirs {
            let dir_path = self.orange_pi_dir.join(dir);
            if !dir_path.exists() {
                info!("Creating directory: {}", dir_path.display());
                fs::create_dir_all(&dir_path)
                    .map_err(|e| BuilderError::IoError(format!("Failed to create {}: {}", dir_path.display(), e)))?;
            }
        }
        
        // Create build directory
        fs::create_dir_all(&self.config.build_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }

    /// Download missing files to Orange-Pi directories
    fn download_missing_files(&self) -> Result<(), BuilderError> {
        info!("Checking for missing files and downloading if needed");
        
        // Check for LXQt desktop packages
        let desktop_dir = self.orange_pi_dir.join("desktop");
        if !self.check_lxqt_packages_exist(&desktop_dir) {
            info!("LXQt packages not found, downloading...");
            self.download_lxqt_packages()?;
        }
        
        // Check for GameScope .deb files
        let gamescope_dir = self.orange_pi_dir.join("gamescope");
        
        // Check if any .deb files exist
        let debs_exist = fs::read_dir(&gamescope_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?
            .any(|entry| {
                if let Ok(entry) = entry {
                    entry.path().extension().and_then(|s| s.to_str()) == Some("deb")
                } else {
                    false
                }
            });
            
        if !debs_exist {
            info!("No GameScope .deb files found, downloading...");
            self.download_gamescope_debs()?;
        }
        
        // Check for GPU drivers
        let gpu_dir = gamescope_dir.join("gpu-drivers");
        if !gpu_dir.exists() {
            info!("GPU drivers not found, downloading...");
            self.download_gpu_drivers()?;
        }
        
        // Check for kernel source
        if !self.kernel_dir.exists() {
            info!("Kernel source not found, downloading...");
            self.download_kernel_source()?;
        }
        
        // Check for U-Boot source
        let uboot_dir = self.orange_pi_dir.join("uboot");
        if !uboot_dir.join("u-boot").exists() {
            info!("U-Boot source not found, downloading...");
            self.download_uboot_source()?;
        }
        
        Ok(())
    }

    /// Check for GameScope packages locally, download from Debian repo if not found
    fn download_gamescope_debs(&self) -> Result<(), BuilderError> {
        info!("Checking for GameScope packages");
        
        let gamescope_dir = self.orange_pi_dir.join("gamescope");
        fs::create_dir_all(&gamescope_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Check if GameScope packages already exist locally
        if self.check_gamescope_packages_exist(&gamescope_dir) {
            info!("GameScope packages found locally in /gamescope folder");
            return Ok(());
        }
        
        info!("GameScope packages not found locally, downloading from Debian repository");
        
        // Download GameScope from Debian repository using apt
        let debian_packages = [
            "gamescope", // Main GameScope package from Debian
        ];
        
        for package_name in &debian_packages {
            info!("Downloading {} from Debian repository", package_name);
            
            // Use apt-get download to get the package from Debian repo
            let output = Command::new("apt-get")
                .args(&["download", package_name])
                .current_dir(&gamescope_dir)
                .output()
                .map_err(|e| BuilderError::DownloadFailed(format!("Failed to download {}: {}", package_name, e)))?;
                
            if !output.status.success() {
                return Err(BuilderError::DownloadFailed(format!(
                    "Failed to download {} from Debian repository: {}", 
                    package_name, 
                    String::from_utf8_lossy(&output.stderr)
                )));
            }
        }
        
        Ok(())
    }
    
    /// Check if GameScope packages exist in local folder
    fn check_gamescope_packages_exist(&self, gamescope_dir: &PathBuf) -> bool {
        if !gamescope_dir.exists() {
            return false;
        }
        
        // Check for GameScope .deb files
        if let Ok(entries) = fs::read_dir(gamescope_dir) {
            let has_gamescope = entries
                .filter_map(|entry| entry.ok())
                .any(|entry| {
                    let filename = entry.file_name().to_string_lossy().to_lowercase();
                    filename.contains("gamescope") && filename.ends_with(".deb")
                });
            
            if has_gamescope {
                info!("Found existing GameScope packages in local folder");
                return true;
            }
        }
        
        false
    }

    /// Check if LXQt packages exist
    fn check_lxqt_packages_exist(&self, desktop_dir: &PathBuf) -> bool {
        if !desktop_dir.exists() {
            return false;
        }
        
        // Check for key LXQt packages
        let required_packages = [
            "lxqt-core",
            "lxqt-config", 
            "lxqt-panel",
            "pcmanfm-qt",
        ];
        
        for package in &required_packages {
            let package_pattern = format!("{}_*.deb", package);
            let has_package = fs::read_dir(desktop_dir)
                .map(|mut entries| {
                    entries.any(|entry| {
                        if let Ok(entry) = entry {
                            entry.file_name().to_string_lossy().starts_with(package)
                                && entry.file_name().to_string_lossy().ends_with(".deb")
                        } else {
                            false
                        }
                    })
                })
                .unwrap_or(false);
                
            if !has_package {
                return false;
            }
        }
        
        true
    }
    
    /// Download LXQt desktop packages
    fn download_lxqt_packages(&self) -> Result<(), BuilderError> {
        info!("Downloading LXQt desktop packages");
        self.log_step("Downloading LXQt desktop environment packages");
        
        let desktop_dir = self.orange_pi_dir.join("desktop");
        fs::create_dir_all(&desktop_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Download LXQt packages from Debian repository
        // Using apt-get download in a chroot would be better, but for now we'll use direct URLs
        let lxqt_packages = [
            "lxqt-core",
            "lxqt-config",
            "lxqt-panel",
            "lxqt-session",
            "lxqt-runner",
            "lxqt-qtplugin",
            "lxqt-notificationd",
            "lxqt-globalkeys",
            "lxqt-about",
            "pcmanfm-qt",
            "lximage-qt",
            "qterminal",
            "qps",
            "screengrab",
            "lxqt-themes",
            "breeze-icon-theme",
            "oxygen-icon-theme",
            "papirus-icon-theme",
        ];
        
        // Create a temporary script to download packages
        let download_script = desktop_dir.join("download_lxqt.sh");
        let script_content = format!(r#"#!/bin/bash
set -e
cd {}

# Download LXQt packages for arm64
for pkg in {}; do
    echo "Downloading $pkg..."
    apt-get download -o APT::Architecture=arm64 "$pkg" || echo "Warning: Failed to download $pkg"
done

echo "LXQt package download completed"
"#, 
            desktop_dir.display(),
            lxqt_packages.join(" ")
        );
        
        fs::write(&download_script, script_content)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        // Make script executable
        let output = Command::new("chmod")
            .args(&["+x", download_script.to_str().unwrap()])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to chmod script: {}", e)))?;
            
        if !output.status.success() {
            warn!("Failed to make download script executable");
        }
        
        // Run the download script
        info!("Running LXQt download script");
        let output = Command::new("bash")
            .arg(download_script.to_str().unwrap())
            .output()
            .map_err(|e| BuilderError::DownloadFailed(format!("Failed to run download script: {}", e)))?;
            
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            let stdout = String::from_utf8_lossy(&output.stdout);
            warn!("LXQt download script had issues:\nSTDERR: {}\nSTDOUT: {}", stderr, stdout);
            // Don't fail completely as some packages might have downloaded
        }
        
        // Clean up script
        let _ = fs::remove_file(&download_script);
        
        info!("LXQt package download completed");
        self.log_step("LXQt package download completed");
        Ok(())
    }
    
    /// Download GPU drivers
    fn download_gpu_drivers(&self) -> Result<(), BuilderError> {
        info!("Checking for Mali G610 GPU drivers");
        
        // Check local /gpu folder first
        let local_gpu_dir = self.orange_pi_dir.join("gpu");
        let gamescope_gpu_dir = self.orange_pi_dir.join("gamescope").join("gpu-drivers");
        
        fs::create_dir_all(&gamescope_gpu_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Check if GPU drivers already exist in local /gpu folder
        if self.check_gpu_drivers_exist(&local_gpu_dir) {
            info!("GPU drivers found locally in /gpu folder, copying to build location");
            self.copy_local_gpu_drivers(&local_gpu_dir, &gamescope_gpu_dir)?;
            return Ok(());
        }
        
        info!("GPU drivers not found locally - GPU drivers must be provided in /gpu folder as .deb packages");
        
        return Err(BuilderError::FileNotFound(local_gpu_dir.clone()));
    }
    
    /// Check if GPU drivers exist in local /gpu folder
    fn check_gpu_drivers_exist(&self, gpu_dir: &PathBuf) -> bool {
        if !gpu_dir.exists() {
            return false;
        }
        
        // Check for Mali driver .deb files
        if let Ok(entries) = fs::read_dir(gpu_dir) {
            let has_mali_driver = entries
                .filter_map(|entry| entry.ok())
                .any(|entry| {
                    let filename = entry.file_name().to_string_lossy().to_lowercase();
                    filename.contains("mali") && filename.ends_with(".deb")
                });
            
            if has_mali_driver {
                info!("Found existing GPU driver .deb packages in local /gpu folder");
                return true;
            }
        }
        
        false
    }
    
    /// Copy GPU drivers from local folder to build location
    fn copy_local_gpu_drivers(&self, src_dir: &PathBuf, dest_dir: &PathBuf) -> Result<(), BuilderError> {
        if let Ok(entries) = fs::read_dir(src_dir) {
            for entry in entries {
                if let Ok(entry) = entry {
                    let filename = entry.file_name().to_string_lossy().to_string();
                    if filename.to_lowercase().contains("mali") && filename.ends_with(".deb") {
                        let src_path = entry.path();
                        let dest_path = dest_dir.join(&filename);
                        
                        info!("Copying GPU driver .deb: {} -> {}", src_path.display(), dest_path.display());
                        fs::copy(&src_path, &dest_path)
                            .map_err(|e| BuilderError::IoError(format!("Failed to copy GPU driver: {}", e)))?;
                    }
                }
            }
        }
        
        Ok(())
    }

    /// Check if kernel source exists, download if needed
    fn download_kernel_source(&self) -> Result<(), BuilderError> {
        info!("Checking kernel source");
        
        let kernel_base_dir = self.orange_pi_dir.join("kernel");
        fs::create_dir_all(&kernel_base_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Check if kernel already exists
        if self.check_kernel_exists() {
            info!("Kernel source already exists locally");
            return Ok(());
        }
        
        info!("Kernel source not found, downloading from Rockchip repository");
        
        match self.config.kernel_choice {
            KernelChoice::Rockchip51 => {
                info!("Downloading Rockchip 5.1 kernel (develop-5.10-rt53 branch)");
                let output = Command::new("git")
                    .args(&[
                        "clone",
                        "-b", "develop-5.10-rt53",
                        "https://github.com/rockchip-linux/kernel.git",
                        self.kernel_dir.to_str().unwrap()
                    ])
                    .output()
                    .map_err(|e| BuilderError::DownloadFailed(format!("Failed to clone kernel: {}", e)))?;
                    
                if !output.status.success() {
                    return Err(BuilderError::DownloadFailed(format!(
                        "Rockchip 5.1 kernel clone failed: {}", 
                        String::from_utf8_lossy(&output.stderr)
                    )));
                }
            },
            KernelChoice::Rockchip61 => {
                info!("Downloading Rockchip 6.1 kernel (develop-6.1 branch)");
                let output = Command::new("git")
                    .args(&[
                        "clone",
                        "-b", "develop-6.1",
                        "https://github.com/rockchip-linux/kernel.git",
                        self.kernel_dir.to_str().unwrap()
                    ])
                    .output()
                    .map_err(|e| BuilderError::DownloadFailed(format!("Failed to clone kernel: {}", e)))?;
                    
                if !output.status.success() {
                    return Err(BuilderError::DownloadFailed(format!(
                        "Rockchip 6.1 kernel clone failed: {}", 
                        String::from_utf8_lossy(&output.stderr)
                    )));
                }
            }
        }
        
        Ok(())
    }
    
    /// Check if kernel source already exists
    fn check_kernel_exists(&self) -> bool {
        // Check if kernel directory exists and has expected files
        if self.kernel_dir.exists() {
            let makefile_path = self.kernel_dir.join("Makefile");
            let kconfig_path = self.kernel_dir.join("Kconfig");
            
            if makefile_path.exists() && kconfig_path.exists() {
                info!("Found existing kernel source at {}", self.kernel_dir.display());
                return true;
            }
        }
        
        false
    }

    /// Download U-Boot source
    fn download_uboot_source(&self) -> Result<(), BuilderError> {
        info!("Downloading U-Boot source");
        
        let uboot_dir = self.orange_pi_dir.join("uboot");
        let uboot_src = uboot_dir.join("u-boot");
        
        let output = Command::new("git")
            .args(&[
                "clone",
                "--depth", "1",
                "--branch", "v2024.01",
                "https://github.com/u-boot/u-boot.git",
                uboot_src.to_str().unwrap()
            ])
            .output()
            .map_err(|e| BuilderError::DownloadFailed(format!("Failed to clone U-Boot: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::DownloadFailed(format!(
                "U-Boot clone failed: {}", 
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        Ok(())
    }

    /// Prepare kernel configuration
    fn prepare_kernel(&self) -> Result<(), BuilderError> {
        info!("Preparing kernel configuration");
        
        match self.config.kernel_choice {
            KernelChoice::Rockchip51 => {
                self.apply_bsp_config()?;
            },
            KernelChoice::Rockchip61 => {
                // Apply gaming optimizations to newer kernel
                self.apply_gaming_config()?;
            }
        }
        
        Ok(())
    }

    /// Apply BSP-specific configuration
    fn apply_bsp_config(&self) -> Result<(), BuilderError> {
        info!("Applying Rockchip BSP configuration");
        
        let defconfig_path = self.kernel_dir.join("arch/arm64/configs/rockchip_linux_defconfig");
        
        let gaming_configs = vec![
            "CONFIG_MALI_DEVFREQ=y",
            "CONFIG_MALI_2MB_ALLOC=y", 
            "CONFIG_MALI_DMA_BUF_MAP_ON_DEMAND=y",
            "CONFIG_MALI_EXPERT=y",
            "CONFIG_MALI_PLATFORM_NAME=\"rk\"",
            "CONFIG_MALI_MEMORY_GROUP_MANAGER=y",
            "CONFIG_TRANSPARENT_HUGEPAGE=y",
            "CONFIG_TRANSPARENT_HUGEPAGE_ALWAYS=y",
            "CONFIG_CMA=y",
            "CONFIG_CMA_SIZE_MBYTES=512",
            "CONFIG_DMA_CMA=y",
            "CONFIG_PM_DEVFREQ=y",
            "CONFIG_DEVFREQ_GOV_PERFORMANCE=y",
            "CONFIG_ARM_ROCKCHIP_DMC_DEVFREQ=y",
            // Gaming-specific optimizations
            "CONFIG_PREEMPT=y",
            "CONFIG_HIGH_RES_TIMERS=y",
            "CONFIG_SCHEDUTIL_DEFAULT=y",
            "CONFIG_CPU_FREQ_GOV_SCHEDUTIL=y",
            "CONFIG_CPU_FREQ_GOV_PERFORMANCE=y",
        ];
        
        let mut file = fs::OpenOptions::new()
            .append(true)
            .open(&defconfig_path)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        for config in gaming_configs {
            writeln!(file, "{}", config)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        Ok(())
    }

    /// Apply gaming configuration to mainline kernel
    fn apply_gaming_config(&self) -> Result<(), BuilderError> {
        info!("Applying gaming optimizations to mainline kernel");
        
        // Create a custom defconfig for gaming
        let defconfig_path = self.kernel_dir.join("arch/arm64/configs/orangepi5plus_gaming_defconfig");
        
        let gaming_defconfig = r#"# Orange Pi 5 Plus Gaming Configuration
CONFIG_ARCH_ROCKCHIP=y
CONFIG_ARM64=y
CONFIG_64BIT=y

# CPU and Performance
CONFIG_PREEMPT=y
CONFIG_HIGH_RES_TIMERS=y
CONFIG_CPU_FREQ=y
CONFIG_CPU_FREQ_GOV_SCHEDUTIL=y
CONFIG_CPU_FREQ_GOV_PERFORMANCE=y
CONFIG_CPU_FREQ_GOV_ONDEMAND=y
CONFIG_SCHEDUTIL_DEFAULT=y

# Memory Management
CONFIG_TRANSPARENT_HUGEPAGE=y
CONFIG_TRANSPARENT_HUGEPAGE_ALWAYS=y
CONFIG_CMA=y
CONFIG_CMA_SIZE_MBYTES=512
CONFIG_DMA_CMA=y

# GPU Support (Panfrost for mainline)
CONFIG_DRM=y
CONFIG_DRM_PANFROST=y
CONFIG_DRM_PANEL_SIMPLE=y

# Gaming optimizations
CONFIG_HZ_1000=y
CONFIG_PREEMPT_RCU=y

# RK3588 specific
CONFIG_ROCKCHIP_PM_DOMAINS=y
CONFIG_ROCKCHIP_IODOMAIN=y
CONFIG_ROCKCHIP_THERMAL=y

# Essential drivers
CONFIG_MMC=y
CONFIG_MMC_DW=y
CONFIG_MMC_DW_ROCKCHIP=y
CONFIG_USB=y
CONFIG_USB_XHCI_HCD=y
CONFIG_USB_DWXC3=y

# Network
CONFIG_ETHERNET=y
CONFIG_STMMAC_ETH=y
CONFIG_DWMAC_ROCKCHIP=y

# Audio
CONFIG_SND=y
CONFIG_SND_SOC=y
CONFIG_SND_SOC_ROCKCHIP=y

# GPIO and pinctrl
CONFIG_PINCTRL_ROCKCHIP=y
CONFIG_GPIO_ROCKCHIP=y
"#;

        fs::write(&defconfig_path, gaming_defconfig)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }

    /// Run debootstrap to create base system
    fn run_debootstrap(&self) -> Result<(), BuilderError> {
        info!("Running debootstrap for Debian 12.11 GameScope system");
        self.log_step("Creating Debian 12.11 base system with debootstrap");
        
        // Remove existing rootfs if it exists
        if self.rootfs_dir.exists() {
            info!("Removing existing rootfs directory");
            fs::remove_dir_all(&self.rootfs_dir)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        fs::create_dir_all(&self.rootfs_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Minimal base system for debootstrap - all other packages will be installed later
        let essential_packages = vec![
            // Core system essentials only
            "systemd", "systemd-sysv", "udev", "dbus", "ca-certificates",
            "locales", "tzdata", "apt-transport-https",
            
            // Basic utilities only
            "sudo", "nano",
        ];
        
        let packages_str = essential_packages.join(",");
        
        self.log_step("Running first stage debootstrap");
        info!("Installing {} packages", essential_packages.len());
        
        // Run first stage debootstrap
        // Create a symlink to avoid issues with spaces in paths
        let temp_rootfs = PathBuf::from("/tmp/gamescope_rootfs");
        if temp_rootfs.exists() {
            let _ = fs::remove_dir_all(&temp_rootfs);
        }
        
        // Create parent directories if needed
        if let Some(parent) = self.rootfs_dir.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| BuilderError::IoError(format!("Failed to create parent directories: {}", e)))?;
        }
        
        let include_arg = format!("--include={}", packages_str);
        let debootstrap_args = vec![
            "--arch", &self.config.arch,
            "--variant=minbase",
            &include_arg,
            "--components=main,contrib,non-free,non-free-firmware",
            &self.config.suite,
            temp_rootfs.to_str().unwrap(),
            &self.config.mirror,
        ];
        
        let output = self.execute_command_with_logging("debootstrap", &debootstrap_args, "debootstrap")?;
            
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            let stdout = String::from_utf8_lossy(&output.stdout);
            
            // Log the full output to a dedicated file for debugging
            let log_dir = std::path::Path::new("/logs");
            if let Ok(_) = std::fs::create_dir_all(log_dir) {
                let debootstrap_log = log_dir.join("debootstrap_full.log");
                let timestamp = chrono::Utc::now().format("%Y-%m-%d %H:%M:%S UTC");
                let full_log = format!(
                    "[{}] Debootstrap failed\n\nCommand: debootstrap --arch {} --variant=minbase --include={} --components=main,contrib,non-free,non-free-firmware {} {} {}\n\nSTDERR:\n{}\n\nSTDOUT:\n{}\n\n",
                    timestamp, self.config.arch, packages_str, self.config.suite, self.rootfs_dir.display(), self.config.mirror,
                    stderr, stdout
                );
                let _ = std::fs::write(&debootstrap_log, &full_log);
            }
            
            let err = BuilderError::BuildFailed(format!(
                "Debootstrap failed:\nSTDERR: {}\nSTDOUT: {}", stderr, stdout
            ));
            self.log_error("debootstrap failed", &err);
            // Clean up temp directory on failure
            let _ = fs::remove_dir_all(&temp_rootfs);
            return Err(err);
        }
        
        // Move temp rootfs to final location
        self.log_step("Moving rootfs to final location");
        info!("Moving {} to {}", temp_rootfs.display(), self.rootfs_dir.display());
        
        // Ensure parent directory exists
        if let Some(parent) = self.rootfs_dir.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| BuilderError::IoError(format!("Failed to create parent directory: {}", e)))?;
        }
        
        // Remove existing rootfs if it exists
        if self.rootfs_dir.exists() {
            fs::remove_dir_all(&self.rootfs_dir)
                .map_err(|e| BuilderError::IoError(format!("Failed to remove existing rootfs: {}", e)))?;
        }
        
        // Use mv command to move the directory (handles cross-device moves better than fs::rename)
        let mv_args = vec![temp_rootfs.to_str().unwrap(), self.rootfs_dir.to_str().unwrap()];
        let mv_output = self.execute_command_with_logging("mv", &mv_args, "mv_rootfs")?;
            
        if !mv_output.status.success() {
            let stderr = String::from_utf8_lossy(&mv_output.stderr);
            return Err(BuilderError::IoError(format!("Failed to move rootfs: {}", stderr)));
        }
        
        self.log_step("Configuring multiarch support for gaming");
        // Enable armhf architecture for Steam compatibility
        let dpkg_arch_file = self.rootfs_dir.join("var/lib/dpkg/arch");
        fs::write(&dpkg_arch_file, "arm64\narmhf\n")
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        info!("Debootstrap completed successfully");
        self.log_step("Debootstrap completed successfully");
        
        // Install armhf packages after base system is ready
        self.install_multiarch_packages()?;
        
        Ok(())
    }
    
    /// Install multiarch packages and Steam after base system is ready
    fn install_multiarch_packages(&self) -> Result<(), BuilderError> {
        info!("Installing multiarch packages for Steam compatibility");
        self.log_step("Installing armhf packages for Steam compatibility");
        
        // Create a chroot script to configure multiarch and install packages
        let chroot_script = self.rootfs_dir.join("tmp/install_multiarch.sh");
        let script_content = r#"#!/bin/bash
set -e

echo "Configuring multiarch support..."
dpkg --add-architecture armhf
apt-get update

echo "Installing armhf packages..."
# Install essential armhf libraries for Steam
apt-get install -y \
    libc6:armhf \
    libstdc++6:armhf \
    zlib1g:armhf \
    libncurses6:armhf \
    libx11-6:armhf \
    libxext6:armhf \
    libxrender1:armhf \
    libgcc-s1:armhf \
    libudev1:armhf \
    libusb-1.0-0:armhf \
    libdbus-1-3:armhf \
    libgl1-mesa-glx:armhf \
    libgl1-mesa-dri:armhf \
    libegl1-mesa:armhf || true

echo "Adding Valve's Steam repository..."
# Add Valve's official Steam repository
wget -qO- https://repo.steampowered.com/steam/archive/stable/steam.gpg | apt-key add - || true
echo "deb [arch=amd64,i386,arm64,armhf] https://repo.steampowered.com/steam/ stable steam" > /etc/apt/sources.list.d/steam.list || true
echo "deb-src [arch=amd64,i386,arm64,armhf] https://repo.steampowered.com/steam/ stable steam" >> /etc/apt/sources.list.d/steam.list || true

# Update again with Steam repository
apt-get update || true

# Try to install Steam
echo "Installing Steam..."
apt-get install -y steam-installer || echo "Steam installation may require manual configuration"

echo "Multiarch setup completed"
"#;
        
        fs::write(&chroot_script, script_content)
            .map_err(|e| BuilderError::IoError(format!("Failed to create multiarch script: {}", e)))?;
        
        // Make script executable
        let chmod_args = vec!["+x", chroot_script.to_str().unwrap()];
        let output = self.execute_command_with_logging("chmod", &chmod_args, "chmod_multiarch_script")?;
            
        if !output.status.success() {
            warn!("Failed to make multiarch script executable");
        }
        
        // Execute the script in chroot
        let chroot_args = vec![
            self.rootfs_dir.to_str().unwrap(),
            "/tmp/install_multiarch.sh"
        ];
        let output = self.execute_command_with_logging("chroot", &chroot_args, "multiarch_install")?;
            
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            let stdout = String::from_utf8_lossy(&output.stdout);
            
            // Log but don't fail - some packages might not be available
            warn!("Some multiarch packages may have failed to install: {}", stderr);
            self.log_step(&format!("Multiarch setup completed with warnings: {}", stderr));
            
            // Log full output for debugging
            let log_dir = std::path::Path::new("/logs");
            if let Ok(_) = std::fs::create_dir_all(log_dir) {
                let multiarch_log = log_dir.join("multiarch_install.log");
                let timestamp = chrono::Utc::now().format("%Y-%m-%d %H:%M:%S UTC");
                let full_log = format!(
                    "[{}] Multiarch installation\n\nSTDOUT:\n{}\n\nSTDERR:\n{}\n\n",
                    timestamp, stdout, stderr
                );
                let _ = std::fs::write(&multiarch_log, &full_log);
            }
        } else {
            info!("Multiarch packages installed successfully");
            self.log_step("Multiarch packages installed successfully");
        }
        
        // Clean up
        let _ = fs::remove_file(&chroot_script);
        
        Ok(())
    }
    
    /// Install multimedia, gaming, and additional packages via apt for proper dependency resolution
    fn install_multimedia_and_gaming_packages(&self) -> Result<(), BuilderError> {
        info!("Installing multimedia, gaming, and additional packages via apt");
        self.log_step("Installing multimedia codecs, gaming apps, and system tools via apt");
        
        // Create a chroot script to install additional packages
        let chroot_script = self.rootfs_dir.join("tmp/install_additional.sh");
        let script_content = r#"#!/bin/bash
set -e

echo "Installing additional packages via apt..."
apt-get update

# Install essential system packages that were moved from debootstrap
echo "Installing essential system packages..."
apt-get install -y \
    keyboard-configuration \
    console-setup \
    network-manager \
    network-manager-gnome \
    openssh-server \
    openssh-client \
    iotop \
    file \
    less \
    grep \
    sed \
    gawk \
    coreutils \
    findutils \
    firmware-linux \
    firmware-linux-nonfree \
    firmware-misc-nonfree \
    bluez \
    bluetooth || echo "Some essential packages failed to install"

# Install Wayland display server and compositors
echo "Installing Wayland display components..."
apt-get install -y \
    wayland-protocols \
    libwayland-client0 \
    libwayland-server0 \
    libwayland-egl1 \
    libwayland-cursor0 \
    weston \
    sway || echo "Some Wayland packages failed to install"

# Install graphics and GPU support
echo "Installing graphics and GPU support..."
apt-get install -y \
    mesa-utils \
    mesa-vulkan-drivers \
    vulkan-tools \
    libdrm2 \
    libgbm1 \
    libegl1-mesa \
    libgl1-mesa-dri \
    libgles2-mesa \
    libvulkan1 \
    mesa-va-drivers \
    mesa-vdpau-drivers || echo "Some graphics packages failed to install"

# Install system performance tools
echo "Installing system performance tools..."
apt-get install -y \
    cpufrequtils \
    powertop \
    stress-ng \
    memtester || echo "Some performance tools failed to install"

# Install audio system
echo "Installing audio system..."
apt-get install -y \
    alsa-utils \
    alsa-tools || echo "Some audio packages failed to install"

# Install development tools
echo "Installing development tools..."
apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    python3 \
    python3-pip \
    gcc \
    g++ \
    make \
    autoconf \
    automake \
    libtool || echo "Some development tools failed to install"

# Install file systems and storage support
echo "Installing file system support..."
apt-get install -y \
    ntfs-3g \
    exfat-fuse \
    dosfstools \
    e2fsprogs \
    btrfs-progs || echo "Some file system packages failed to install"

# Install USB and input device support
echo "Installing USB and input device support..."
apt-get install -y \
    libinput-tools \
    usb-modeswitch || echo "Some input device packages failed to install"

# Install fonts
echo "Installing fonts..."
apt-get install -y \
    fonts-liberation \
    fonts-dejavu-core \
    fonts-noto || echo "Some font packages failed to install"

# Install PulseAudio properly via apt (critical for audio)
echo "Installing PulseAudio audio system..."
apt-get install -y \
    pulseaudio \
    pulseaudio-utils \
    pavucontrol \
    pulseaudio-module-bluetooth \
    pipewire \
    pipewire-pulse \
    pipewire-alsa || echo "Some audio packages failed to install"

# Configure PulseAudio to start automatically
echo "Configuring PulseAudio..."
systemctl --user enable pulseaudio.service || echo "Failed to enable PulseAudio service"
systemctl --user enable pulseaudio.socket || echo "Failed to enable PulseAudio socket"

# Install multimedia codecs and applications via apt
echo "Installing multimedia codecs and applications..."
apt-get install -y \
    ffmpeg \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    libavcodec-extra \
    vlc \
    mpv \
    gstreamer1.0-tools \
    gstreamer1.0-vaapi || echo "Some multimedia packages failed to install"

# Install gaming and performance applications via apt
echo "Installing gaming and performance applications..."
apt-get install -y \
    gamemode \
    mangohud \
    wine \
    winetricks \
    lutris \
    steam-devices \
    joystick \
    jstest-gtk \
    evtest || echo "Some gaming packages failed to install"

# Install X11 compatibility packages (only if LXQt is selected)
echo "Installing X11 compatibility packages for LXQt..."
apt-get install -y \
    xwayland \
    xserver-xorg-core \
    xserver-xorg-video-fbdev \
    xserver-xorg-input-libinput \
    libx11-6 \
    libxext6 \
    libxrender1 \
    libxrandr2 \
    libxi6 \
    libqt5gui5 \
    libqt5widgets5 \
    libqt5x11extras5 \
    lightdm || echo "Some X11 packages failed to install"

# Install additional development and system tools
echo "Installing additional system tools..."
apt-get install -y \
    wget \
    curl \
    git \
    vim \
    htop \
    tree \
    unzip \
    zip \
    rsync \
    screen \
    tmux \
    neofetch || echo "Some system tools failed to install"

# Clean up package cache
echo "Cleaning up package cache..."
apt-get autoremove -y
apt-get autoclean

echo "Additional packages installation completed"
"#;
        
        fs::write(&chroot_script, script_content)
            .map_err(|e| BuilderError::IoError(format!("Failed to create additional packages script: {}", e)))?;
        
        // Make script executable
        let chmod_args = vec!["+x", chroot_script.to_str().unwrap()];
        let _output = self.execute_command_with_logging("chmod", &chmod_args, "chmod_additional_script")?;
        
        // Execute the script in chroot
        let chroot_args = vec![
            self.rootfs_dir.to_str().unwrap(),
            "/tmp/install_additional.sh"
        ];
        let output = self.execute_command_with_logging("chroot", &chroot_args, "additional_packages_install")?;
        
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            // Don't fail the build if some packages fail - log as warning
            warn!("Some additional packages may have failed to install: {}", stderr);
            self.log_step(&format!("Additional packages setup completed with warnings: {}", stderr));
        } else {
            info!("Additional packages installed successfully");
            self.log_step("Additional packages installed successfully");
        }
        
        // Clean up
        let _ = fs::remove_file(&chroot_script);
        
        Ok(())
    }

    /// Build and install kernel
    fn build_kernel(&self) -> Result<(), BuilderError> {
        info!("Building kernel for GameScope");
        
        // Determine defconfig to use
        let defconfig = match self.config.kernel_choice {
            KernelChoice::Rockchip51 => "rockchip_linux_defconfig",
            KernelChoice::Rockchip61 => "orangepi5plus_gaming_defconfig",
        };
        
        // Configure kernel
        let output = Command::new("make")
            .args(&[
                &format!("ARCH={}", self.config.arch),
                &format!("CROSS_COMPILE={}", self.cross_compile),
                defconfig,
            ])
            .current_dir(&self.kernel_dir)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Kernel config failed: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Kernel configuration failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Build kernel with gaming optimizations
        let cflags = "-O3 -march=armv8-a+crc+crypto+simd -mtune=cortex-a76.cortex-a55 -pipe";
        
        let output = Command::new("make")
            .args(&[
                &format!("ARCH={}", self.config.arch),
                &format!("CROSS_COMPILE={}", self.cross_compile),
                &format!("-j{}", self.jobs),
                &format!("CFLAGS={}", cflags),
                "Image", "dtbs", "modules",
            ])
            .current_dir(&self.kernel_dir)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Kernel build failed: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Kernel build failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Install modules
        let modules_install_dir = self.rootfs_dir.join("lib/modules");
        let output = Command::new("make")
            .args(&[
                &format!("ARCH={}", self.config.arch),
                &format!("CROSS_COMPILE={}", self.cross_compile),
                &format!("INSTALL_MOD_PATH={}", self.rootfs_dir.to_str().unwrap()),
                "modules_install",
            ])
            .current_dir(&self.kernel_dir)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Modules install failed: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Modules install failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        info!("Kernel build and installation completed");
        Ok(())
    }

    /// Install LXQt desktop environment from downloaded packages
    fn install_lxqt_desktop(&self) -> Result<(), BuilderError> {
        info!("Installing LXQt desktop environment");
        self.log_step("Installing LXQt desktop packages");
        
        let desktop_dir = self.orange_pi_dir.join("desktop");
        let tmp_packages_dir = self.rootfs_dir.join("tmp/lxqt-packages");
        
        // Create temporary directory in rootfs
        fs::create_dir_all(&tmp_packages_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        // Copy all .deb files from desktop directory to rootfs tmp
        info!("Copying LXQt packages to rootfs");
        for entry in fs::read_dir(&desktop_dir).map_err(|e| BuilderError::IoError(e.to_string()))? {
            let entry = entry.map_err(|e| BuilderError::IoError(e.to_string()))?;
            let path = entry.path();
            
            if path.extension().and_then(|s| s.to_str()) == Some("deb") {
                let dest = tmp_packages_dir.join(entry.file_name());
                fs::copy(&path, &dest)
                    .map_err(|e| BuilderError::IoError(format!("Failed to copy {}: {}", path.display(), e)))?;
            }
        }
        
        // Update apt cache first
        info!("Updating apt cache before LXQt installation");
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "apt-get", "update"
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to update apt cache: {}", e)))?;
            
        if !output.status.success() {
            warn!("apt-get update failed: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        // Install all LXQt packages using dpkg
        info!("Installing LXQt packages with dpkg");
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "sh", "-c",
                "cd /tmp/lxqt-packages && dpkg -i *.deb || apt-get -f install -y"
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to install LXQt packages: {}", e)))?;
            
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            warn!("LXQt installation had issues: {}", stderr);
            
            // Try to fix dependencies
            info!("Attempting to fix dependencies");
            let fix_output = Command::new("chroot")
                .args(&[
                    self.rootfs_dir.to_str().unwrap(),
                    "apt-get", "-f", "install", "-y"
                ])
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to fix dependencies: {}", e)))?;
                
            if !fix_output.status.success() {
                let stderr = String::from_utf8_lossy(&fix_output.stderr);
                self.log_error("Failed to fix LXQt dependencies", &BuilderError::BuildFailed(stderr.to_string()));
            }
        }
        
        // Configure LXQt as default desktop session
        info!("Configuring LXQt as default session");
        let lightdm_conf = r#"[Seat:*]
autologin-user=gamer
autologin-user-timeout=0
user-session=lxqt
"#;
        
        let lightdm_conf_dir = self.rootfs_dir.join("etc/lightdm/lightdm.conf.d");
        fs::create_dir_all(&lightdm_conf_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        fs::write(lightdm_conf_dir.join("50-gamescope-autologin.conf"), lightdm_conf)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Clean up temporary packages
        let _ = fs::remove_dir_all(&tmp_packages_dir);
        
        info!("LXQt desktop installation completed");
        self.log_step("LXQt desktop installation completed");
        Ok(())
    }
    
    /// Install RetroArch emulation frontend
    fn install_retroarch(&self) -> Result<(), BuilderError> {
        info!("Installing RetroArch for GameScope");
        self.log_step("Installing RetroArch emulation frontend");
        
        // Install RetroArch and cores
        let retroarch_packages = vec![
            "retroarch",
            "libretro-beetle-psx",
            "libretro-beetle-psx-hw", 
            "libretro-beetle-vb",
            "libretro-beetle-wswan",
            "libretro-bsnes-mercury-accuracy",
            "libretro-bsnes-mercury-performance",
            "libretro-desmume",
            "libretro-gambatte",
            "libretro-genesisplusgx",
            "libretro-mgba",
            "libretro-mupen64plus",
            "libretro-nestopia",
            "libretro-snes9x",
        ];
        
        let packages_str = retroarch_packages.join(" ");
        
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "apt-get", "install", "-y", "--no-install-recommends"
            ])
            .args(&retroarch_packages)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to install RetroArch: {}", e)))?;
            
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            let err = BuilderError::BuildFailed(format!("RetroArch installation failed: {}", stderr));
            self.log_error("Failed to install RetroArch", &err);
            return Err(err);
        }
        
        // Configure RetroArch for GameScope
        self.configure_retroarch_for_gamescope()?;
        
        info!("RetroArch installation completed");
        self.log_step("RetroArch installation completed");
        Ok(())
    }
    
    /// Configure RetroArch for optimal GameScope integration
    fn configure_retroarch_for_gamescope(&self) -> Result<(), BuilderError> {
        info!("Configuring RetroArch for GameScope");
        
        // Create RetroArch config directory
        let retroarch_config_dir = self.rootfs_dir.join("home/gamer/.config/retroarch");
        fs::create_dir_all(&retroarch_config_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        // Create optimized RetroArch config for GameScope
        let retroarch_config = r#"# RetroArch configuration for GameScope
video_driver = "vulkan"
video_fullscreen = "true"
video_windowed_fullscreen = "false"
video_monitor_index = "0"
video_vsync = "true"

# Performance settings
video_threaded = "true"
video_gpu_screenshot = "false"
video_smooth = "false"

# Audio settings
audio_driver = "pulse"
audio_enable = "true"
audio_latency = "64"

# Input settings
input_driver = "udev"
input_joypad_driver = "udev"

# Wayland specific
video_context_driver = "wayland"
"#;
        
        fs::write(retroarch_config_dir.join("retroarch.cfg"), retroarch_config)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }
    
    /// Install GameScope and related packages
    fn install_gamescope(&self) -> Result<(), BuilderError> {
        info!("Installing GameScope and GPU drivers");
        
        // Install GPU drivers
        let gpu_driver_path = self.orange_pi_dir.join("gamescope/gpu-drivers/libmali-valhall-g610-g13p0-wayland-gbm.so");
        if gpu_driver_path.exists() {
            let lib_dir = self.rootfs_dir.join("usr/lib/aarch64-linux-gnu");
            fs::create_dir_all(&lib_dir)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
                
            fs::copy(&gpu_driver_path, lib_dir.join("libmali.so.1"))
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        // Install GameScope .deb packages
        let gamescope_dir = self.orange_pi_dir.join("gamescope");
        let tmp_gamescope_dir = self.rootfs_dir.join("tmp/gamescope");
        
        // Create temp directory
        fs::create_dir_all(&tmp_gamescope_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        // Copy and install GameScope packages
        for entry in fs::read_dir(&gamescope_dir).map_err(|e| BuilderError::IoError(e.to_string()))? {
            let entry = entry.map_err(|e| BuilderError::IoError(e.to_string()))?;
            let path = entry.path();
            
            if path.extension().and_then(|s| s.to_str()) == Some("deb") {
                let filename = path.file_name().unwrap();
                let dest = tmp_gamescope_dir.join(filename);
                
                // Copy to rootfs
                fs::copy(&path, &dest)
                    .map_err(|e| BuilderError::IoError(format!("Failed to copy GameScope package: {}", e)))?;
                    
                info!("Installing {}", filename.to_string_lossy());
                
                let output = Command::new("chroot")
                    .args(&[
                        self.rootfs_dir.to_str().unwrap(),
                        "dpkg", "-i",
                        &format!("/tmp/gamescope/{}", filename.to_string_lossy()),
                    ])
                    .output()
                    .map_err(|e| BuilderError::BuildFailed(format!("Failed to install GameScope package: {}", e)))?;
                    
                if !output.status.success() {
                    warn!("GameScope package install failed: {}", String::from_utf8_lossy(&output.stderr));
                }
            }
        }
        
        // Clean up
        let _ = fs::remove_dir_all(&tmp_gamescope_dir);
        
        Ok(())
    }

    /// Build U-Boot for SPI flash (not included in image)
    fn build_uboot(&self) -> Result<(), BuilderError> {
        info!("Building U-Boot for SPI flash");
        self.log_step("Building U-Boot for SPI flash (separate from image)");
        
        let uboot_src = self.orange_pi_dir.join("uboot/u-boot");
        let uboot_output_dir = self.config.build_dir.join("uboot_spi");
        
        // Create output directory for SPI files
        fs::create_dir_all(&uboot_output_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Configure U-Boot for Orange Pi 5 Plus
        let output = Command::new("make")
            .args(&[
                &format!("CROSS_COMPILE={}", self.cross_compile),
                "orangepi-5-plus-rk3588s_defconfig",
            ])
            .current_dir(&uboot_src)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("U-Boot config failed: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "U-Boot configuration failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Build U-Boot
        let output = Command::new("make")
            .args(&[
                &format!("CROSS_COMPILE={}", self.cross_compile),
                &format!("-j{}", self.jobs),
            ])
            .current_dir(&uboot_src)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("U-Boot build failed: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "U-Boot build failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Copy U-Boot SPI image to output directory
        let spi_image = uboot_src.join("u-boot-rockchip-spi.bin");
        if spi_image.exists() {
            let dest = uboot_output_dir.join("u-boot-rockchip-spi.bin");
            fs::copy(&spi_image, &dest)
                .map_err(|e| BuilderError::IoError(format!("Failed to copy SPI image: {}", e)))?;
            info!("U-Boot SPI image saved to: {}", dest.display());
        }
        
        info!("U-Boot build completed - ready for SPI flashing");
        self.log_step("U-Boot build completed - SPI image ready at build/uboot_spi/");
        Ok(())
    }

    /// Configure audio system properly 
    fn configure_audio_system(&self) -> Result<(), BuilderError> {
        info!("Configuring audio system");
        self.log_step("Configuring PulseAudio and audio system");
        
        // Create PulseAudio configuration for the user
        let pulseaudio_config = self.rootfs_dir.join("etc/pulse/client.conf");
        let pulseaudio_content = r#"# PulseAudio client configuration
# Enable autospawn
autospawn = yes
# Set default server
default-server = unix:/run/user/1000/pulse/native
"#;
        fs::write(&pulseaudio_config, pulseaudio_content)
            .map_err(|e| BuilderError::IoError(format!("Failed to write PulseAudio config: {}", e)))?;
        
        // Create ALSA configuration to use PulseAudio
        let alsa_config = self.rootfs_dir.join("etc/asound.conf");
        let alsa_content = r#"# ALSA configuration to use PulseAudio
pcm.!default {
    type pulse
}
ctl.!default {
    type pulse
}
"#;
        fs::write(&alsa_config, alsa_content)
            .map_err(|e| BuilderError::IoError(format!("Failed to write ALSA config: {}", e)))?;
        
        // Create systemd user service for PulseAudio
        let systemd_user_dir = self.rootfs_dir.join("etc/systemd/user");
        fs::create_dir_all(&systemd_user_dir)
            .map_err(|e| BuilderError::IoError(format!("Failed to create systemd user directory: {}", e)))?;
        
        // Enable audio group permissions
        let audio_group_script = self.rootfs_dir.join("tmp/setup_audio.sh");
        let audio_script_content = format!(r#"#!/bin/bash
set -e

# Add user to audio groups
usermod -a -G audio,pulse-access,pulse {}

# Set up audio permissions
echo "Setting up audio permissions..."
chmod 666 /dev/snd/* || echo "Audio device permissions setup failed"

# Create PulseAudio directories
mkdir -p /home/{}/.config/pulse
chown {}:{} /home/{}/.config/pulse

echo "Audio system configuration completed"
"#, self.config.username, self.config.username, self.config.username, self.config.username, self.config.username);
        
        fs::write(&audio_group_script, audio_script_content)
            .map_err(|e| BuilderError::IoError(format!("Failed to create audio setup script: {}", e)))?;
        
        // Make script executable and run it
        let chmod_args = vec!["+x", audio_group_script.to_str().unwrap()];
        let _output = self.execute_command_with_logging("chmod", &chmod_args, "chmod_audio_script")?;
        
        let chroot_args = vec![
            self.rootfs_dir.to_str().unwrap(),
            "/tmp/setup_audio.sh"
        ];
        let output = self.execute_command_with_logging("chroot", &chroot_args, "audio_system_setup")?;
        
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            warn!("Audio system setup may have failed: {}", stderr);
        }
        
        // Clean up
        let _ = fs::remove_file(&audio_group_script);
        
        info!("Audio system configuration completed");
        Ok(())
    }
    
    /// Configure GameScope-specific system settings
    fn configure_gamescope_system(&self) -> Result<(), BuilderError> {
        info!("Configuring GameScope Wayland system");
        self.log_step("Configuring GameScope Wayland system");
        
        // Set hostname
        fs::write(self.rootfs_dir.join("etc/hostname"), &self.config.hostname)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Configure for gaming performance
        self.log_step("Configuring gaming performance optimizations");
        let gaming_sysctl = r#"# GameScope Gaming Optimizations
# Memory management for gaming
vm.swappiness=10
vm.dirty_ratio=15
vm.dirty_background_ratio=5
vm.vfs_cache_pressure=50

# Network optimizations for gaming
net.core.rmem_max=16777216
net.core.wmem_max=16777216
net.core.netdev_max_backlog=5000

# CPU scheduling optimizations
kernel.sched_rt_runtime_us=950000
kernel.sched_rt_period_us=1000000
kernel.sched_migration_cost_ns=5000000

# Gaming-specific optimizations
kernel.sched_autogroup_enabled=0
kernel.sched_child_runs_first=1
"#;

        fs::write(self.rootfs_dir.join("etc/sysctl.d/99-gaming.conf"), gaming_sysctl)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Configure Wayland environment
        self.log_step("Configuring Wayland environment");
        self.configure_wayland_environment()?;
        
        // Create GameScope auto-start service based on desktop choice
        self.log_step("Creating GameScope systemd service");
        
        let gamescope_service = match self.config.desktop_choice {
            DesktopChoice::LXQtWithGameScope => {
                // For LXQt, GameScope runs as a Wayland compositor option
                r#"[Unit]
Description=GameScope Wayland Compositor
After=multi-user.target
Wants=multi-user.target

[Service]
Type=simple
User=gamer
Group=gamer
Environment=XDG_RUNTIME_DIR=/run/user/1000
Environment=WAYLAND_DISPLAY=wayland-1
Environment=XDG_SESSION_TYPE=wayland
Environment=GBM_BACKEND=mali
Environment=MALI_GLES_LIB=/usr/lib/aarch64-linux-gnu/libmali.so.1
Environment=LD_LIBRARY_PATH=/usr/lib/aarch64-linux-gnu:$LD_LIBRARY_PATH
ExecStartPre=/bin/mkdir -p /run/user/1000
ExecStartPre=/bin/chown gamer:gamer /run/user/1000
ExecStart=/usr/bin/gamescope -W 1920 -H 1080 -r 60 --adaptive-sync --immediate-flips --wayland-display wayland-1
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
"#
            },
            DesktopChoice::GameScopeRetroArch => {
                // For RetroArch, GameScope launches directly in DRM mode with RetroArch
                r#"[Unit]
Description=GameScope with RetroArch (DRM Mode)
After=multi-user.target
Wants=multi-user.target

[Service]
Type=simple
User=gamer
Group=gamer
PAMName=login
TTYPath=/dev/tty1
StandardInput=tty
StandardOutput=tty
Environment=XDG_RUNTIME_DIR=/run/user/1000
Environment=GBM_BACKEND=mali
Environment=MALI_GLES_LIB=/usr/lib/aarch64-linux-gnu/libmali.so.1
Environment=LD_LIBRARY_PATH=/usr/lib/aarch64-linux-gnu:$LD_LIBRARY_PATH
ExecStartPre=/bin/mkdir -p /run/user/1000
ExecStartPre=/bin/chown gamer:gamer /run/user/1000
ExecStart=/usr/bin/gamescope --drm --prefer-vk-device 0 -W 1920 -H 1080 -r 60 --adaptive-sync --immediate-flips -- retroarch
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
"#
            },
        };

        let systemd_dir = self.rootfs_dir.join("etc/systemd/system");
        fs::create_dir_all(&systemd_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        fs::write(systemd_dir.join("gamescope.service"), gamescope_service)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // For RetroArch mode, configure automatic TTY login
        if matches!(self.config.desktop_choice, DesktopChoice::GameScopeRetroArch) {
            self.log_step("Configuring automatic TTY login for RetroArch mode");
            
            // Create getty override for automatic login
            let getty_override_dir = self.rootfs_dir.join("etc/systemd/system/getty@tty1.service.d");
            fs::create_dir_all(&getty_override_dir)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
                
            let autologin_conf = r#"[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin gamer --noclear %I $TERM
Type=idle
"#;
            
            fs::write(getty_override_dir.join("autologin.conf"), autologin_conf)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
                
            // Disable graphical target for pure console mode
            let disable_graphical = Command::new("chroot")
                .args(&[
                    self.rootfs_dir.to_str().unwrap(),
                    "systemctl", "set-default", "multi-user.target"
                ])
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to set default target: {}", e)))?;
                
            if !disable_graphical.status.success() {
                warn!("Failed to set multi-user target: {}", String::from_utf8_lossy(&disable_graphical.stderr));
            }
        }
        
        // Configure PipeWire for gaming
        self.log_step("Configuring PipeWire audio system for gaming");
        self.configure_pipewire_gaming()?;
        
        // Configure GPU drivers and environment
        self.log_step("Configuring Mali GPU drivers");
        self.configure_gpu_environment()?;
        
        // Create users and set passwords
        self.log_step("Creating users and setting passwords");
        self.create_users()?;
        
        // Configure gaming-specific services
        self.log_step("Configuring gaming-specific services");
        self.configure_gaming_services()?;
        
        info!("GameScope system configuration completed");
        self.log_step("GameScope system configuration completed");
        Ok(())
    }
    
    /// Configure Wayland environment
    fn configure_wayland_environment(&self) -> Result<(), BuilderError> {
        // Configure Wayland environment variables
        let wayland_env = r#"# Wayland environment configuration
export XDG_SESSION_TYPE=wayland
export GDK_BACKEND=wayland
export QT_QPA_PLATFORM=wayland-egl
export SDL_VIDEODRIVER=wayland
export CLUTTER_BACKEND=wayland
export MOZ_ENABLE_WAYLAND=1

# Mali GPU configuration
export GBM_BACKEND=mali
export MALI_GLES_LIB=/usr/lib/aarch64-linux-gnu/libmali.so.1
export LD_LIBRARY_PATH=/usr/lib/aarch64-linux-gnu:$LD_LIBRARY_PATH

# Gaming optimizations
export __GL_SYNC_TO_VBLANK=0
export __GL_YIELD=USLEEP
export MESA_NO_ERROR=1
"#;

        fs::write(self.rootfs_dir.join("etc/environment"), wayland_env)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        // Create profile script for Wayland
        let profile_dir = self.rootfs_dir.join("etc/profile.d");
        fs::create_dir_all(&profile_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        fs::write(profile_dir.join("wayland-gaming.sh"), wayland_env)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }
    
    /// Configure PipeWire audio system for gaming
    fn configure_pipewire_gaming(&self) -> Result<(), BuilderError> {
        // Configure PipeWire as the default audio system
        let pipewire_config = r#"# PipeWire gaming configuration
context.properties = {
    default.clock.rate = 48000
    default.clock.quantum = 1024
    default.clock.min-quantum = 32
    default.clock.max-quantum = 2048
}
"#;

        let config_dir = self.rootfs_dir.join("etc/pipewire");
        fs::create_dir_all(&config_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        fs::write(config_dir.join("pipewire.conf"), pipewire_config)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }
    
    /// Configure GPU environment for Mali G610
    fn configure_gpu_environment(&self) -> Result<(), BuilderError> {
        // Create udev rules for GPU access
        let udev_rules = r#"# Mali GPU access rules
KERNEL=="mali0", MODE="0666", GROUP="render"
KERNEL=="dri/card*", MODE="0666", GROUP="render"
KERNEL=="dri/renderD*", MODE="0666", GROUP="render"
"#;

        let udev_dir = self.rootfs_dir.join("etc/udev/rules.d");
        fs::create_dir_all(&udev_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        fs::write(udev_dir.join("99-mali-gpu.rules"), udev_rules)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }
    
    /// Configure gaming-specific services
    fn configure_gaming_services(&self) -> Result<(), BuilderError> {
        // Enable performance governor on boot
        let performance_service = r#"[Unit]
Description=Set CPU governor to performance for gaming
After=multi-user.target

[Service]
Type=oneshot
ExecStart=/bin/bash -c 'echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor'
RemainAfterExit=true

[Install]
WantedBy=multi-user.target
"#;

        let systemd_dir = self.rootfs_dir.join("etc/systemd/system");
        fs::write(systemd_dir.join("gaming-performance.service"), performance_service)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
        Ok(())
    }

    /// Create users and set passwords
    fn create_users(&self) -> Result<(), BuilderError> {
        info!("Creating users");
        
        // Set root password
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "sh", "-c",
                &format!("echo 'root:{}' | chpasswd", self.config.root_password),
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to set root password: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed("Failed to set root password".to_string()));
        }
        
        // Create gamer user
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "useradd", "-m", "-s", "/bin/bash",
                "-G", "sudo,audio,video,input,render",
                &self.config.username,
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to create user: {}", e)))?;
            
        if !output.status.success() {
            warn!("User creation failed: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        // Set user password
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "sh", "-c",
                &format!("echo '{}:{}' | chpasswd", self.config.username, self.config.password),
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to set user password: {}", e)))?;
            
        if !output.status.success() {
            return Err(BuilderError::BuildFailed("Failed to set user password".to_string()));
        }
        
        Ok(())
    }

    /// Create final image
    fn create_image(&self) -> Result<(), BuilderError> {
        info!("Creating GameScope image: {}", self.config.output_path);
        
        // Implementation would create the final disk image
        // This is a placeholder - you would implement the actual image creation logic here
        
        Ok(())
    }

    /// Write directly to device
    fn write_to_device(&self) -> Result<(), BuilderError> {
        info!("Writing GameScope system directly to device: {}", self.config.output_path);
        
        // Implementation would write directly to the target device
        // This is a placeholder - you would implement the actual device writing logic here
        
        Ok(())
    }

    /// Create GameScopeBuilder from BuildConfig (for UI integration)
    pub fn new(config: super::BuildConfig) -> Result<Self, BuilderError> {
        // Convert BuildConfig to GameScopeConfig
        let suite = match config.distro_version.as_str() {
            "12" | "12.11" => "bookworm",
            "11" => "bullseye",
            _ => "bookworm",
        };

        let desktop_choice = match config.desktop_environment.as_deref() {
            Some("gamescope-lxqt") => DesktopChoice::LXQtWithGameScope,
            Some("gamescope-retroarch") => DesktopChoice::GameScopeRetroArch,
            _ => DesktopChoice::LXQtWithGameScope,
        };

        let kernel_choice = if config.kernel_version.contains("6.1") {
            KernelChoice::Rockchip61
        } else {
            KernelChoice::Rockchip51
        };

        let write_to_device = config.output_path.starts_with("/dev/");
        
        let build_dir = PathBuf::from("build").join(format!("{}-{}", config.distro, config.distro_version));

        let gamescope_config = GameScopeConfig {
            suite: suite.to_string(),
            arch: "arm64".to_string(),
            mirror: "http://deb.debian.org/debian".to_string(),
            build_dir,
            output_path: config.output_path,
            hostname: config.hostname,
            username: config.username,
            password: config.password,
            root_password: config.root_password,
            locale: config.locale,
            timezone: config.timezone,
            kernel_choice,
            desktop_choice,
            write_to_device,
        };

        Self::from_gamescope_config(gamescope_config)
    }

    /// Build desktop distro (LXQt + GameScope)
    pub fn build_desktop_distro(&self) -> Result<(), BuilderError> {
        info!("Building GameScope Desktop distro");
        
        // Ensure we're building the desktop version
        if !matches!(self.config.desktop_choice, DesktopChoice::LXQtWithGameScope) {
            return Err(BuilderError::BuildFailed("Desktop builder called with non-desktop configuration".to_string()));
        }

        // Run the main build process
        self.build()
    }

    /// Build gaming distro (GameScope + RetroArch)
    pub fn build_gaming_distro(&self) -> Result<(), BuilderError> {
        info!("Building GameScope Gaming distro");
        
        // Ensure we're building the gaming version
        if !matches!(self.config.desktop_choice, DesktopChoice::GameScopeRetroArch) {
            return Err(BuilderError::BuildFailed("Gaming builder called with non-gaming configuration".to_string()));
        }

        // Run the main build process
        self.build()
    }
}