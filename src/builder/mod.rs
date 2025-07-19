use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::io::Write;
use log::{info, error, warn};
use crate::error::BuilderError;
use cursive::{Cursive, views::{Dialog, TextView, LinearLayout, EditView, SelectView, DummyView, ProgressBar}};
use cursive::traits::*;
use std::sync::{Arc, Mutex};
use std::thread;

pub mod gamescope_builder;


#[derive(Debug, Clone)]
pub struct BuildConfig {
    pub distro: String,
    pub distro_version: String,
    pub kernel_version: String,
    pub desktop_environment: Option<String>,
    pub gpu_driver: Option<String>,
    pub bootloader: String,
    pub hostname: String,
    pub username: String,
    pub password: String,
    pub root_password: String,
    pub locale: String,
    pub timezone: String,
    pub packages: Vec<String>,
    pub image_size_gb: u32,
    pub output_path: String,
}

pub struct Builder {
    config: BuildConfig,
    work_dir: PathBuf,
    rootfs_dir: PathBuf,
    boot_dir: PathBuf,
}

impl Builder {
    pub fn new(config: BuildConfig) -> Result<Self, BuilderError> {
        let work_dir = PathBuf::from(format!("build/{}-{}", config.distro, config.distro_version));
        let rootfs_dir = work_dir.join("rootfs");
        let boot_dir = work_dir.join("boot");
        
        Ok(Self {
            config,
            work_dir,
            rootfs_dir,
            boot_dir,
        })
    }
    
    pub fn build(&self) -> Result<(), BuilderError> {
        info!("Starting build process for {} {}", self.config.distro, self.config.distro_version);
        
        // Create working directories
        self.create_directories()?;
        
        // Create base system with debootstrap
        self.create_base_system()?;
        
        // Configure the system
        self.configure_system()?;
        
        // Install kernel
        self.install_kernel()?;
        
        // Install desktop environment or server packages
        self.install_packages()?;
        
        // Install bootloader
        self.install_bootloader()?;
        
        // Generate device tree
        self.generate_device_tree()?;
        
        // Create image or write to device
        if self.config.output_path.starts_with("/dev/") {
            self.write_to_device()?;
        } else {
            self.create_image()?;
        }
        
        info!("Build completed successfully!");
        Ok(())
    }
    
