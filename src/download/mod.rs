use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;
use log::{info, warn, error};
use crate::error::{BuilderError, Result};

pub struct DownloadManager {
    root_dir: PathBuf,
}

impl DownloadManager {
    pub fn new() -> Result<Self> {
        let root_dir = dirs::home_dir()
            .ok_or_else(|| BuilderError::IoError("Could not find home directory".to_string()))?
            .join("Orange-Pi");
        Ok(Self { root_dir })
    }
    
    /// Create all required download directories
    pub fn create_directory_structure(&self) -> Result<()> {
        let directories = vec![
            "gpu",         // GPU drivers and related files
            "kernel",      // Kernel source and builds
            "linux",       // Linux distribution files
            "desktop",     // Desktop environment files
            "app",         // Application packages
            "uboot",       // U-Boot bootloader files
            "firmware",    // Rockchip firmware binaries
            "tools",       // Development tools
            "gamescope",   // GameScope and related files
            "retroarch",   // RetroArch and cores
            "build-system", // Orange Pi build system
        ];
        
        for dir in directories {
            let path = self.root_dir.join(dir);
            if !path.exists() {
                fs::create_dir_all(&path)
                    .map_err(|e| BuilderError::IoError(format!("Failed to create directory {}: {}", dir, e)))?;
                info!("Created directory: {}", path.display());
            }
        }
        
        info!("Download directory structure created successfully");
        Ok(())
    }
    
    /// Get the path for a specific download category
    pub fn get_download_path(&self, category: DownloadCategory) -> PathBuf {
        let dir_name = match category {
            DownloadCategory::Gpu => "gpu",
            DownloadCategory::Kernel => "kernel",
            DownloadCategory::Linux => "linux",
            DownloadCategory::Desktop => "desktop",
            DownloadCategory::App => "app",
            DownloadCategory::Uboot => "uboot",
            DownloadCategory::Firmware => "firmware",
            DownloadCategory::Tools => "tools",
            DownloadCategory::Gamescope => "gamescope",
            DownloadCategory::RetroArch => "retroarch",
            DownloadCategory::BuildSystem => "build-system",
        };
        
        self.root_dir.join(dir_name)
    }
    
    /// Download files for a specific distribution
    pub fn download_distro_files(&self, distro: &str, version: &str) -> Result<()> {
        info!("Downloading files for {} {}", distro, version);
        
        // Create distro-specific subdirectory
        let linux_dir = self.get_download_path(DownloadCategory::Linux);
        let distro_dir = linux_dir.join(format!("{}-{}", distro, version));
        
        if !distro_dir.exists() {
            fs::create_dir_all(&distro_dir)
                .map_err(|e| BuilderError::IoError(format!("Failed to create distro directory: {}", e)))?;
        }
        
        // TODO: Implement actual download logic
        // For now, just create placeholder files
        let placeholder_files = vec![
            "rootfs.tar.xz",
            "packages.list",
            "sources.list",
        ];
        
        for file in placeholder_files {
            let file_path = distro_dir.join(file);
            if !file_path.exists() {
                fs::write(&file_path, format!("# Placeholder for {} {}\n", distro, version))
                    .map_err(|e| BuilderError::IoError(format!("Failed to create placeholder file: {}", e)))?;
                info!("Created placeholder: {}", file_path.display());
            }
        }
        
        Ok(())
    }
    
    /// Download Armbian Rockchip kernel
    pub fn download_armbian_rockchip_kernel(&self) -> Result<()> {
        info!("Downloading Armbian Rockchip kernel (rk-6.1-rkr5.1)");
        
        let kernel_dir = self.get_download_path(DownloadCategory::Kernel);
        let repo_dir = kernel_dir.join("linux-rockchip");
        
        // Clone or update the repository
        if !repo_dir.exists() {
            info!("Cloning Armbian Rockchip kernel repository...");
            let output = Command::new("git")
                .args(&["clone", "--depth=1", "-b", "rk-6.1-rkr5.1", 
                       "https://github.com/armbian/linux-rockchip.git"])
                .current_dir(&kernel_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                return Err(BuilderError::BuildFailed(format!("Git clone failed: {}", stderr)));
            }
            
            info!("Armbian Rockchip kernel downloaded successfully");
        } else {
            info!("Armbian Rockchip kernel already exists, updating...");
            let output = Command::new("git")
                .args(&["pull", "origin", "rk-6.1-rkr5.1"])
                .current_dir(&repo_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git pull: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                warn!("Git pull failed: {}", stderr);
            } else {
                info!("Armbian Rockchip kernel updated successfully");
            }
        }
        
        Ok(())
    }
    
