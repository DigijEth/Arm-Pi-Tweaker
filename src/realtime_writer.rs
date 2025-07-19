use std::fs::{File, OpenOptions};
use std::io::{self, Write, Seek, SeekFrom};
use std::path::Path;
use std::process::Command;
use log::{info, warn, error};

pub struct RealtimeWriter {
    device_path: String,
    device_file: Option<File>,
    is_nvme: bool,
    is_emmc: bool,
}

impl RealtimeWriter {
    pub fn new(device_path: &str) -> Result<Self, String> {
        let path = Path::new(device_path);
        if !path.exists() {
            return Err(format!("Device path does not exist: {}", device_path));
        }

        let is_nvme = device_path.contains("nvme");
        let is_emmc = device_path.contains("mmcblk");

        if !is_nvme && !is_emmc {
            return Err("Device is not NVMe or eMMC".to_string());
        }

        Ok(RealtimeWriter {
            device_path: device_path.to_string(),
            device_file: None,
            is_nvme,
            is_emmc,
        })
    }

    pub fn detect_devices() -> Result<Vec<String>, String> {
        let mut devices = Vec::new();

        // Detect NVMe devices
        if let Ok(output) = Command::new("lsblk")
            .args(&["-d", "-n", "-o", "NAME,TYPE"])
            .output()
        {
            let stdout = String::from_utf8_lossy(&output.stdout);
            for line in stdout.lines() {
                if line.contains("nvme") {
                    let parts: Vec<&str> = line.split_whitespace().collect();
                    if parts.len() >= 2 {
                        devices.push(format!("/dev/{}", parts[0]));
                    }
                }
            }
        }

        // Detect eMMC devices
        if let Ok(output) = Command::new("lsblk")
            .args(&["-d", "-n", "-o", "NAME,TYPE"])
            .output()
        {
            let stdout = String::from_utf8_lossy(&output.stdout);
            for line in stdout.lines() {
                if line.contains("mmcblk") {
                    let parts: Vec<&str> = line.split_whitespace().collect();
                    if parts.len() >= 2 {
                        devices.push(format!("/dev/{}", parts[0]));
                    }
                }
            }
        }

        if devices.is_empty() {
            return Err("No NVMe or eMMC devices found".to_string());
        }

        Ok(devices)
    }

    pub fn open_device(&mut self) -> Result<(), String> {
        info!("Opening device: {}", self.device_path);
        
        match OpenOptions::new()
            .read(true)
            .write(true)
            .open(&self.device_path)
        {
            Ok(file) => {
                self.device_file = Some(file);
                Ok(())
            }
            Err(e) => {
                error!("Failed to open device {}: {}", self.device_path, e);
                Err(format!("Failed to open device: {}", e))
            }
        }
    }

    pub fn write_partition_table(&mut self) -> Result<(), String> {
        info!("Creating partition table on {}", self.device_path);
        
        // Use parted to create a GPT partition table
        let output = Command::new("parted")
            .args(&[
                &self.device_path,
                "--script",
                "mklabel",
                "gpt"
            ])
            .output()
            .map_err(|e| format!("Failed to run parted: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("parted failed: {}", stderr));
        }

        Ok(())
    }

    pub fn create_partitions(&mut self) -> Result<(), String> {
        info!("Creating partitions on {}", self.device_path);
        
        // Create boot partition (512MB)
        let output = Command::new("parted")
            .args(&[
                &self.device_path,
                "--script",
                "mkpart",
                "primary",
                "fat32",
                "1MiB",
                "513MiB"
            ])
            .output()
            .map_err(|e| format!("Failed to create boot partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Boot partition creation failed: {}", stderr));
        }

        // Create root partition (remaining space)
        let output = Command::new("parted")
            .args(&[
                &self.device_path,
                "--script",
                "mkpart",
                "primary",
                "ext4",
                "513MiB",
                "100%"
            ])
            .output()
            .map_err(|e| format!("Failed to create root partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Root partition creation failed: {}", stderr));
        }

        // Set boot flag on first partition
        let output = Command::new("parted")
            .args(&[
                &self.device_path,
                "--script",
                "set",
                "1",
                "boot",
                "on"
            ])
            .output()
            .map_err(|e| format!("Failed to set boot flag: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            warn!("Failed to set boot flag: {}", stderr);
        }

        Ok(())
    }

    pub fn format_partitions(&mut self) -> Result<(), String> {
        info!("Formatting partitions on {}", self.device_path);
        
        let boot_partition = if self.is_nvme {
            format!("{}p1", self.device_path)
        } else {
            format!("{}p1", self.device_path)
        };

        let root_partition = if self.is_nvme {
            format!("{}p2", self.device_path)
        } else {
            format!("{}p2", self.device_path)
        };

        // Format boot partition as FAT32
        let output = Command::new("mkfs.fat")
            .args(&["-F", "32", "-n", "BOOT", &boot_partition])
            .output()
            .map_err(|e| format!("Failed to format boot partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Boot partition formatting failed: {}", stderr));
        }

