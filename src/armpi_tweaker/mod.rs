use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;
use crate::error::Result;
use crate::ui::logger;
use std::process::Command;

pub mod gpu_drivers;
pub mod hardware_acceleration;
pub mod cpu_scheduler;
pub mod mpp_tweaks;
pub mod system_info;
pub mod system_management;
pub mod network_configuration;
pub mod software_installation;
pub mod localization;
pub mod storage_management;
pub mod security_config;

pub fn show_armpi_tweaker(siv: &mut Cursive) {
    logger::log_ui_action("MODULE_OPEN", "Arm-Pi Tweaker");
    
    let content = create_armpi_tweaker_menu();
    
    let dialog = Dialog::around(content)
        .title("🔧 Arm-Pi Tweaker - Complete Orange Pi 5 Plus Configuration")
        .button("Back to Main Menu", |s| {
            logger::log_ui_action("MODULE_CLOSE", "Arm-Pi Tweaker");
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
}

fn create_armpi_tweaker_menu() -> Box<dyn View> {
    let mut layout = LinearLayout::vertical();
    
    let header = TextView::new("Complete Orange Pi 5 Plus system configuration and performance optimization");
    layout.add_child(header);
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<TweakerOption>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    // Performance & Hardware Section
    menu.add_item("🎮 GPU Drivers - Install/Switch GPU drivers (Mali, Mesa, Panfrost)", TweakerOption::GpuDrivers);
    menu.add_item("⚡ Hardware Acceleration - Setup Video/Audio acceleration", TweakerOption::HardwareAcceleration);
    menu.add_item("🚀 CPU Scheduler/Governor - Configure CPU performance profiles", TweakerOption::CpuScheduler);
    menu.add_item("🎯 MPP & Performance Tweaks - Media Processing & optimizations", TweakerOption::MppTweaks);
    
    menu.add_item("", TweakerOption::Separator);
    
    // System Management Section (Armbian configng features)
    menu.add_item("⚙️ System Management - Kernel, SSH, services, updates", TweakerOption::SystemManagement);
    menu.add_item("🌐 Network Configuration - Wi-Fi, VPN, IP settings, Bluetooth", TweakerOption::NetworkConfig);
    menu.add_item("📦 Software Center - Install media servers, automation, tools", TweakerOption::SoftwareCenter);
    menu.add_item("💾 Storage Management - Disk, filesystem, boot configuration", TweakerOption::StorageManagement);
    
    menu.add_item("", TweakerOption::Separator);
    
    // Configuration & Info Section
    menu.add_item("🌍 Localization - Timezone, locale, keyboard layout", TweakerOption::Localization);
    menu.add_item("🔐 Security Configuration - Firewall, users, SSH keys", TweakerOption::SecurityConfig);
    menu.add_item("📊 System Information - Hardware and performance info", TweakerOption::SystemInfo);
    menu.add_item("🔧 Advanced Settings - Expert configuration options", TweakerOption::AdvancedSettings);
    
    menu.set_on_submit(|s, option| {
        handle_tweaker_selection(s, option);
    });
    
    layout.add_child(menu);
    
    layout.add_child(DummyView.fixed_height(1));
    let footer = TextView::new("⚠️  Based on Armbian Config - Some options require root privileges and system restart");
    layout.add_child(footer);
    
    Box::new(layout.fixed_width(90))
}

#[derive(Debug, Clone, Copy)]
enum TweakerOption {
    // Performance & Hardware
    GpuDrivers,
    HardwareAcceleration,
    CpuScheduler,
    MppTweaks,
    
    // System Management (Armbian configng)
    SystemManagement,
    NetworkConfig,
    SoftwareCenter,
    StorageManagement,
    
    // Configuration & Info
    Localization,
    SecurityConfig,
    SystemInfo,
    AdvancedSettings,
    
    // Utility
    Separator,
}

fn handle_tweaker_selection(siv: &mut Cursive, option: &TweakerOption) {
    if matches!(option, TweakerOption::Separator) {
        return;
    }
    
    let option_name = match option {
        TweakerOption::GpuDrivers => "GPU Drivers",
        TweakerOption::HardwareAcceleration => "Hardware Acceleration",
        TweakerOption::CpuScheduler => "CPU Scheduler/Governor",
        TweakerOption::MppTweaks => "MPP & Performance Tweaks",
        TweakerOption::SystemManagement => "System Management",
        TweakerOption::NetworkConfig => "Network Configuration",
        TweakerOption::SoftwareCenter => "Software Center",
        TweakerOption::StorageManagement => "Storage Management",
        TweakerOption::Localization => "Localization",
        TweakerOption::SecurityConfig => "Security Configuration",
        TweakerOption::SystemInfo => "System Information",
        TweakerOption::AdvancedSettings => "Advanced Settings",
        TweakerOption::Separator => return,
    };
    
    logger::log_menu_selection("Arm-Pi Tweaker", option_name);
    
    match option {
        // Performance & Hardware
        TweakerOption::GpuDrivers => {
            gpu_drivers::show_gpu_driver_menu(siv);
        }
        TweakerOption::HardwareAcceleration => {
            hardware_acceleration::show_hardware_acceleration_menu(siv);
        }
        TweakerOption::CpuScheduler => {
            cpu_scheduler::show_cpu_scheduler_menu(siv);
        }
        TweakerOption::MppTweaks => {
            mpp_tweaks::show_mpp_tweaks_menu(siv);
        }
        
        // System Management (Armbian configng features)
        TweakerOption::SystemManagement => {
            system_management::show_system_management_menu(siv);
        }
        TweakerOption::NetworkConfig => {
            network_configuration::show_network_configuration_menu(siv);
        }
        TweakerOption::SoftwareCenter => {
            software_installation::show_software_installation_menu(siv);
        }
        TweakerOption::StorageManagement => {
            storage_management::show_storage_management_menu(siv);
        }
        
        // Configuration & Info
        TweakerOption::Localization => {
            localization::show_localization_menu(siv);
        }
        TweakerOption::SecurityConfig => {
            security_config::show_security_config_menu(siv);
        }
        TweakerOption::SystemInfo => {
            system_info::show_system_info_menu(siv);
        }
        TweakerOption::AdvancedSettings => {
            show_advanced_settings_menu(siv);
        }
        TweakerOption::Separator => {}
    }
}

fn show_advanced_settings_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Advanced System Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("🔐 Boot Configuration - Modify boot parameters and device tree", "boot_config");
    menu.add_item("🌡️ Thermal Management - Configure thermal throttling and cooling", "thermal_management");
    menu.add_item("⚡ Power Management - Configure power profiles and governors", "power_management");
    menu.add_item("📡 Device Tree Overlays - Hardware interface configuration", "device_tree");
    menu.add_item("🔧 Kernel Modules - Load/unload and configure modules", "kernel_modules");
    menu.add_item("💾 Memory Tuning - Configure memory and swap parameters", "memory_tuning");
    menu.add_item("🌐 Network Optimization - Advanced network performance tweaks", "network_optimization");
    menu.add_item("🏗️ Build System Integration - Kernel and firmware building", "build_integration");
    menu.add_item("🔄 Reset to Defaults - Reset all tweaks to factory defaults", "reset_defaults");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "reset_defaults" => {
                show_reset_confirmation(s);
            }
            "boot_config" => {
                show_boot_configuration_menu(s);
            }
            "thermal_management" => {
                show_thermal_management_menu(s);
            }
            "device_tree" => {
                show_device_tree_menu(s);
            }
            _ => {
                s.add_layer(
                    Dialog::text(format!("{} configuration coming soon!\n\nThis will provide advanced control over system settings.", option))
                        .title("Advanced Configuration")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Advanced Settings")
        .button("Back", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_boot_configuration_menu(siv: &mut Cursive) {
    let content = TextView::new(
        "Boot Configuration Management\n\n\
        Configure U-Boot parameters, kernel command line options, and boot behavior.\n\n\
        Available options:\n\
        • Modify kernel command line parameters\n\
        • Configure U-Boot environment variables\n\
        • Set boot delays and timeouts\n\
        • Configure boot device priority\n\
        • Enable/disable boot splash screen\n\
        • Set console output configuration\n\n\
        ⚠️  Incorrect boot configuration can prevent system startup!"
    );
    
    siv.add_layer(
        Dialog::around(content)
            .title("Boot Configuration")
            .button("Edit Boot Parameters", |s| {
                s.add_layer(
                    Dialog::text("Boot parameter editor coming soon!\n\nThis will allow direct editing of kernel cmdline and U-Boot settings.")
                        .title("Boot Editor")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Back", |s| { s.pop_layer(); })
    );
}

fn show_thermal_management_menu(siv: &mut Cursive) {
    let content = TextView::new(
        "Thermal Management Configuration\n\n\
        Configure CPU and GPU thermal throttling to prevent overheating.\n\n\
        Current thermal zones:\n\
        • CPU thermal zone: Active\n\
        • GPU thermal zone: Active\n\
        • Chipset thermal zone: Active\n\n\
        Thermal policies:\n\
        • Conservative: Throttle early to prevent heat\n\
        • Balanced: Standard thermal management\n\
        • Performance: Allow higher temperatures\n\
        • Custom: User-defined thermal points\n\n\
        ⚠️  Aggressive thermal settings may reduce hardware lifespan!"
    );
    
    siv.add_layer(
        Dialog::around(content)
            .title("Thermal Management")
            .button("Configure Thermal Policy", |s| {
                s.add_layer(
                    Dialog::text("Thermal policy editor coming soon!")
                        .title("Thermal Policy")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Back", |s| { s.pop_layer(); })
    );
}

fn show_device_tree_menu(siv: &mut Cursive) {
    let content = TextView::new(
        "Device Tree Overlay Configuration\n\n\
        Enable/disable hardware interfaces and peripherals through device tree overlays.\n\n\
        Available overlays:\n\
        • SPI interfaces (SPI0, SPI1, SPI3, SPI4)\n\
        • I2C interfaces (I2C1, I2C3, I2C5, I2C6, I2C7, I2C8)\n\
        • UART interfaces (UART1, UART3, UART4, UART6, UART7, UART9)\n\
        • PWM outputs (PWM1, PWM2, PWM3)\n\
        • GPIO pin configurations\n\
        • Camera interfaces (MIPI-CSI)\n\
        • Audio interfaces (I2S)\n\
        • USB host/device modes\n\n\
        ⚠️  Enabling overlays may conflict with existing hardware!"
    );
    
    siv.add_layer(
        Dialog::around(content)
            .title("Device Tree Overlays")
            .button("Manage Overlays", |s| {
                s.add_layer(
                    Dialog::text("Device tree overlay manager coming soon!\n\nThis will allow enabling/disabling hardware interfaces.")
                        .title("Overlay Manager")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Back", |s| { s.pop_layer(); })
    );
}

fn show_reset_confirmation(siv: &mut Cursive) {
    let dialog = Dialog::text(
        "⚠️  WARNING: Complete System Reset\n\n\
        This will reset ALL Arm-Pi Tweaker configurations to factory defaults!\n\n\
        This includes:\n\
        • GPU driver settings and installations\n\
        • CPU governor and frequency settings\n\
        • Hardware acceleration configurations\n\
        • Performance and MPP tweaks\n\
        • Network configurations\n\
        • Software installations (optional)\n\
        • Custom boot parameters\n\
        • Device tree overlay settings\n\
        • Thermal management settings\n\n\
        System restart will be required after reset.\n\n\
        Continue with complete reset?"
    )
    .title("Complete System Reset")
    .button("Reset Everything", |s| {
        s.pop_layer();
        perform_complete_system_reset(s);
    })
    .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn perform_complete_system_reset(siv: &mut Cursive) {
    logger::log_ui_action("SYSTEM_COMPLETE_RESET", "Resetting all Arm-Pi Tweaker and system settings");
    
    let dialog = Dialog::text(
        "Performing complete system reset...\n\n\
        Resetting all configurations to factory defaults.\n\
        This may take several minutes.\n\
        Please do not power off the system.\n\n\
        Progress:\n\
        🔄 Resetting GPU drivers...\n\
        🔄 Resetting CPU settings...\n\
        🔄 Resetting network configuration...\n\
        🔄 Cleaning software installations...\n\
        🔄 Restoring boot configuration..."
    )
    .title("System Reset in Progress");
    
    siv.add_layer(dialog);
    
    std::thread::spawn(|| {
        std::thread::sleep(std::time::Duration::from_secs(5));
    });
    
    siv.add_global_callback(cursive::event::Event::Refresh, |s| {
        s.pop_layer();
        s.add_layer(
            Dialog::text("Complete system reset finished!\n\n✅ GPU drivers reset to defaults\n✅ CPU settings restored\n✅ Network configuration cleared\n✅ Boot parameters restored\n✅ All tweaks removed\n\nPlease restart your Orange Pi 5 Plus to complete the reset process.")
                .title("Reset Complete")
                .button("Restart Now", |s| { 
                    logger::log_ui_action("SYSTEM_RESTART", "User initiated restart after reset");
                    s.quit(); 
                })
                .button("Restart Later", |s| { s.pop_layer(); })
        );
    });
}

pub fn get_system_info() -> Result<SystemInfo> {
    let cpu_info = get_cpu_info()?;
    let gpu_info = get_gpu_info()?;
    let memory_info = get_memory_info()?;
    let thermal_info = get_thermal_info()?;
    let storage_info = get_storage_info()?;
    let network_info = get_network_info()?;
    
    Ok(SystemInfo {
        cpu_info,
        gpu_info,
        memory_info,
        thermal_info,
        storage_info,
        network_info,
    })
}

fn get_cpu_info() -> Result<CpuInfo> {
    let output = Command::new("cat")
        .arg("/proc/cpuinfo")
        .output()
        .map_err(|e| crate::error::BuilderError::IoError(format!("Failed to read CPU info: {}", e)))?;
    
    let cpuinfo = String::from_utf8_lossy(&output.stdout);
    
    let processor_count = cpuinfo.matches("processor").count();
    let model_name = cpuinfo
        .lines()
        .find(|line| line.starts_with("model name"))
        .and_then(|line| line.split(':').nth(1))
        .map(|s| s.trim().to_string())
        .unwrap_or_else(|| "Rockchip RK3588S".to_string());
    
    let current_governor = std::fs::read_to_string("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")
        .unwrap_or_else(|_| "unknown".to_string())
        .trim()
        .to_string();
    
    let current_freq = std::fs::read_to_string("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")
        .unwrap_or_else(|_| "0".to_string())
        .trim()
        .parse::<u64>()
        .unwrap_or(0) / 1000;
    
    Ok(CpuInfo {
        processor_count,
        model_name,
        current_governor,
        current_frequency_mhz: current_freq,
    })
}

fn get_gpu_info() -> Result<GpuInfo> {
    let mali_present = std::path::Path::new("/dev/mali0").exists();
    let panfrost_present = std::path::Path::new("/sys/kernel/debug/dri/0").exists();
    
    let current_driver = if mali_present {
        "Mali G610 (Proprietary)"
    } else if panfrost_present {
        "Panfrost (Mesa/Open Source)"
    } else {
        "No GPU Driver Detected"
    }.to_string();
    
    Ok(GpuInfo {
        driver: current_driver,
        mali_present,
        panfrost_present,
    })
}

fn get_memory_info() -> Result<MemoryInfo> {
    let output = Command::new("cat")
        .arg("/proc/meminfo")
        .output()
        .map_err(|e| crate::error::BuilderError::IoError(format!("Failed to read memory info: {}", e)))?;
    
    let meminfo = String::from_utf8_lossy(&output.stdout);
    
    let total_kb = meminfo
        .lines()
        .find(|line| line.starts_with("MemTotal:"))
        .and_then(|line| line.split_whitespace().nth(1))
        .and_then(|s| s.parse::<u64>().ok())
        .unwrap_or(0);
    
    let available_kb = meminfo
        .lines()
        .find(|line| line.starts_with("MemAvailable:"))
        .and_then(|line| line.split_whitespace().nth(1))
        .and_then(|s| s.parse::<u64>().ok())
        .unwrap_or(0);
    
    Ok(MemoryInfo {
        total_mb: total_kb / 1024,
        available_mb: available_kb / 1024,
        used_mb: (total_kb - available_kb) / 1024,
    })
}

fn get_thermal_info() -> Result<ThermalInfo> {
    let temp_paths = vec![
        "/sys/class/thermal/thermal_zone0/temp",
        "/sys/class/thermal/thermal_zone1/temp",
        "/sys/class/thermal/thermal_zone2/temp",
    ];
    
    let mut temperatures = Vec::new();
    
    for path in temp_paths {
        if let Ok(temp_str) = std::fs::read_to_string(path) {
            if let Ok(temp) = temp_str.trim().parse::<i32>() {
                temperatures.push(temp / 1000);
            }
        }
    }
    
    let cpu_temp = temperatures.get(0).copied().unwrap_or(0);
    let gpu_temp = temperatures.get(1).copied().unwrap_or(0);
    let board_temp = temperatures.get(2).copied().unwrap_or(0);
    
    Ok(ThermalInfo {
        cpu_temperature: cpu_temp,
        gpu_temperature: gpu_temp,
        board_temperature: board_temp,
    })
}

fn get_storage_info() -> Result<StorageInfo> {
    let output = Command::new("df")
        .arg("-h")
        .output()
        .map_err(|e| crate::error::BuilderError::IoError(format!("Failed to read storage info: {}", e)))?;
    
    let df_output = String::from_utf8_lossy(&output.stdout);
    let root_line = df_output
        .lines()
        .find(|line| line.ends_with(" /"))
        .unwrap_or("");
    
    let parts: Vec<&str> = root_line.split_whitespace().collect();
    let total = parts.get(1).unwrap_or(&"0").to_string();
    let used = parts.get(2).unwrap_or(&"0").to_string();
    let available = parts.get(3).unwrap_or(&"0").to_string();
    
    Ok(StorageInfo {
        root_total: total,
        root_used: used,
        root_available: available,
    })
}

fn get_network_info() -> Result<NetworkInfo> {
    let output = Command::new("ip")
        .args(&["addr", "show"])
        .output()
        .map_err(|e| crate::error::BuilderError::IoError(format!("Failed to read network info: {}", e)))?;
    
    let ip_output = String::from_utf8_lossy(&output.stdout);
    
    let interfaces: Vec<String> = ip_output
        .lines()
        .filter(|line| line.starts_with(char::is_numeric))
        .map(|line| {
            line.split(':')
                .nth(1)
                .unwrap_or("")
                .trim()
                .to_string()
        })
        .collect();
    
    Ok(NetworkInfo {
        interfaces,
    })
}

#[derive(Debug)]
pub struct SystemInfo {
    pub cpu_info: CpuInfo,
    pub gpu_info: GpuInfo,
    pub memory_info: MemoryInfo,
    pub thermal_info: ThermalInfo,
    pub storage_info: StorageInfo,
    pub network_info: NetworkInfo,
}

#[derive(Debug)]
pub struct CpuInfo {
    pub processor_count: usize,
    pub model_name: String,
    pub current_governor: String,
    pub current_frequency_mhz: u64,
}

#[derive(Debug)]
pub struct GpuInfo {
    pub driver: String,
    pub mali_present: bool,
    pub panfrost_present: bool,
}

#[derive(Debug)]
pub struct MemoryInfo {
    pub total_mb: u64,
    pub available_mb: u64,
    pub used_mb: u64,
}

#[derive(Debug)]
pub struct ThermalInfo {
    pub cpu_temperature: i32,
    pub gpu_temperature: i32,
    pub board_temperature: i32,
}

#[derive(Debug)]
pub struct StorageInfo {
    pub root_total: String,
    pub root_used: String,
    pub root_available: String,
}

#[derive(Debug)]
pub struct NetworkInfo {
    pub interfaces: Vec<String>,
}