    /// Download kernel source for a specific version (legacy function)
    pub fn download_kernel(&self, version: &str) -> Result<()> {
        info!("Downloading kernel version {}", version);
        
        // For Orange Pi 5 Plus, we use the Armbian Rockchip kernel
        if version.contains("rockchip") || version.contains("rk3588") {
            return self.download_armbian_rockchip_kernel();
        }
        
        let kernel_dir = self.get_download_path(DownloadCategory::Kernel);
        let version_dir = kernel_dir.join(format!("linux-{}", version));
        
        if !version_dir.exists() {
            fs::create_dir_all(&version_dir)
                .map_err(|e| BuilderError::IoError(format!("Failed to create kernel directory: {}", e)))?;
        }
        
        // TODO: Implement actual kernel download for other versions
        let placeholder_files = vec![
            "Makefile",
            "Kconfig", 
            ".config",
        ];
        
        for file in placeholder_files {
            let file_path = version_dir.join(file);
            if !file_path.exists() {
                fs::write(&file_path, format!("# Linux kernel {} placeholder\n", version))
                    .map_err(|e| BuilderError::IoError(format!("Failed to create placeholder file: {}", e)))?;
                info!("Created placeholder: {}", file_path.display());
            }
        }
        
        Ok(())
    }
    
    /// Download desktop environment files
    pub fn download_desktop_environment(&self, de_name: &str) -> Result<()> {
        info!("Downloading desktop environment: {}", de_name);
        
        let desktop_dir = self.get_download_path(DownloadCategory::Desktop);
        let de_dir = desktop_dir.join(de_name.to_lowercase());
        
        if !de_dir.exists() {
            fs::create_dir_all(&de_dir)
                .map_err(|e| BuilderError::IoError(format!("Failed to create desktop directory: {}", e)))?;
        }
        
        // TODO: Implement actual desktop environment download
        let placeholder_files = vec![
            "packages.list",
            "config.json",
            "theme.conf",
        ];
        
        for file in placeholder_files {
            let file_path = de_dir.join(file);
            if !file_path.exists() {
                fs::write(&file_path, format!("# {} configuration\n", de_name))
                    .map_err(|e| BuilderError::IoError(format!("Failed to create placeholder file: {}", e)))?;
                info!("Created placeholder: {}", file_path.display());
            }
        }
        
        Ok(())
    }
    
    /// Download Rockchip U-Boot
    pub fn download_rockchip_uboot(&self) -> Result<()> {
        info!("Downloading Rockchip U-Boot");
        
        let uboot_dir = self.get_download_path(DownloadCategory::Uboot);
        let repo_dir = uboot_dir.join("u-boot-rockchip");
        
        // Clone or update the repository
        if !repo_dir.exists() {
            info!("Cloning Rockchip U-Boot repository...");
            let output = Command::new("git")
                .args(&["clone", "--depth=1", 
                       "https://github.com/rockchip-linux/u-boot.git", 
                       "u-boot-rockchip"])
                .current_dir(&uboot_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                return Err(BuilderError::BuildFailed(format!("Git clone failed: {}", stderr)));
            }
            
            info!("Rockchip U-Boot downloaded successfully");
        } else {
            info!("Rockchip U-Boot already exists, updating...");
            let output = Command::new("git")
                .args(&["pull", "origin", "master"])
                .current_dir(&repo_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git pull: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                warn!("Git pull failed: {}", stderr);
            } else {
                info!("Rockchip U-Boot updated successfully");
            }
        }
        
        Ok(())
    }
    
    /// Download U-Boot bootloader (legacy function)
    pub fn download_uboot(&self, version: &str) -> Result<()> {
        info!("Downloading U-Boot version {}", version);
        
        // For Orange Pi 5 Plus, we use the Rockchip U-Boot
        if version.contains("rockchip") || version.contains("rk3588") {
            return self.download_rockchip_uboot();
        }
        
        let uboot_dir = self.get_download_path(DownloadCategory::Uboot);
        let version_dir = uboot_dir.join(format!("u-boot-{}", version));
        
        if !version_dir.exists() {
            fs::create_dir_all(&version_dir)
                .map_err(|e| BuilderError::IoError(format!("Failed to create U-Boot directory: {}", e)))?;
        }
        
        // TODO: Implement actual U-Boot download for other versions
        let placeholder_files = vec![
            "u-boot.bin",
            "u-boot.dtb",
            "config.mk",
        ];
        
        for file in placeholder_files {
            let file_path = version_dir.join(file);
            if !file_path.exists() {
                fs::write(&file_path, format!("# U-Boot {} placeholder\n", version))
                    .map_err(|e| BuilderError::IoError(format!("Failed to create placeholder file: {}", e)))?;
                info!("Created placeholder: {}", file_path.display());
            }
        }
        
        Ok(())
    }
    
