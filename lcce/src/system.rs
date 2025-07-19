use anyhow::{Result, anyhow};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs;
use std::process::Command;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SystemInfo {
    pub device_name: String,
    pub kernel_version: String,
    pub architecture: String,
    pub cpu_info: CpuInfo,
    pub gpu_info: GpuInfo,
    pub storage_devices: Vec<StorageDevice>,
    pub memory_info: MemoryInfo,
    pub network_interfaces: Vec<NetworkInterface>,
    pub current_boot_device: String,
    pub available_target_devices: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CpuInfo {
    pub model: String,
    pub cores: u32,
    pub architecture: String,
    pub current_governor: String,
    pub available_governors: Vec<String>,
    pub current_frequency: u64,
    pub max_frequency: u64,
    pub temperature: f32,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GpuInfo {
    pub model: String,
    pub driver_type: String,
    pub driver_version: String,
    pub memory_size: u64,
    pub acceleration_features: Vec<String>,
    pub supported_apis: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StorageDevice {
    pub device_path: String,
    pub device_name: String,
    pub size_gb: u64,
    pub device_type: StorageType,
    pub is_removable: bool,
    pub is_boot_device: bool,
    pub filesystems: Vec<Filesystem>,
    pub health_status: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum StorageType {
    SD,
    MicroSD,
    EMMC,
    NVME,
    SATA,
    USB,
    Unknown,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Filesystem {
    pub partition: String,
    pub filesystem_type: String,
    pub size_gb: u64,
    pub used_gb: u64,
    pub mount_point: Option<String>,
    pub is_boot_partition: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MemoryInfo {
    pub total_gb: u64,
    pub available_gb: u64,
    pub swap_gb: u64,
    pub memory_type: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NetworkInterface {
    pub name: String,
    pub interface_type: String,
    pub mac_address: String,
    pub is_connected: bool,
    pub ip_address: Option<String>,
    pub speed_mbps: Option<u32>,
}

impl SystemInfo {
    pub fn detect() -> Result<Self> {
        log::info!("Starting system detection for Orange Pi 5 Plus");
        
        let device_name = detect_device_name()?;
        let kernel_version = detect_kernel_version()?;
        let architecture = detect_architecture()?;
        let cpu_info = detect_cpu_info()?;
        let gpu_info = detect_gpu_info()?;
        let storage_devices = detect_storage_devices()?;
        let memory_info = detect_memory_info()?;
        let network_interfaces = detect_network_interfaces()?;
        let current_boot_device = detect_boot_device(&storage_devices)?;
        let available_target_devices = detect_target_devices(&storage_devices, &current_boot_device)?;
        
        Ok(SystemInfo {
            device_name,
            kernel_version,
            architecture,
            cpu_info,
            gpu_info,
            storage_devices,
            memory_info,
            network_interfaces,
            current_boot_device,
            available_target_devices,
        })
    }
    
    pub fn get_target_devices(&self) -> Vec<&StorageDevice> {
        self.storage_devices
            .iter()
            .filter(|device| {
                self.available_target_devices.contains(&device.device_path) &&
                !device.is_boot_device
            })
            .collect()
    }
    
    pub fn get_current_kernel_modules(&self) -> Result<Vec<String>> {
        let output = Command::new("lsmod")
            .output()
            .map_err(|e| anyhow!("Failed to run lsmod: {}", e))?;
        
        let modules = String::from_utf8_lossy(&output.stdout)
            .lines()
            .skip(1) // Skip header
            .map(|line| {
                line.split_whitespace()
                    .next()
                    .unwrap_or("")
                    .to_string()
            })
            .filter(|name| !name.is_empty())
            .collect();
        
        Ok(modules)
    }
    
    pub fn get_boot_parameters(&self) -> Result<String> {
        fs::read_to_string("/proc/cmdline")
            .map_err(|e| anyhow!("Failed to read boot parameters: {}", e))
    }
}

fn detect_device_name() -> Result<String> {
    // Try to detect from device tree
    if let Ok(model) = fs::read_to_string("/proc/device-tree/model") {
        if model.to_lowercase().contains("orange pi 5 plus") {
            return Ok("Orange Pi 5 Plus".to_string());
        }
    }
    
    // Fallback detection methods
    if let Ok(cpuinfo) = fs::read_to_string("/proc/cpuinfo") {
        if cpuinfo.contains("rk3588") || cpuinfo.contains("RK3588S") {
            return Ok("Orange Pi 5 Plus (RK3588S)".to_string());
        }
    }
    
    Ok("Orange Pi 5 Plus".to_string())
}

fn detect_kernel_version() -> Result<String> {
    let output = Command::new("uname")
        .arg("-r")
        .output()
        .map_err(|e| anyhow!("Failed to get kernel version: {}", e))?;
    
    Ok(String::from_utf8_lossy(&output.stdout).trim().to_string())
}

fn detect_architecture() -> Result<String> {
    let output = Command::new("uname")
        .arg("-m")
        .output()
        .map_err(|e| anyhow!("Failed to get architecture: {}", e))?;
    
    Ok(String::from_utf8_lossy(&output.stdout).trim().to_string())
}

fn detect_cpu_info() -> Result<CpuInfo> {
    let cpuinfo = fs::read_to_string("/proc/cpuinfo")
        .map_err(|e| anyhow!("Failed to read CPU info: {}", e))?;
    
    let cores = cpuinfo.matches("processor").count() as u32;
    
    let model = cpuinfo
        .lines()
        .find(|line| line.starts_with("Hardware"))
        .and_then(|line| line.split(':').nth(1))
        .map(|s| s.trim().to_string())
        .unwrap_or_else(|| "Rockchip RK3588S".to_string());
    
    let current_governor = fs::read_to_string("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")
        .unwrap_or_else(|_| "unknown".to_string())
        .trim()
        .to_string();
    
    let available_governors = fs::read_to_string("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors")
        .unwrap_or_else(|_| "performance powersave".to_string())
        .split_whitespace()
        .map(|s| s.to_string())
        .collect();
    
    let current_frequency = fs::read_to_string("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")
        .unwrap_or_else(|_| "0".to_string())
        .trim()
        .parse::<u64>()
        .unwrap_or(0) / 1000; // Convert to MHz
    
    let max_frequency = fs::read_to_string("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq")
        .unwrap_or_else(|_| "2400000".to_string())
        .trim()
        .parse::<u64>()
        .unwrap_or(2400000) / 1000; // Convert to MHz
    
    let temperature = fs::read_to_string("/sys/class/thermal/thermal_zone0/temp")
        .unwrap_or_else(|_| "45000".to_string())
        .trim()
        .parse::<i32>()
        .unwrap_or(45000) as f32 / 1000.0; // Convert to Celsius
    
    Ok(CpuInfo {
        model,
        cores,
        architecture: "aarch64".to_string(),
        current_governor,
        available_governors,
        current_frequency,
        max_frequency,
        temperature,
    })
}

fn detect_gpu_info() -> Result<GpuInfo> {
    let mut model = "Mali-G610 MP4".to_string();
    let mut driver_type = "Unknown".to_string();
    let mut driver_version = "Unknown".to_string();
    let acceleration_features = vec![
        "Hardware Video Decode".to_string(),
        "Hardware Video Encode".to_string(),
        "OpenGL ES 3.2".to_string(),
        "Vulkan 1.1".to_string(),
    ];
    let supported_apis = vec![
        "OpenGL ES 3.2".to_string(),
        "EGL 1.5".to_string(),
        "Vulkan 1.1".to_string(),
        "OpenCL 2.0".to_string(),
    ];
    
    // Check for Mali driver
    if std::path::Path::new("/dev/mali0").exists() {
        driver_type = "Mali Proprietary".to_string();
        
        // Try to get version from package manager
        if let Ok(output) = Command::new("dpkg").args(["-l", "libmali*"]).output() {
            let packages = String::from_utf8_lossy(&output.stdout);
            for line in packages.lines() {
                if line.contains("libmali") && line.contains("g610") {
                    if let Some(version) = line.split_whitespace().nth(2) {
                        driver_version = version.to_string();
                        break;
                    }
                }
            }
        }
    } else if std::path::Path::new("/sys/kernel/debug/dri/0").exists() {
        driver_type = "Panfrost (Open Source)".to_string();
        driver_version = "Mesa".to_string();
    }
    
    Ok(GpuInfo {
        model,
        driver_type,
        driver_version,
        memory_size: 0, // Shared with system memory
        acceleration_features,
        supported_apis,
    })
}

fn detect_storage_devices() -> Result<Vec<StorageDevice>> {
    let mut devices = Vec::new();
    
    // Read from /proc/partitions and lsblk
    let output = Command::new("lsblk")
        .args(["-J", "-o", "NAME,SIZE,TYPE,MOUNTPOINT,FSTYPE,MODEL"])
        .output()
        .map_err(|e| anyhow!("Failed to run lsblk: {}", e))?;
    
    let lsblk_output = String::from_utf8_lossy(&output.stdout);
    
    // Parse common Orange Pi 5 Plus storage devices
    if std::path::Path::new("/dev/mmcblk0").exists() {
        devices.push(StorageDevice {
            device_path: "/dev/mmcblk0".to_string(),
            device_name: "eMMC Storage".to_string(),
            size_gb: get_device_size("/dev/mmcblk0")?,
            device_type: StorageType::EMMC,
            is_removable: false,
            is_boot_device: false,
            filesystems: get_device_filesystems("/dev/mmcblk0")?,
            health_status: "Good".to_string(),
        });
    }
    
    if std::path::Path::new("/dev/mmcblk1").exists() {
        devices.push(StorageDevice {
            device_path: "/dev/mmcblk1".to_string(),
            device_name: "MicroSD Card".to_string(),
            size_gb: get_device_size("/dev/mmcblk1")?,
            device_type: StorageType::MicroSD,
            is_removable: true,
            is_boot_device: true, // Usually boot device
            filesystems: get_device_filesystems("/dev/mmcblk1")?,
            health_status: "Good".to_string(),
        });
    }
    
    if std::path::Path::new("/dev/nvme0n1").exists() {
        devices.push(StorageDevice {
            device_path: "/dev/nvme0n1".to_string(),
            device_name: "NVMe SSD".to_string(),
            size_gb: get_device_size("/dev/nvme0n1")?,
            device_type: StorageType::NVME,
            is_removable: false,
            is_boot_device: false,
            filesystems: get_device_filesystems("/dev/nvme0n1")?,
            health_status: "Good".to_string(),
        });
    }
    
    Ok(devices)
}

fn get_device_size(device: &str) -> Result<u64> {
    let output = Command::new("blockdev")
        .args(["--getsize64", device])
        .output()
        .map_err(|e| anyhow!("Failed to get device size: {}", e))?;
    
    let size_bytes = String::from_utf8_lossy(&output.stdout)
        .trim()
        .parse::<u64>()
        .unwrap_or(0);
    
    Ok(size_bytes / 1_000_000_000) // Convert to GB
}

fn get_device_filesystems(device: &str) -> Result<Vec<Filesystem>> {
    let mut filesystems = Vec::new();
    
    let output = Command::new("lsblk")
        .args(["-n", "-o", "NAME,SIZE,FSTYPE,MOUNTPOINT", device])
        .output()
        .map_err(|e| anyhow!("Failed to get filesystems: {}", e))?;
    
    let lsblk_output = String::from_utf8_lossy(&output.stdout);
    
    for line in lsblk_output.lines() {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 3 {
            let partition = format!("/dev/{}", parts[0].trim_start_matches("├─").trim_start_matches("└─"));
            let size_str = parts[1];
            let fstype = parts.get(2).unwrap_or(&"").to_string();
            let mount_point = parts.get(3).map(|s| s.to_string());
            
            // Parse size
            let size_gb = parse_size_to_gb(size_str);
            
            filesystems.push(Filesystem {
                partition,
                filesystem_type: fstype,
                size_gb,
                used_gb: 0, // TODO: Get actual usage
                mount_point,
                is_boot_partition: mount_point.as_ref().map_or(false, |mp| mp == "/boot" || mp == "/"),
            });
        }
    }
    
    Ok(filesystems)
}

fn parse_size_to_gb(size_str: &str) -> u64 {
    let size_str = size_str.to_uppercase();
    if let Some(num_str) = size_str.strip_suffix("G") {
        num_str.parse().unwrap_or(0)
    } else if let Some(num_str) = size_str.strip_suffix("M") {
        num_str.parse::<u64>().unwrap_or(0) / 1000
    } else if let Some(num_str) = size_str.strip_suffix("K") {
        num_str.parse::<u64>().unwrap_or(0) / 1_000_000
    } else {
        0
    }
}

fn detect_memory_info() -> Result<MemoryInfo> {
    let meminfo = fs::read_to_string("/proc/meminfo")
        .map_err(|e| anyhow!("Failed to read memory info: {}", e))?;
    
    let mut total_kb = 0;
    let mut available_kb = 0;
    let mut swap_kb = 0;
    
    for line in meminfo.lines() {
        if line.starts_with("MemTotal:") {
            total_kb = parse_meminfo_value(line);
        } else if line.starts_with("MemAvailable:") {
            available_kb = parse_meminfo_value(line);
        } else if line.starts_with("SwapTotal:") {
            swap_kb = parse_meminfo_value(line);
        }
    }
    
    Ok(MemoryInfo {
        total_gb: total_kb / 1_000_000,
        available_gb: available_kb / 1_000_000,
        swap_gb: swap_kb / 1_000_000,
        memory_type: "LPDDR5".to_string(), // Orange Pi 5 Plus uses LPDDR5
    })
}

fn parse_meminfo_value(line: &str) -> u64 {
    line.split_whitespace()
        .nth(1)
        .and_then(|s| s.parse().ok())
        .unwrap_or(0)
}

fn detect_network_interfaces() -> Result<Vec<NetworkInterface>> {
    let mut interfaces = Vec::new();
    
    let output = Command::new("ip")
        .args(["addr", "show"])
        .output()
        .map_err(|e| anyhow!("Failed to get network interfaces: {}", e))?;
    
    let ip_output = String::from_utf8_lossy(&output.stdout);
    
    // Basic parsing - this could be more sophisticated
    for line in ip_output.lines() {
        if line.contains("state UP") || line.contains("state DOWN") {
            if let Some(name) = line.split(':').nth(1) {
                let name = name.trim();
                if name != "lo" { // Skip loopback
                    interfaces.push(NetworkInterface {
                        name: name.to_string(),
                        interface_type: if name.starts_with("eth") {
                            "Ethernet".to_string()
                        } else if name.starts_with("wlan") {
                            "Wi-Fi".to_string()
                        } else {
                            "Unknown".to_string()
                        },
                        mac_address: "00:00:00:00:00:00".to_string(), // TODO: Parse actual MAC
                        is_connected: line.contains("state UP"),
                        ip_address: None, // TODO: Parse IP address
                        speed_mbps: None,
                    });
                }
            }
        }
    }
    
    Ok(interfaces)
}

fn detect_boot_device(storage_devices: &[StorageDevice]) -> Result<String> {
    // Check which device contains the root filesystem
    for device in storage_devices {
        for fs in &device.filesystems {
            if fs.mount_point.as_ref().map_or(false, |mp| mp == "/") {
                return Ok(device.device_path.clone());
            }
        }
    }
    
    // Fallback: assume MicroSD if available
    for device in storage_devices {
        if device.device_type == StorageType::MicroSD {
            return Ok(device.device_path.clone());
        }
    }
    
    Err(anyhow!("Could not determine boot device"))
}

fn detect_target_devices(storage_devices: &[StorageDevice], boot_device: &str) -> Result<Vec<String>> {
    let mut targets = Vec::new();
    
    for device in storage_devices {
        if device.device_path != boot_device {
            match device.device_type {
                StorageType::EMMC | StorageType::NVME => {
                    targets.push(device.device_path.clone());
                }
                _ => {}
            }
        }
    }
    
    Ok(targets)
}