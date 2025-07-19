use cursive::views::{Dialog, LinearLayout, TextView, SelectView, ScrollView};
use cursive::Cursive;
use cursive::align::HAlign;
use cursive::theme::{ColorStyle, BaseColor, Color};
use cursive::utils::markup::StyledString;
use cursive::traits::*;
use std::path::PathBuf;
use std::collections::HashMap;
use crate::error::{BuilderError, Result};

pub mod uboot;

/// Bootloader types supported by the Orange Pi 5 Plus
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum BootloaderType {
    UBoot,
    EDK2UEFI,
    TowBoot,
    Petitboot,
    Barebox,
    LinuxBoot,
    RockchipMiniLoader,
    Coreboot,
}

impl std::fmt::Display for BootloaderType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BootloaderType::UBoot => write!(f, "U-Boot"),
            BootloaderType::EDK2UEFI => write!(f, "EDK2/UEFI"),
            BootloaderType::TowBoot => write!(f, "Tow-Boot"),
            BootloaderType::Petitboot => write!(f, "Petitboot"),
            BootloaderType::Barebox => write!(f, "Barebox"),
            BootloaderType::LinuxBoot => write!(f, "LinuxBoot"),
            BootloaderType::RockchipMiniLoader => write!(f, "Rockchip MiniLoader"),
            BootloaderType::Coreboot => write!(f, "Coreboot"),
        }
    }
}

/// Bootloader configuration
#[derive(Debug, Clone)]
pub struct BootloaderConfig {
    pub bootloader_type: BootloaderType,
    pub variant: String,
    pub source_repo: String,
    pub branch: String,
    pub build_options: HashMap<String, String>,
    pub output_path: PathBuf,
    pub target_arch: String,
    pub cross_compile: String,
    pub device_tree: Option<String>,
    pub enable_debug: bool,
    pub enable_secure_boot: bool,
    pub enable_fastboot: bool,
    pub memory_size: Option<u32>,
    pub storage_type: String,
}

impl Default for BootloaderConfig {
    fn default() -> Self {
        Self {
            bootloader_type: BootloaderType::UBoot,
            variant: "mainline".to_string(),
            source_repo: "https://github.com/u-boot/u-boot.git".to_string(),
            branch: "master".to_string(),
            build_options: HashMap::new(),
            output_path: PathBuf::from("./output"),
            target_arch: "aarch64".to_string(),
            cross_compile: "aarch64-linux-gnu-".to_string(),
            device_tree: Some("rk3588s-orangepi-5-plus".to_string()),
            enable_debug: false,
            enable_secure_boot: false,
            enable_fastboot: true,
            memory_size: Some(16384), // 16GB
            storage_type: "spi".to_string(),
        }
    }
}

/// Bootloader build status
#[derive(Debug, Clone)]
pub enum BuildStatus {
    NotStarted,
    Preparing,
    Downloading,
    Configuring,
    Building,
    Installing,
    Completed,
    Failed(String),
}

impl std::fmt::Display for BuildStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BuildStatus::NotStarted => write!(f, "Not Started"),
            BuildStatus::Preparing => write!(f, "Preparing"),
            BuildStatus::Downloading => write!(f, "Downloading"),
            BuildStatus::Configuring => write!(f, "Configuring"),
            BuildStatus::Building => write!(f, "Building"),
            BuildStatus::Installing => write!(f, "Installing"),
            BuildStatus::Completed => write!(f, "Completed"),
            BuildStatus::Failed(msg) => write!(f, "Failed: {}", msg),
        }
    }
}