    /// Download GPU drivers
    pub fn download_gpu_drivers(&self, driver_name: &str) -> Result<()> {
        info!("Downloading GPU drivers: {}", driver_name);
        
        let gpu_dir = self.get_download_path(DownloadCategory::Gpu);
        let driver_dir = gpu_dir.join(driver_name);
        
        if !driver_dir.exists() {
            fs::create_dir_all(&driver_dir)
                .map_err(|e| BuilderError::IoError(format!("Failed to create GPU driver directory: {}", e)))?;
        }
        
        // TODO: Implement actual GPU driver download
        let placeholder_files = vec![
            "driver.ko",
            "firmware.bin",
            "config.txt",
        ];
        
        for file in placeholder_files {
            let file_path = driver_dir.join(file);
            if !file_path.exists() {
                fs::write(&file_path, format!("# {} driver placeholder\n", driver_name))
                    .map_err(|e| BuilderError::IoError(format!("Failed to create placeholder file: {}", e)))?;
                info!("Created placeholder: {}", file_path.display());
            }
        }
        
        Ok(())
    }
    
    /// Download Rockchip firmware binaries
    pub fn download_rockchip_firmware(&self) -> Result<()> {
        info!("Downloading Rockchip firmware binaries");
        
        let firmware_dir = self.get_download_path(DownloadCategory::Firmware);
        let repo_dir = firmware_dir.join("rkbin");
        
        // Clone or update the repository
        if !repo_dir.exists() {
            info!("Cloning Rockchip firmware repository...");
            let output = Command::new("git")
                .args(&["clone", "--depth=1", 
                       "https://github.com/rockchip-linux/rkbin.git"])
                .current_dir(&firmware_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                return Err(BuilderError::BuildFailed(format!("Git clone failed: {}", stderr)));
            }
            
            info!("Rockchip firmware downloaded successfully");
        } else {
            info!("Rockchip firmware already exists, updating...");
            let output = Command::new("git")
                .args(&["pull", "origin", "master"])
                .current_dir(&repo_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git pull: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                warn!("Git pull failed: {}", stderr);
            } else {
                info!("Rockchip firmware updated successfully");
            }
        }
        
        Ok(())
    }
    
    /// Download Rockchip development tools
    pub fn download_rockchip_tools(&self) -> Result<()> {
        info!("Downloading Rockchip development tools");
        
        let tools_dir = self.get_download_path(DownloadCategory::Tools);
        let repo_dir = tools_dir.join("rkdeveloptool");
        
        // Clone or update the repository
        if !repo_dir.exists() {
            info!("Cloning rkdeveloptool repository...");
            let output = Command::new("git")
                .args(&["clone", "--depth=1", 
                       "https://github.com/rockchip-linux/rkdeveloptool.git"])
                .current_dir(&tools_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                return Err(BuilderError::BuildFailed(format!("Git clone failed: {}", stderr)));
            }
            
            info!("rkdeveloptool downloaded successfully");
        } else {
            info!("rkdeveloptool already exists, updating...");
            let output = Command::new("git")
                .args(&["pull", "origin", "master"])
                .current_dir(&repo_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git pull: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                warn!("Git pull failed: {}", stderr);
            } else {
                info!("rkdeveloptool updated successfully");
            }
        }
        
        Ok(())
    }
    
    /// Download Orange Pi build system
    pub fn download_orangepi_build_system(&self) -> Result<()> {
        info!("Downloading Orange Pi build system");
        
        let build_dir = self.get_download_path(DownloadCategory::BuildSystem);
        let repo_dir = build_dir.join("orangepi-build");
        
        // Clone or update the repository
        if !repo_dir.exists() {
            info!("Cloning Orange Pi build system repository...");
            let output = Command::new("git")
                .args(&["clone", "--depth=1", "-b", "next",
                       "https://github.com/orangepi-xunlong/orangepi-build.git"])
                .current_dir(&build_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                return Err(BuilderError::BuildFailed(format!("Git clone failed: {}", stderr)));
            }
            
            info!("Orange Pi build system downloaded successfully");
        } else {
            info!("Orange Pi build system already exists, updating...");
            let output = Command::new("git")
                .args(&["pull", "origin", "next"])
                .current_dir(&repo_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git pull: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                warn!("Git pull failed: {}", stderr);
            } else {
                info!("Orange Pi build system updated successfully");
            }
        }
        
        Ok(())
    }
    