    fn create_directories(&self) -> Result<(), BuilderError> {
        info!("Creating build directories");
        
        fs::create_dir_all(&self.work_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        fs::create_dir_all(&self.rootfs_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        fs::create_dir_all(&self.boot_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        Ok(())
    }
    
    fn create_base_system(&self) -> Result<(), BuilderError> {
        info!("Creating base system with debootstrap");
        
        let suite = match self.config.distro.as_str() {
            "ubuntu" => match self.config.distro_version.as_str() {
                "22.04" => "jammy",
                "24.04" => "noble",
                "25.04" => "plucky",
                _ => return Err(BuilderError::BuildFailed("Unknown Ubuntu version".to_string())),
            },
            "debian" => match self.config.distro_version.as_str() {
                "11" => "bullseye",
                "12" => "bookworm",
                "13" => "trixie",
                _ => return Err(BuilderError::BuildFailed("Unknown Debian version".to_string())),
            },
            _ => return Err(BuilderError::BuildFailed("Unsupported distribution".to_string())),
        };
        
        let mirror = match self.config.distro.as_str() {
            "ubuntu" => "http://ports.ubuntu.com/ubuntu-ports",
            "debian" => "http://deb.debian.org/debian",
            _ => return Err(BuilderError::BuildFailed("Unknown mirror".to_string())),
        };
        
        // Run debootstrap
        let output = Command::new("debootstrap")
            .args(&[
                "--arch=arm64",
                "--foreign",
                "--include=ca-certificates,locales,sudo,systemd",
                suite,
                self.rootfs_dir.to_str().unwrap(),
                mirror,
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to run debootstrap: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Debootstrap failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Copy qemu-aarch64-static for second stage
        let qemu_static = "/usr/bin/qemu-aarch64-static";
        if Path::new(qemu_static).exists() {
            let dest = self.rootfs_dir.join("usr/bin/qemu-aarch64-static");
            fs::copy(qemu_static, dest)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        // Run second stage debootstrap
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "/debootstrap/debootstrap",
                "--second-stage",
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to run second stage: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Second stage debootstrap failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        Ok(())
    }
    
    fn configure_system(&self) -> Result<(), BuilderError> {
        info!("Configuring system");
        
        // Set hostname
        let hostname_path = self.rootfs_dir.join("etc/hostname");
        fs::write(&hostname_path, &self.config.hostname)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Set hosts file
        let hosts_content = format!(
            "127.0.0.1\tlocalhost\n127.0.1.1\t{}\n",
            self.config.hostname
        );
        let hosts_path = self.rootfs_dir.join("etc/hosts");
        fs::write(&hosts_path, hosts_content)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Configure locale
        self.configure_locale()?;
        
        // Configure timezone
        self.configure_timezone()?;
        
        // Create users
        self.create_users()?;
        
        // Configure network
        self.configure_network()?;
        
        // Configure apt sources
        self.configure_apt()?;
        
        Ok(())
    }
    
    fn configure_locale(&self) -> Result<(), BuilderError> {
        info!("Configuring locale: {}", self.config.locale);
        
        // Generate locale
        let locale_gen_path = self.rootfs_dir.join("etc/locale.gen");
        let mut locale_gen = fs::OpenOptions::new()
            .append(true)
            .open(&locale_gen_path)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        writeln!(locale_gen, "{} UTF-8", self.config.locale.replace(".UTF-8", ""))
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Run locale-gen in chroot
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "locale-gen",
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to generate locale: {}", e)))?;
        
        if !output.status.success() {
            warn!("locale-gen failed: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        // Set default locale
        let default_locale = format!("LANG={}\n", self.config.locale);
        let locale_path = self.rootfs_dir.join("etc/default/locale");
        fs::write(&locale_path, default_locale)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        Ok(())
    }
    
    fn configure_timezone(&self) -> Result<(), BuilderError> {
        info!("Configuring timezone: {}", self.config.timezone);
        
        // Set timezone
        let tz_path = self.rootfs_dir.join("etc/timezone");
        fs::write(&tz_path, &self.config.timezone)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Create symlink
        let zoneinfo_path = format!("/usr/share/zoneinfo/{}", self.config.timezone);
        let localtime_path = self.rootfs_dir.join("etc/localtime");
        
        if localtime_path.exists() {
            fs::remove_file(&localtime_path)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        // Run in chroot to create proper symlink
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "ln",
                "-sf",
                &zoneinfo_path,
                "/etc/localtime",
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to set timezone: {}", e)))?;
        
        if !output.status.success() {
            warn!("Failed to create timezone symlink: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        Ok(())
    }
    
    fn create_users(&self) -> Result<(), BuilderError> {
        info!("Creating users");
        
        // Set root password
        let root_pw_cmd = format!("echo 'root:{}' | chpasswd", self.config.root_password);
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "sh",
                "-c",
                &root_pw_cmd,
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to set root password: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Failed to set root password: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Create user
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "useradd",
                "-m",
                "-s",
                "/bin/bash",
                "-G",
                "sudo,audio,video,plugdev",
                &self.config.username,
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to create user: {}", e)))?;
        
        if !output.status.success() {
            // User might already exist
            warn!("useradd failed: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        // Set user password
        let user_pw_cmd = format!("echo '{}:{}' | chpasswd", self.config.username, self.config.password);
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "sh",
                "-c",
                &user_pw_cmd,
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to set user password: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Failed to set user password: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        Ok(())
    }
    
    fn configure_network(&self) -> Result<(), BuilderError> {
        info!("Configuring network");
        
        // Create systemd network configuration
        let network_dir = self.rootfs_dir.join("etc/systemd/network");
        fs::create_dir_all(&network_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Ethernet configuration
        let eth_config = r#"[Match]
Name=eth*

[Network]
DHCP=yes
"#;
        let eth_path = network_dir.join("20-wired.network");
        fs::write(&eth_path, eth_config)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Enable systemd-networkd and systemd-resolved
        let services = ["systemd-networkd", "systemd-resolved"];
        for service in &services {
            let output = Command::new("chroot")
                .args(&[
                    self.rootfs_dir.to_str().unwrap(),
                    "systemctl",
                    "enable",
                    service,
                ])
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to enable {}: {}", service, e)))?;
            
            if !output.status.success() {
                warn!("Failed to enable {}: {}", service, String::from_utf8_lossy(&output.stderr));
            }
        }
        
        Ok(())
    }
    
    fn configure_apt(&self) -> Result<(), BuilderError> {
        info!("Configuring APT sources");
        
        let sources_content = match self.config.distro.as_str() {
            "ubuntu" => {
                let suite = match self.config.distro_version.as_str() {
                    "22.04" => "jammy",
                    "24.04" => "noble",
                    "25.04" => "plucky",
                    _ => "jammy",
                };
                format!(
                    "deb http://ports.ubuntu.com/ubuntu-ports {} main restricted universe multiverse\n\
                     deb http://ports.ubuntu.com/ubuntu-ports {}-updates main restricted universe multiverse\n\
                     deb http://ports.ubuntu.com/ubuntu-ports {}-security main restricted universe multiverse\n",
                    suite, suite, suite
                )
            },
            "debian" => {
                let suite = match self.config.distro_version.as_str() {
                    "11" => "bullseye",
                    "12" => "bookworm",
                    "13" => "trixie",
                    _ => "bookworm",
                };
                format!(
                    "deb http://deb.debian.org/debian {} main contrib non-free\n\
                     deb http://deb.debian.org/debian {}-updates main contrib non-free\n\
                     deb http://security.debian.org/debian-security {}-security main contrib non-free\n",
                    suite, suite, suite
                )
            },
            _ => return Err(BuilderError::BuildFailed("Unknown distribution".to_string())),
        };
        
        let sources_path = self.rootfs_dir.join("etc/apt/sources.list");
        fs::write(&sources_path, sources_content)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Update package lists
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "apt-get",
                "update",
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to update apt: {}", e)))?;
        
        if !output.status.success() {
            warn!("apt-get update failed: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        Ok(())
    }
    
    fn install_kernel(&self) -> Result<(), BuilderError> {
        info!("Installing kernel {}", self.config.kernel_version);
        
        // Copy kernel files from download directory
        let kernel_dir = PathBuf::from(format!("kernel/{}", self.config.kernel_version));
        if !kernel_dir.exists() {
            return Err(BuilderError::BuildFailed(format!("Kernel {} not found in download directory", self.config.kernel_version)));
        }
        
        // Copy kernel image
        let kernel_image = kernel_dir.join("Image");
        if kernel_image.exists() {
            let dest = self.boot_dir.join("Image");
            fs::copy(&kernel_image, &dest)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        // Copy device tree files
        let dtb_dir = kernel_dir.join("dtbs");
        if dtb_dir.exists() {
            let dest_dtb = self.boot_dir.join("dtbs");
            fs::create_dir_all(&dest_dtb)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
            // Copy all dtb files
            for entry in fs::read_dir(&dtb_dir).map_err(|e| BuilderError::IoError(e.to_string()))? {
                let entry = entry.map_err(|e| BuilderError::IoError(e.to_string()))?;
                let path = entry.path();
                if path.extension().and_then(|s| s.to_str()) == Some("dtb") {
                    let dest = dest_dtb.join(entry.file_name());
                    fs::copy(&path, &dest)
                        .map_err(|e| BuilderError::IoError(e.to_string()))?;
                }
            }
        }
        
        // Install kernel modules
        let modules_dir = kernel_dir.join("modules");
        if modules_dir.exists() {
            let dest_modules = self.rootfs_dir.join("lib/modules");
            fs::create_dir_all(&dest_modules)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
            
            // Copy modules
            self.copy_dir_recursive(&modules_dir, &dest_modules)?;
        }
        
        Ok(())
    }
    
    fn install_packages(&self) -> Result<(), BuilderError> {
        info!("Installing packages");
        
        // Base packages to install
        let mut packages = vec![
            "linux-firmware",
            "network-manager",
            "ssh",
            "curl",
            "wget",
            "vim",
            "htop",
            "build-essential",
        ];
        
        // Add desktop environment packages
        if let Some(de) = &self.config.desktop_environment {
            match de.as_str() {
                "lxqt" => {
                    packages.extend(&["lxqt", "sddm", "firefox", "xorg"]);
                },
                "gnome" => {
                    packages.extend(&["gnome-shell", "gdm3", "gnome-terminal", "firefox"]);
                },
                "kde" => {
                    packages.extend(&["kde-plasma-desktop", "sddm", "firefox", "konsole"]);
                },
                "xfce" => {
                    packages.extend(&["xfce4", "lightdm", "firefox", "xfce4-terminal"]);
                },
                "server-minimal" => {
                    // No GUI packages
                },
                "server" => {
                    packages.extend(&["python3", "perl", "git"]);
                },
                "server-full" => {
                    packages.extend(&["python3", "perl", "git", "gcc", "g++", "make"]);
                },
                _ => {},
            }
        }
        
        // Add user-selected packages
        for pkg in &self.config.packages {
            packages.push(pkg.as_str());
        }
        
        // Install packages
        let output = Command::new("chroot")
            .args(&[
                self.rootfs_dir.to_str().unwrap(),
                "apt-get",
                "install",
                "-y",
                "--no-install-recommends",
            ])
            .args(&packages)
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to install packages: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Package installation failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Install GPU driver if specified
        if let Some(gpu) = &self.config.gpu_driver {
            self.install_gpu_driver(gpu)?;
        }
        
        Ok(())
    }
    
    fn install_gpu_driver(&self, driver: &str) -> Result<(), BuilderError> {
        info!("Installing GPU driver: {}", driver);
        
        let driver_dir = PathBuf::from(format!("gpu/proprietary/mali-g610/{}", driver));
        if !driver_dir.exists() {
            warn!("GPU driver {} not found, skipping", driver);
            return Ok(());
        }
        
        // Copy driver libraries
        let lib_dir = self.rootfs_dir.join("usr/lib/aarch64-linux-gnu");
        fs::create_dir_all(&lib_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        for entry in fs::read_dir(&driver_dir).map_err(|e| BuilderError::IoError(e.to_string()))? {
            let entry = entry.map_err(|e| BuilderError::IoError(e.to_string()))?;
            let path = entry.path();
            if path.extension().and_then(|s| s.to_str()) == Some("so") {
                let dest = lib_dir.join(entry.file_name());
                fs::copy(&path, &dest)
                    .map_err(|e| BuilderError::IoError(e.to_string()))?;
            }
        }
        
        Ok(())
    }
    
    fn install_bootloader(&self) -> Result<(), BuilderError> {
        info!("Installing bootloader: {}", self.config.bootloader);
        
        // Create boot configuration
        let boot_cmd = format!(
            "setenv bootargs console=ttyS2,1500000 root=/dev/mmcblk0p2 rootwait rw\n\
             load mmc 0:1 ${{kernel_addr_r}} /boot/Image\n\
             load mmc 0:1 ${{fdt_addr_r}} /boot/dtbs/rockchip/rk3588s-orangepi-5-plus.dtb\n\
             booti ${{kernel_addr_r}} - ${{fdt_addr_r}}\n"
        );
        
        let boot_scr_path = self.boot_dir.join("boot.cmd");
        fs::write(&boot_scr_path, boot_cmd)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Compile boot script
        let output = Command::new("mkimage")
            .args(&[
                "-C", "none",
                "-A", "arm64",
                "-T", "script",
                "-d", boot_scr_path.to_str().unwrap(),
                self.boot_dir.join("boot.scr").to_str().unwrap(),
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to create boot script: {}", e)))?;
        
        if !output.status.success() {
            warn!("mkimage failed: {}", String::from_utf8_lossy(&output.stderr));
        }
        
        Ok(())
    }
    
    fn generate_device_tree(&self) -> Result<(), BuilderError> {
        info!("Generating device tree");
        
        // Use the DeviceTreeManager to generate appropriate DTS
        let dt_config = crate::devicetree::DeviceTreeConfig {
            build_type: crate::devicetree::BuildType::DesktopServer,
            kernel_version: self.config.kernel_version.clone(),
            distro_name: self.config.distro.clone(),
            distro_version: self.config.distro_version.clone(),
            gpu_driver: self.config.gpu_driver.clone().unwrap_or_else(|| "g13p0".to_string()),
            enable_av1: true,
            enable_gpu_oc: false,
            target_freq_mhz: 1000,
        };
        
        let dt_manager = crate::devicetree::DeviceTreeManager::new();
        let dts_name = dt_manager.generate_dts(&dt_config)?;
        
        // Compile the DTS to DTB
        let dts_path = PathBuf::from("kernel/devicetree").join(&dts_name);
        let dtb_path = self.boot_dir.join("dtbs/rockchip/rk3588s-orangepi-5-plus.dtb");
        
        fs::create_dir_all(dtb_path.parent().unwrap())
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        let output = Command::new("dtc")
            .args(&[
                "-I", "dts",
                "-O", "dtb",
                "-o", dtb_path.to_str().unwrap(),
                dts_path.to_str().unwrap(),
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to compile device tree: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Device tree compilation failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        Ok(())
    }
    
    fn create_image(&self) -> Result<(), BuilderError> {
        info!("Creating disk image: {}", self.config.output_path);
        
        // Create image file
        let image_size_mb = self.config.image_size_gb * 1024;
        let output = Command::new("dd")
            .args(&[
                "if=/dev/zero",
                &format!("of={}", self.config.output_path),
                "bs=1M",
                &format!("count={}", image_size_mb),
            ])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to create image: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Image creation failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Create partitions
        self.create_partitions(&self.config.output_path)?;
        
        // Setup loop device
        let output = Command::new("losetup")
            .args(&["-P", "-f", "--show", &self.config.output_path])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to setup loop device: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Loop device setup failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        let loop_device = String::from_utf8_lossy(&output.stdout).trim().to_string();
        
        // Format partitions
        self.format_partitions(&loop_device)?;
        
        // Mount and copy files
        self.copy_to_image(&loop_device)?;
        
        // Cleanup loop device
        let _ = Command::new("losetup")
            .args(&["-d", &loop_device])
            .output();
        
        info!("Image created successfully: {}", self.config.output_path);
        Ok(())
    }
    
    fn create_partitions(&self, image_path: &str) -> Result<(), BuilderError> {
        info!("Creating partitions");
        
        // Create GPT partition table
        let parted_cmds = vec![
            "mklabel gpt",
            "mkpart primary fat32 16MiB 528MiB",
            "mkpart primary ext4 528MiB 100%",
            "set 1 boot on",
        ];
        
        for cmd in parted_cmds {
            let output = Command::new("parted")
                .args(&["-s", image_path, cmd])
                .output()
                .map_err(|e| BuilderError::BuildFailed(format!("Failed to run parted: {}", e)))?;
            
            if !output.status.success() {
                return Err(BuilderError::BuildFailed(format!(
                    "Parted command '{}' failed: {}",
                    cmd,
                    String::from_utf8_lossy(&output.stderr)
                )));
            }
        }
        
        Ok(())
    }
    
    fn format_partitions(&self, loop_device: &str) -> Result<(), BuilderError> {
        info!("Formatting partitions");
        
        // Format boot partition (FAT32)
        let boot_part = format!("{}p1", loop_device);
        let output = Command::new("mkfs.vfat")
            .args(&["-F", "32", "-n", "BOOT", &boot_part])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to format boot partition: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Boot partition format failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Format root partition (ext4)
        let root_part = format!("{}p2", loop_device);
        let output = Command::new("mkfs.ext4")
            .args(&["-L", "rootfs", &root_part])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to format root partition: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Root partition format failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        Ok(())
    }
    
    fn copy_to_image(&self, loop_device: &str) -> Result<(), BuilderError> {
        info!("Copying files to image");
        
        let mount_dir = PathBuf::from("/tmp/opi_mount");
        fs::create_dir_all(&mount_dir)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Mount root partition
        let root_part = format!("{}p2", loop_device);
        let output = Command::new("mount")
            .args(&[&root_part, mount_dir.to_str().unwrap()])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to mount root partition: {}", e)))?;
        
        if !output.status.success() {
            return Err(BuilderError::BuildFailed(format!(
                "Root partition mount failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Copy rootfs
        info!("Copying rootfs...");
        self.copy_dir_recursive(&self.rootfs_dir, &mount_dir)?;
        
        // Create boot directory and mount boot partition
        let boot_mount = mount_dir.join("boot");
        fs::create_dir_all(&boot_mount)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        let boot_part = format!("{}p1", loop_device);
        let output = Command::new("mount")
            .args(&[&boot_part, boot_mount.to_str().unwrap()])
            .output()
            .map_err(|e| BuilderError::BuildFailed(format!("Failed to mount boot partition: {}", e)))?;
        
        if !output.status.success() {
            let _ = Command::new("umount").args(&[mount_dir.to_str().unwrap()]).output();
            return Err(BuilderError::BuildFailed(format!(
                "Boot partition mount failed: {}",
                String::from_utf8_lossy(&output.stderr)
            )));
        }
        
        // Copy boot files
        info!("Copying boot files...");
        self.copy_dir_recursive(&self.boot_dir, &boot_mount)?;
        
        // Create fstab
        let fstab_content = "# <file system> <mount point> <type> <options> <dump> <pass>\n\
                            /dev/mmcblk0p2 / ext4 defaults,noatime 0 1\n\
                            /dev/mmcblk0p1 /boot vfat defaults 0 2\n";
        let fstab_path = mount_dir.join("etc/fstab");
        fs::write(&fstab_path, fstab_content)
            .map_err(|e| BuilderError::IoError(e.to_string()))?;
        
        // Unmount partitions
        let _ = Command::new("umount").args(&[boot_mount.to_str().unwrap()]).output();
        let _ = Command::new("umount").args(&[mount_dir.to_str().unwrap()]).output();
        
        // Remove mount directory
        let _ = fs::remove_dir_all(&mount_dir);
        
        Ok(())
    }
    
    fn write_to_device(&self) -> Result<(), BuilderError> {
        info!("Writing directly to device: {}", self.config.output_path);
        
        // This would use the realtime writer to write directly to the device
        // For safety, we'll just return an error for now
        Err(BuilderError::BuildFailed("Direct device writing not implemented yet".to_string()))
    }
    
    fn copy_dir_recursive(&self, src: &Path, dst: &Path) -> Result<(), BuilderError> {
        if !dst.exists() {
            fs::create_dir_all(dst)
                .map_err(|e| BuilderError::IoError(e.to_string()))?;
        }
        
        for entry in fs::read_dir(src).map_err(|e| BuilderError::IoError(e.to_string()))? {
            let entry = entry.map_err(|e| BuilderError::IoError(e.to_string()))?;
            let path = entry.path();
            let file_name = entry.file_name();
            let dest_path = dst.join(&file_name);
            
            if path.is_dir() {
                self.copy_dir_recursive(&path, &dest_path)?;
            } else {
                fs::copy(&path, &dest_path)
                    .map_err(|e| BuilderError::IoError(e.to_string()))?;
            }
        }
        
        Ok(())
    }
}

/// Show GameScope Desktop Builder UI
pub fn show_gamescope_desktop_builder(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    // Header
    layout.add_child(TextView::new("GameScope Desktop Distro Builder"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("This will build Debian 12.11 with LXQt desktop + GameScope"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Configuration fields
    layout.add_child(TextView::new("Hostname:"));
    layout.add_child(EditView::new()
        .content("orangepi-gamescope")
        .with_name("hostname")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Username:"));
    layout.add_child(EditView::new()
        .content("gamer")
        .with_name("username")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Password:"));
    layout.add_child(EditView::new()
        .content("")
        .secret()
        .with_name("password")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Root Password:"));
    layout.add_child(EditView::new()
        .content("")
        .secret()
        .with_name("root_password")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Output Path (image file or device):"));
    layout.add_child(EditView::new()
        .content("orangepi-gamescope-desktop.img")
        .with_name("output_path")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Image Size (GB):"));
    layout.add_child(EditView::new()
        .content("16")
        .with_name("image_size")
        .fixed_width(10));
    
    let dialog = Dialog::around(layout)
        .title("GameScope Desktop Builder")
        .button("Build", |s| {
            build_gamescope_desktop(s);
        })
        .button("Cancel", |s| {
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
}

/// Show GameScope Gaming Builder UI
pub fn show_gamescope_gaming_builder(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    // Header
    layout.add_child(TextView::new("GameScope RetroArch Distro Builder"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("This will build Debian 12.11 minimal + GameScope + RetroArch"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Configuration fields
    layout.add_child(TextView::new("Hostname:"));
    layout.add_child(EditView::new()
        .content("orangepi-retroarch")
        .with_name("hostname")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Username:"));
    layout.add_child(EditView::new()
        .content("retro")
        .with_name("username")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Password:"));
    layout.add_child(EditView::new()
        .content("")
        .secret()
        .with_name("password")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Root Password:"));
    layout.add_child(EditView::new()
        .content("")
        .secret()
        .with_name("root_password")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Output Path (image file or device):"));
    layout.add_child(EditView::new()
        .content("orangepi-gamescope-retroarch.img")
        .with_name("output_path")
        .fixed_width(40));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Image Size (GB):"));
    layout.add_child(EditView::new()
        .content("8")
        .with_name("image_size")
        .fixed_width(10));
    layout.add_child(DummyView.fixed_height(1));
    
    // RetroArch cores selection
    layout.add_child(TextView::new("Select RetroArch cores to include:"));
    layout.add_child(TextView::new("[X] Genesis Plus GX (Sega Genesis/Mega Drive)"));
    layout.add_child(TextView::new("[X] Snes9x (Super Nintendo)"));
    layout.add_child(TextView::new("[X] Nestopia (Nintendo Entertainment System)"));
    layout.add_child(TextView::new("[X] PCSX ReARMed (PlayStation 1)"));
    layout.add_child(TextView::new("[X] Mupen64Plus (Nintendo 64)"));
    
    let dialog = Dialog::around(layout)
        .title("GameScope RetroArch Builder")
        .button("Build", |s| {
            build_gamescope_retroarch(s);
        })
        .button("Cancel", |s| {
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
}

/// Build GameScope Desktop distro
fn build_gamescope_desktop(siv: &mut Cursive) {
    // Get values from UI
    let hostname = siv.call_on_name("hostname", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "orangepi-gamescope".to_string());
    
    let username = siv.call_on_name("username", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "gamer".to_string());
    
    let password = siv.call_on_name("password", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "changeme".to_string());
    
    let root_password = siv.call_on_name("root_password", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "root".to_string());
    
    let output_path = siv.call_on_name("output_path", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "orangepi-gamescope-desktop.img".to_string());
    
    let image_size: u32 = siv.call_on_name("image_size", |view: &mut EditView| {
        view.get_content().parse().unwrap_or(16)
    }).unwrap_or(16);
    
    // Validate inputs
    if password.is_empty() || root_password.is_empty() {
        siv.add_layer(Dialog::text("Password fields cannot be empty!")
            .title("Validation Error")
            .button("OK", |s| { s.pop_layer(); }));
        return;
    }
    
    // Create build configuration for desktop distro
    let config = BuildConfig {
        distro: "debian".to_string(),
        distro_version: "12.11".to_string(),
        kernel_version: "rockchip-rk3588".to_string(),
        desktop_environment: Some("gamescope-lxqt".to_string()),
        gpu_driver: Some("mali-g610-g13p0".to_string()),
        bootloader: "u-boot-rockchip".to_string(),
        hostname,
        username,
        password,
        root_password,
        locale: "en_US.UTF-8".to_string(),
        timezone: "UTC".to_string(),
        packages: vec![],
        image_size_gb: image_size,
        output_path,
    };
    
    // Show progress dialog
    let progress = Arc::new(Mutex::new(0));
    let progress_clone = Arc::clone(&progress);
    
    let mut progress_bar = ProgressBar::new()
        .with_label(|value, _| format!("Building: {}%", value))
        .max(100);
    
    siv.pop_layer();
    siv.add_layer(Dialog::around(
        LinearLayout::vertical()
            .child(TextView::new("Building GameScope Desktop distro..."))
            .child(DummyView.fixed_height(1))
            .child(progress_bar.with_name("build_progress"))
            .child(DummyView.fixed_height(1))
            .child(TextView::new("").with_name("build_status"))
    )
    .title("Build Progress"));
    
    // Start build in background thread
    thread::spawn(move || {
        // Use the gamescope_builder module
        match gamescope_builder::GameScopeBuilder::new(config) {
            Ok(builder) => {
                match builder.build_desktop_distro() {
                    Ok(_) => {
                        if let Ok(mut p) = progress_clone.lock() {
                            *p = 100;
                        }
                    }
                    Err(e) => {
                        error!("Build failed: {}", e);
                    }
                }
            }
            Err(e) => {
                error!("Failed to create builder: {}", e);
            }
        }
    });
    
    // Update progress periodically
    siv.add_global_callback(cursive::event::Event::Refresh, move |s| {
        if let Ok(p) = progress.lock() {
            s.call_on_name("build_progress", |view: &mut ProgressBar| {
                view.set_value(*p);
            });
            
            if *p >= 100 {
                s.pop_layer();
                s.add_layer(Dialog::text("GameScope Desktop distro built successfully!")
                    .title("Build Complete")
                    .button("OK", |s| {
                        s.pop_layer();
                        crate::ui::setup_main_menu(s);
                    }));
            }
        }
    });
}

/// Build GameScope RetroArch distro
fn build_gamescope_retroarch(siv: &mut Cursive) {
    // Get values from UI
    let hostname = siv.call_on_name("hostname", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "orangepi-retroarch".to_string());
    
    let username = siv.call_on_name("username", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "retro".to_string());
    
    let password = siv.call_on_name("password", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "changeme".to_string());
    
    let root_password = siv.call_on_name("root_password", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "root".to_string());
    
    let output_path = siv.call_on_name("output_path", |view: &mut EditView| {
        view.get_content().to_string()
    }).unwrap_or_else(|| "orangepi-gamescope-retroarch.img".to_string());
    
    let image_size: u32 = siv.call_on_name("image_size", |view: &mut EditView| {
        view.get_content().parse().unwrap_or(8)
    }).unwrap_or(8);
    
    // Validate inputs
    if password.is_empty() || root_password.is_empty() {
        siv.add_layer(Dialog::text("Password fields cannot be empty!")
            .title("Validation Error")
            .button("OK", |s| { s.pop_layer(); }));
        return;
    }
    
    // Create build configuration for gaming distro
    let config = BuildConfig {
        distro: "debian".to_string(),
        distro_version: "12.11".to_string(),
        kernel_version: "rockchip-rk3588".to_string(),
        desktop_environment: Some("gamescope-retroarch".to_string()),
        gpu_driver: Some("mali-g610-g13p0".to_string()),
        bootloader: "u-boot-rockchip".to_string(),
        hostname,
        username,
        password,
        root_password,
        locale: "en_US.UTF-8".to_string(),
        timezone: "UTC".to_string(),
        packages: vec![],
        image_size_gb: image_size,
        output_path,
    };
    
    // Show progress dialog
    let progress = Arc::new(Mutex::new(0));
    let progress_clone = Arc::clone(&progress);
    
    let mut progress_bar = ProgressBar::new()
        .with_label(|value, _| format!("Building: {}%", value))
        .max(100);
    
    siv.pop_layer();
    siv.add_layer(Dialog::around(
        LinearLayout::vertical()
            .child(TextView::new("Building GameScope RetroArch distro..."))
            .child(DummyView.fixed_height(1))
            .child(progress_bar.with_name("build_progress"))
            .child(DummyView.fixed_height(1))
            .child(TextView::new("").with_name("build_status"))
    )
    .title("Build Progress"));
    
    // Start build in background thread
    thread::spawn(move || {
        // Use the gamescope_builder module
        match gamescope_builder::GameScopeBuilder::new(config) {
            Ok(builder) => {
                match builder.build_gaming_distro() {
                    Ok(_) => {
                        if let Ok(mut p) = progress_clone.lock() {
                            *p = 100;
                        }
                    }
                    Err(e) => {
                        error!("Build failed: {}", e);
                    }
                }
            }
            Err(e) => {
                error!("Failed to create builder: {}", e);
            }
        }
    });
    
    // Update progress periodically
    siv.add_global_callback(cursive::event::Event::Refresh, move |s| {
        if let Ok(p) = progress.lock() {
            s.call_on_name("build_progress", |view: &mut ProgressBar| {
                view.set_value(*p);
            });
            
            if *p >= 100 {
                s.pop_layer();
                s.add_layer(Dialog::text("GameScope RetroArch distro built successfully!")
                    .title("Build Complete")
                    .button("OK", |s| {
                        s.pop_layer();
                        crate::ui::setup_main_menu(s);
                    }));
            }
        }
    });
}