/// Show bootloader builder selection menu
pub fn show_bootloader_builder(siv: &mut Cursive) {
    let mut content = LinearLayout::vertical();
    
    // Header
    content.add_child(TextView::new(StyledString::styled(
        "Bootloader Builder for Orange Pi 5 Plus",
        ColorStyle::from(Color::Light(BaseColor::Green))
    )));
    content.add_child(TextView::new("Select a bootloader type to build:"));
    content.add_child(TextView::new(""));
    
    // Bootloader selection
    let mut bootloader_select = SelectView::<BootloaderType>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    // Desktop/Server focused bootloaders
    bootloader_select.add_item("🔧 U-Boot - Universal Boot Loader (Recommended)", BootloaderType::UBoot);
    bootloader_select.add_item("🖥️ EDK2/UEFI - UEFI Firmware Interface", BootloaderType::EDK2UEFI);
    bootloader_select.add_item("📡 Petitboot - Network Boot Loader", BootloaderType::Petitboot);
    bootloader_select.add_item("🚀 Tow-Boot - SPI Boot Loader", BootloaderType::TowBoot);
    bootloader_select.add_item("⚡ Rockchip MiniLoader - Proprietary Loader", BootloaderType::RockchipMiniLoader);
    bootloader_select.add_item("🔬 Barebox - Bare Box Boot Loader", BootloaderType::Barebox);
    bootloader_select.add_item("🐧 LinuxBoot - Linux as Boot Loader", BootloaderType::LinuxBoot);
    bootloader_select.add_item("🏗️ Coreboot - Open Source Firmware", BootloaderType::Coreboot);
    
    bootloader_select.set_on_submit(|s, bootloader_type| {
        match bootloader_type {
            BootloaderType::UBoot => {
                s.pop_layer();
                uboot::show_uboot_builder(s);
            }
            BootloaderType::EDK2UEFI => {
                s.pop_layer();
                show_edk2_builder(s);
            }
            BootloaderType::TowBoot => {
                s.pop_layer();
                show_towboot_builder(s);
            }
            BootloaderType::Petitboot => {
                s.pop_layer();
                show_petitboot_builder(s);
            }
            BootloaderType::RockchipMiniLoader => {
                s.pop_layer();
                show_rockchip_miniloader_builder(s);
            }
            _ => {
                s.add_layer(
                    Dialog::info(format!("{} builder is not yet implemented", bootloader_type))
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    content.add_child(bootloader_select);
    
    // Info section
    content.add_child(TextView::new(""));
    content.add_child(TextView::new(StyledString::styled(
        "Bootloader Information:",
        ColorStyle::from(Color::Light(BaseColor::Yellow))
    )));
    content.add_child(TextView::new("• U-Boot: Most compatible, supports all distros"));
    content.add_child(TextView::new("• EDK2/UEFI: Modern UEFI boot for enterprise"));
    content.add_child(TextView::new("• Petitboot: Network boot and recovery"));
    content.add_child(TextView::new("• Tow-Boot: SPI flash boot loader"));
    content.add_child(TextView::new("• Rockchip MiniLoader: Proprietary fast boot"));
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(20))
        .title("Bootloader Builder")
        .button("Back", |s| {
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
}

/// Show EDK2/UEFI builder (placeholder)
fn show_edk2_builder(siv: &mut Cursive) {
    let content = LinearLayout::vertical()
        .child(TextView::new("EDK2/UEFI Builder"))
        .child(TextView::new(""))
        .child(TextView::new("This will build EDK2/UEFI firmware for Orange Pi 5 Plus"))
        .child(TextView::new("Supporting full UEFI boot process with Secure Boot"))
        .child(TextView::new(""))
        .child(TextView::new("Status: Not yet implemented"));
    
    let dialog = Dialog::around(content)
        .title("EDK2/UEFI Builder")
        .button("Back", |s| {
            s.pop_layer();
            show_bootloader_builder(s);
        });
    
    siv.add_layer(dialog);
}

/// Show Tow-Boot builder (placeholder)
fn show_towboot_builder(siv: &mut Cursive) {
    let content = LinearLayout::vertical()
        .child(TextView::new("Tow-Boot SPI Builder"))
        .child(TextView::new(""))
        .child(TextView::new("This will build Tow-Boot for SPI flash storage"))
        .child(TextView::new("Provides recovery and emergency boot options"))
        .child(TextView::new(""))
        .child(TextView::new("Status: Not yet implemented"));
    
    let dialog = Dialog::around(content)
        .title("Tow-Boot Builder")
        .button("Back", |s| {
            s.pop_layer();
            show_bootloader_builder(s);
        });
    
    siv.add_layer(dialog);
}

/// Show Petitboot builder (placeholder)
fn show_petitboot_builder(siv: &mut Cursive) {
    let content = LinearLayout::vertical()
        .child(TextView::new("Petitboot Network Boot Builder"))
        .child(TextView::new(""))
        .child(TextView::new("This will build Petitboot for network boot"))
        .child(TextView::new("Supports PXE boot and remote image loading"))
        .child(TextView::new(""))
        .child(TextView::new("Status: Not yet implemented"));
    
    let dialog = Dialog::around(content)
        .title("Petitboot Builder")
        .button("Back", |s| {
            s.pop_layer();
            show_bootloader_builder(s);
        });
    
    siv.add_layer(dialog);
}

/// Show Rockchip MiniLoader builder (placeholder)
fn show_rockchip_miniloader_builder(siv: &mut Cursive) {
    let content = LinearLayout::vertical()
        .child(TextView::new("Rockchip MiniLoader Builder"))
        .child(TextView::new(""))
        .child(TextView::new("This will build Rockchip's proprietary MiniLoader"))
        .child(TextView::new("Provides fast boot and hardware initialization"))
        .child(TextView::new(""))
        .child(TextView::new("Status: Not yet implemented"));
    
    let dialog = Dialog::around(content)
        .title("Rockchip MiniLoader Builder")
        .button("Back", |s| {
            s.pop_layer();
            show_bootloader_builder(s);
        });
    
    siv.add_layer(dialog);
}

/// Bootloader compatibility matrix
pub fn get_bootloader_compatibility() -> HashMap<BootloaderType, Vec<&'static str>> {
    let mut compatibility = HashMap::new();
    
    compatibility.insert(BootloaderType::UBoot, vec![
        "Ubuntu 22.04 LTS", "Ubuntu 24.04 LTS", "Debian 12", "Debian 13",
        "Armbian", "Manjaro ARM", "Arch Linux ARM", "Fedora ARM"
    ]);
    
    compatibility.insert(BootloaderType::EDK2UEFI, vec![
        "Ubuntu 22.04 LTS", "Ubuntu 24.04 LTS", "Debian 12",
        "Windows 11 ARM", "VMware ESXi ARM"
    ]);
    
    compatibility.insert(BootloaderType::TowBoot, vec![
        "Ubuntu 22.04 LTS", "Debian 12", "Armbian",
        "PostmarketOS", "Mobian"
    ]);
    
    compatibility.insert(BootloaderType::Petitboot, vec![
        "Ubuntu Server", "Debian Server", "CentOS Stream",
        "OpenSUSE Leap", "Fedora Server"
    ]);
    
    compatibility.insert(BootloaderType::RockchipMiniLoader, vec![
        "Android", "Rockchip Linux", "Buildroot",
        "Yocto Project", "OpenWrt"
    ]);
    
    compatibility.insert(BootloaderType::Barebox, vec![
        "Buildroot", "Yocto Project", "OpenWrt",
        "Custom Linux", "Embedded Linux"
    ]);
    
    compatibility.insert(BootloaderType::LinuxBoot, vec![
        "Ubuntu Server", "Debian Server", "CentOS Stream",
        "Cloud Linux", "Container Linux"
    ]);
    
    compatibility.insert(BootloaderType::Coreboot, vec![
        "Ubuntu", "Debian", "Fedora", "OpenSUSE",
        "QubesOS", "Heads", "Custom Firmware"
    ]);
    
    compatibility
}

/// Show bootloader compatibility matrix
pub fn show_compatibility_matrix(siv: &mut Cursive) {
    let compatibility = get_bootloader_compatibility();
    let mut content = LinearLayout::vertical();
    
    content.add_child(TextView::new(StyledString::styled(
        "Bootloader Compatibility Matrix",
        ColorStyle::from(Color::Light(BaseColor::Green))
    )));
    content.add_child(TextView::new("Supported distributions for each bootloader:"));
    content.add_child(TextView::new(""));
    
    for (bootloader, distros) in compatibility.iter() {
        content.add_child(TextView::new(StyledString::styled(
            format!("{}:", bootloader),
            ColorStyle::from(Color::Light(BaseColor::Blue))
        )));
        
        for distro in distros {
            content.add_child(TextView::new(format!("  • {}", distro)));
        }
        
        content.add_child(TextView::new(""));
    }
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(20))
        .title("Bootloader Compatibility")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

/// Validate bootloader configuration
pub fn validate_bootloader_config(config: &BootloaderConfig) -> Result<()> {
    // Validate output path
    let parent_path = config.output_path.parent().unwrap_or(&PathBuf::from(".")).to_path_buf();
    crate::utils::validate_dir_exists(&parent_path)?;
    
    // Validate source repository URL
    if !config.source_repo.starts_with("https://") && !config.source_repo.starts_with("git://") {
        return Err(BuilderError::UrlParseError(config.source_repo.clone()));
    }
    
    // Validate cross-compile toolchain
    if config.cross_compile.is_empty() {
        return Err(BuilderError::ValidationError("Cross-compile toolchain cannot be empty".to_string()));
    }
    
    // Validate target architecture
    if !["aarch64", "armv8", "arm64"].contains(&config.target_arch.as_str()) {
        return Err(BuilderError::ValidationError(format!("Unsupported target architecture: {}", config.target_arch)));
    }
    
    // Validate memory size if specified
    if let Some(memory_size) = config.memory_size {
        if memory_size < 1024 || memory_size > 32768 {
            return Err(BuilderError::ValidationError("Memory size must be between 1GB and 32GB".to_string()));
        }
    }
    
    // Validate storage type
    if !["spi", "mmc", "emmc", "nvme"].contains(&config.storage_type.as_str()) {
        return Err(BuilderError::ValidationError(format!("Unsupported storage type: {}", config.storage_type)));
    }
    
    Ok(())
}

/// Build progress tracking
pub struct BuildProgress {
    pub current_step: String,
    pub total_steps: u32,
    pub current_step_number: u32,
    pub status: BuildStatus,
    pub log_messages: Vec<String>,
}

impl BuildProgress {
    pub fn new(total_steps: u32) -> Self {
        Self {
            current_step: "Initializing".to_string(),
            total_steps,
            current_step_number: 0,
            status: BuildStatus::NotStarted,
            log_messages: Vec::new(),
        }
    }
    
    pub fn update_step(&mut self, step: &str, step_number: u32) {
        self.current_step = step.to_string();
        self.current_step_number = step_number;
        self.log_messages.push(format!("Step {}/{}: {}", step_number, self.total_steps, step));
    }
    
    pub fn update_status(&mut self, status: BuildStatus) {
        self.status = status;
    }
    
    pub fn add_log_message(&mut self, message: &str) {
        self.log_messages.push(message.to_string());
    }
    
    pub fn get_progress_percentage(&self) -> f32 {
        if self.total_steps == 0 {
            return 0.0;
        }
        (self.current_step_number as f32 / self.total_steps as f32) * 100.0
    }
}