    /// Download RetroArch and cores
    pub fn download_retroarch(&self) -> Result<()> {
        info!("Downloading RetroArch and cores");
        
        let retroarch_dir = self.get_download_path(DownloadCategory::RetroArch);
        
        // Download RetroArch main source
        let retroarch_repo = retroarch_dir.join("RetroArch");
        if !retroarch_repo.exists() {
            info!("Cloning RetroArch repository...");
            let output = Command::new("git")
                .args(&["clone", "--depth=1", 
                       "https://github.com/libretro/RetroArch.git"])
                .current_dir(&retroarch_dir)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                return Err(BuilderError::BuildFailed(format!("Git clone failed: {}", stderr)));
            }
            
            info!("RetroArch downloaded successfully");
        } else {
            info!("RetroArch already exists, updating...");
            let output = Command::new("git")
                .args(&["pull", "origin", "master"])
                .current_dir(&retroarch_repo)
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git pull: {}", e)))?;
            
            if !output.status.success() {
                let stderr = String::from_utf8_lossy(&output.stderr);
                warn!("Git pull failed: {}", stderr);
            } else {
                info!("RetroArch updated successfully");
            }
        }
        
        // Download common libretro cores
        let cores_dir = retroarch_dir.join("libretro-cores");
        fs::create_dir_all(&cores_dir)
            .map_err(|e| BuilderError::IoError(format!("Failed to create cores directory: {}", e)))?;
        
        let cores = vec![
            ("genesis_plus_gx", "https://github.com/libretro/Genesis-Plus-GX.git"),
            ("snes9x", "https://github.com/libretro/snes9x.git"),
            ("nestopia", "https://github.com/libretro/nestopia.git"),
            ("pcsx_rearmed", "https://github.com/libretro/pcsx_rearmed.git"),
            ("mupen64plus", "https://github.com/libretro/mupen64plus-libretro-nx.git"),
        ];
        
        for (core_name, core_url) in cores {
            let core_dir = cores_dir.join(core_name);
            if !core_dir.exists() {
                info!("Downloading RetroArch core: {}", core_name);
                let output = Command::new("git")
                    .args(&["clone", "--depth=1", core_url, core_name])
                    .current_dir(&cores_dir)
                    .output()
                    .map_err(|e| BuilderError::BuildFailed(format!("Failed to execute git clone: {}", e)))?;
                
                if !output.status.success() {
                    let stderr = String::from_utf8_lossy(&output.stderr);
                    warn!("Failed to download core {}: {}", core_name, stderr);
                } else {
                    info!("Core {} downloaded successfully", core_name);
                }
            } else {
                info!("Core {} already exists, skipping...", core_name);
            }
        }
        
        Ok(())
    }
    
    /// Download all required components for Orange Pi 5 Plus
    pub fn download_all_components(&self) -> Result<()> {
        info!("Downloading all required components for Orange Pi 5 Plus");
        
        // Download all components
        self.download_armbian_rockchip_kernel()?;
        self.download_rockchip_uboot()?;
        self.download_rockchip_firmware()?;
        self.download_rockchip_tools()?;
        self.download_orangepi_build_system()?;
        self.download_retroarch()?;
        
        info!("All components downloaded successfully");
        Ok(())
    }
    
    /// Check if required files are already downloaded
    pub fn check_downloads(&self, category: DownloadCategory) -> bool {
        let path = self.get_download_path(category);
        path.exists() && path.read_dir().map(|mut d| d.next().is_some()).unwrap_or(false)
    }
}

#[derive(Debug, Clone, Copy)]
pub enum DownloadCategory {
    Gpu,
    Kernel,
    Linux,
    Desktop,
    App,
    Uboot,
    Firmware,
    Tools,
    Gamescope,
    RetroArch,
    BuildSystem,
}

/// Initialize download system and create directory structure
pub fn init_download_system() -> Result<()> {
    let manager = DownloadManager::new()?;
    manager.create_directory_structure()?;
    Ok(())
}