        // Format root partition as ext4
        let output = Command::new("mkfs.ext4")
            .args(&["-F", "-L", "rootfs", &root_partition])
            .output()
            .map_err(|e| format!("Failed to format root partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Root partition formatting failed: {}", stderr));
        }

        Ok(())
    }

    pub fn write_bootloader(&mut self, bootloader_path: &str) -> Result<(), String> {
        info!("Writing bootloader to {}", self.device_path);
        
        if !Path::new(bootloader_path).exists() {
            return Err(format!("Bootloader file not found: {}", bootloader_path));
        }

        // For RK3588, write U-Boot at offset 8192 sectors (4MB)
        let output = Command::new("dd")
            .args(&[
                &format!("if={}", bootloader_path),
                &format!("of={}", self.device_path),
                "bs=512",
                "seek=8192",
                "conv=fsync"
            ])
            .output()
            .map_err(|e| format!("Failed to write bootloader: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Bootloader writing failed: {}", stderr));
        }

        Ok(())
    }

    pub fn mount_partitions(&mut self, mount_point: &str) -> Result<(), String> {
        info!("Mounting partitions at {}", mount_point);
        
        let boot_partition = if self.is_nvme {
            format!("{}p1", self.device_path)
        } else {
            format!("{}p1", self.device_path)
        };

        let root_partition = if self.is_nvme {
            format!("{}p2", self.device_path)
        } else {
            format!("{}p2", self.device_path)
        };

        // Create mount points
        std::fs::create_dir_all(mount_point)
            .map_err(|e| format!("Failed to create mount point: {}", e))?;

        let boot_mount = format!("{}/boot", mount_point);
        std::fs::create_dir_all(&boot_mount)
            .map_err(|e| format!("Failed to create boot mount point: {}", e))?;

        // Mount root partition
        let output = Command::new("mount")
            .args(&[&root_partition, mount_point])
            .output()
            .map_err(|e| format!("Failed to mount root partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Root partition mount failed: {}", stderr));
        }

        // Mount boot partition
        let output = Command::new("mount")
            .args(&[&boot_partition, &boot_mount])
            .output()
            .map_err(|e| format!("Failed to mount boot partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Boot partition mount failed: {}", stderr));
        }

        Ok(())
    }

    pub fn unmount_partitions(&mut self, mount_point: &str) -> Result<(), String> {
        info!("Unmounting partitions at {}", mount_point);
        
        let boot_mount = format!("{}/boot", mount_point);

        // Unmount boot partition
        let _ = Command::new("umount").arg(&boot_mount).output();

        // Unmount root partition
        let output = Command::new("umount")
            .arg(mount_point)
            .output()
            .map_err(|e| format!("Failed to unmount root partition: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            warn!("Root partition unmount failed: {}", stderr);
        }

        Ok(())
    }

    pub fn sync_filesystems(&mut self) -> Result<(), String> {
        info!("Syncing filesystems");
        
        let output = Command::new("sync")
            .output()
            .map_err(|e| format!("Failed to sync filesystems: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Filesystem sync failed: {}", stderr));
        }

        Ok(())
    }

    pub fn get_device_info(&self) -> Result<DeviceInfo, String> {
        let output = Command::new("lsblk")
            .args(&["-b", "-n", "-o", "NAME,SIZE,MODEL", &self.device_path])
            .output()
            .map_err(|e| format!("Failed to get device info: {}", e))?;

        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            return Err(format!("Device info failed: {}", stderr));
        }

        let stdout = String::from_utf8_lossy(&output.stdout);
        let lines: Vec<&str> = stdout.lines().collect();
        
        if lines.is_empty() {
            return Err("No device information found".to_string());
        }

        let parts: Vec<&str> = lines[0].split_whitespace().collect();
        if parts.len() < 2 {
            return Err("Invalid device information format".to_string());
        }

        let size_bytes: u64 = parts[1].parse()
            .map_err(|e| format!("Failed to parse device size: {}", e))?;

        let model = if parts.len() >= 3 {
            parts[2..].join(" ")
        } else {
            "Unknown".to_string()
        };

        Ok(DeviceInfo {
            name: parts[0].to_string(),
            size_bytes,
            model,
            device_type: if self.is_nvme { "NVMe".to_string() } else { "eMMC".to_string() },
        })
    }
}

#[derive(Debug, Clone)]
pub struct DeviceInfo {
    pub name: String,
    pub size_bytes: u64,
    pub model: String,
    pub device_type: String,
}

impl DeviceInfo {
    pub fn size_gb(&self) -> f64 {
        self.size_bytes as f64 / 1_000_000_000.0
    }

    pub fn size_human_readable(&self) -> String {
        let gb = self.size_gb();
        if gb >= 1000.0 {
            format!("{:.1} TB", gb / 1000.0)
        } else {
            format!("{:.1} GB", gb)
        }
    }
}