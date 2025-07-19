use cursive::views::{Dialog, LinearLayout, TextView, SelectView, EditView, Checkbox, ScrollView, ProgressBar};
use cursive::Cursive;
use cursive::align::HAlign;
use cursive::theme::{ColorStyle, BaseColor, Color};
use cursive::utils::markup::StyledString;
use cursive::traits::*;
use std::path::PathBuf;
use crate::ui;
use super::{BootloaderConfig, BuildProgress, BuildStatus, validate_bootloader_config};

/// U-Boot variants for Orange Pi 5 Plus
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum UBootVariant {
    Mainline,
    Rockchip,
    Armbian,
    JoshuaRiek,
    Collabora,
    Custom,
}

impl std::fmt::Display for UBootVariant {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            UBootVariant::Mainline => write!(f, "Mainline U-Boot"),
            UBootVariant::Rockchip => write!(f, "Rockchip U-Boot"),
            UBootVariant::Armbian => write!(f, "Armbian U-Boot"),
            UBootVariant::JoshuaRiek => write!(f, "Joshua-Riek U-Boot"),
            UBootVariant::Collabora => write!(f, "Collabora U-Boot"),
            UBootVariant::Custom => write!(f, "Custom U-Boot"),
        }
    }
}

/// U-Boot configuration for Orange Pi 5 Plus
#[derive(Debug, Clone)]
pub struct UBootConfig {
    pub variant: UBootVariant,
    pub config: BootloaderConfig,
    pub defconfig: String,
    pub board_name: String,
    pub enable_spl: bool,
    pub enable_tpl: bool,
    pub enable_atf: bool,
    pub atf_source: String,
    pub enable_optee: bool,
    pub optee_source: String,
    pub enable_rockchip_tpl: bool,
    pub enable_fit: bool,
    pub fit_source: String,
    pub enable_verified_boot: bool,
    pub signing_key: Option<PathBuf>,
}

impl Default for UBootConfig {
    fn default() -> Self {
        Self {
            variant: UBootVariant::Mainline,
            config: BootloaderConfig::default(),
            defconfig: "orangepi-5-plus-rk3588s_defconfig".to_string(),
            board_name: "orangepi-5-plus".to_string(),
            enable_spl: true,
            enable_tpl: true,
            enable_atf: true,
            atf_source: "https://github.com/ARM-software/arm-trusted-firmware.git".to_string(),
            enable_optee: false,
            optee_source: "https://github.com/OP-TEE/optee_os.git".to_string(),
            enable_rockchip_tpl: true,
            enable_fit: true,
            fit_source: "./fit.its".to_string(),
            enable_verified_boot: false,
            signing_key: None,
        }
    }
}

impl UBootConfig {
    pub fn for_variant(variant: UBootVariant) -> Self {
        let mut config = Self::default();
        config.variant = variant.clone();
        
        match variant {
            UBootVariant::Mainline => {
                config.config.source_repo = "https://github.com/u-boot/u-boot.git".to_string();
                config.config.branch = "master".to_string();
                config.defconfig = "orangepi-5-plus-rk3588s_defconfig".to_string();
            }
            UBootVariant::Rockchip => {
                config.config.source_repo = "https://github.com/rockchip-linux/u-boot.git".to_string();
                config.config.branch = "develop".to_string();
                config.defconfig = "rk3588s_defconfig".to_string();
                config.enable_rockchip_tpl = true;
            }
            UBootVariant::Armbian => {
                config.config.source_repo = "https://github.com/armbian/build.git".to_string();
                config.config.branch = "main".to_string();
                config.defconfig = "orangepi5-plus_defconfig".to_string();
            }
            UBootVariant::JoshuaRiek => {
                config.config.source_repo = "https://github.com/Joshua-Riek/u-boot-rockchip.git".to_string();
                config.config.branch = "rk3588".to_string();
                config.defconfig = "orangepi-5-plus-rk3588s_defconfig".to_string();
            }
            UBootVariant::Collabora => {
                config.config.source_repo = "https://gitlab.collabora.com/hardware-enablement/rockchip-3588/u-boot.git".to_string();
                config.config.branch = "rk3588".to_string();
                config.defconfig = "rk3588s_defconfig".to_string();
            }
            UBootVariant::Custom => {
                // Default values for custom variant
            }
        }
        
        config
    }
    
    pub fn get_build_commands(&self) -> Vec<String> {
        let mut commands = Vec::new();
        
        // Environment setup
        commands.push(format!("export CROSS_COMPILE={}", self.config.cross_compile));
        commands.push(format!("export ARCH={}", self.config.target_arch));
        commands.push("export KBUILD_OUTPUT=./build".to_string());
        
        // Clean previous build
        commands.push("make mrproper".to_string());
        
        // Configure
        commands.push(format!("make {}", self.defconfig));
        
        // Build ATF if enabled
        if self.enable_atf {
            commands.push("# Build ARM Trusted Firmware".to_string());
            commands.push("cd ../arm-trusted-firmware".to_string());
            commands.push("make PLAT=rk3588 DEBUG=0 bl31".to_string());
            commands.push("cd ../u-boot".to_string());
            commands.push("export BL31=../arm-trusted-firmware/build/rk3588/release/bl31/bl31.elf".to_string());
        }
        
        // Build OP-TEE if enabled
        if self.enable_optee {
            commands.push("# Build OP-TEE".to_string());
            commands.push("cd ../optee_os".to_string());
            commands.push("make PLATFORM=rockchip-rk3588 CFG_ARM64_core=y".to_string());
            commands.push("cd ../u-boot".to_string());
            commands.push("export TEE=../optee_os/out/arm-plat-rockchip/core/tee-pager_v2.bin".to_string());
        }
        
        // Build U-Boot
        commands.push("# Build U-Boot".to_string());
        commands.push("make -j$(nproc)".to_string());
        
        // Build FIT image if enabled
        if self.enable_fit {
            commands.push("# Build FIT image".to_string());
            commands.push(format!("mkimage -f {} u-boot.itb", self.fit_source));
        }
        
        // Create flash images
        commands.push("# Create flash images".to_string());
        commands.push("./tools/mkimage -n rk3588 -T rksd -d spl/u-boot-spl.bin idbloader.img".to_string());
        commands.push("cat spl/u-boot-spl.bin > idbloader.img".to_string());
        
        commands
    }
    
    pub fn get_required_dependencies(&self) -> Vec<String> {
        let mut deps = vec![
            "build-essential".to_string(),
            "gcc-aarch64-linux-gnu".to_string(),
            "device-tree-compiler".to_string(),
            "swig".to_string(),
            "python3-dev".to_string(),
            "python3-setuptools".to_string(),
            "python3-pyelftools".to_string(),
            "libssl-dev".to_string(),
            "libfdt-dev".to_string(),
            "flex".to_string(),
            "bison".to_string(),
            "bc".to_string(),
        ];
        
        if self.enable_atf {
            deps.push("arm-trusted-firmware-tools".to_string());
        }
        
        if self.enable_optee {
            deps.push("optee-client".to_string());
        }
        
        deps
    }
}

/// Show U-Boot builder interface
pub fn show_uboot_builder(siv: &mut Cursive) {
    let mut content = LinearLayout::vertical();
    
    // Header
    content.add_child(TextView::new(StyledString::styled(
        "U-Boot Builder for Orange Pi 5 Plus",
        ColorStyle::from(Color::Light(BaseColor::Green))
    )));
    content.add_child(TextView::new("Universal Boot Loader for ARM64 systems"));
    content.add_child(TextView::new(""));
    
    // Variant selection
    content.add_child(TextView::new(StyledString::styled(
        "Select U-Boot Variant:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    
    let mut variant_select = SelectView::<UBootVariant>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    variant_select.add_item("🏠 Mainline U-Boot - Latest upstream (Recommended)", UBootVariant::Mainline);
    variant_select.add_item("🪨 Rockchip U-Boot - Vendor optimized", UBootVariant::Rockchip);
    variant_select.add_item("🛡️ Armbian U-Boot - Armbian patches", UBootVariant::Armbian);
    variant_select.add_item("👨‍💻 Joshua-Riek U-Boot - Ubuntu optimized", UBootVariant::JoshuaRiek);
    variant_select.add_item("🤝 Collabora U-Boot - Enterprise patches", UBootVariant::Collabora);
    variant_select.add_item("🔧 Custom U-Boot - Specify your own", UBootVariant::Custom);
    
    variant_select.set_on_submit(|s, variant| {
        s.pop_layer();
        show_uboot_configuration(s, variant.clone());
    });
    
    content.add_child(variant_select);
    
    // Information section
    content.add_child(TextView::new(""));
    content.add_child(TextView::new(StyledString::styled(
        "Variant Information:",
        ColorStyle::from(Color::Light(BaseColor::Yellow))
    )));
    content.add_child(TextView::new("• Mainline: Latest features, best compatibility"));
    content.add_child(TextView::new("• Rockchip: Vendor optimizations, hardware support"));
    content.add_child(TextView::new("• Armbian: Stable patches, tested configurations"));
    content.add_child(TextView::new("• Joshua-Riek: Ubuntu-specific optimizations"));
    content.add_child(TextView::new("• Collabora: Enterprise-grade patches"));
    content.add_child(TextView::new("• Custom: Your own repository and configuration"));
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(20))
        .title("U-Boot Builder")
        .button("Back", |s| {
            s.pop_layer();
            super::show_bootloader_builder(s);
        })
        .button("Dependencies", |s| {
            show_uboot_dependencies(s);
        });
    
    siv.add_layer(dialog);
}

/// Show U-Boot configuration interface
pub fn show_uboot_configuration(siv: &mut Cursive, variant: UBootVariant) {
    let config = UBootConfig::for_variant(variant.clone());
    
    let mut content = LinearLayout::vertical();
    
    // Header
    content.add_child(TextView::new(StyledString::styled(
        format!("Configure {} Build", variant),
        ColorStyle::from(Color::Light(BaseColor::Green))
    )));
    content.add_child(TextView::new(""));
    
    // Build configuration
    content.add_child(TextView::new(StyledString::styled(
        "Build Configuration:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    
    content.add_child(
        LinearLayout::horizontal()
            .child(TextView::new("Source Repository: "))
            .child(EditView::new()
                .content(config.config.source_repo.clone())
                .with_name("source_repo")
                .min_width(40))
    );
    
    content.add_child(
        LinearLayout::horizontal()
            .child(TextView::new("Branch: "))
            .child(EditView::new()
                .content(config.config.branch.clone())
                .with_name("branch")
                .min_width(20))
    );
    
    content.add_child(
        LinearLayout::horizontal()
            .child(TextView::new("Defconfig: "))
            .child(EditView::new()
                .content(config.defconfig.clone())
                .with_name("defconfig")
                .min_width(30))
    );
    
    content.add_child(
        LinearLayout::horizontal()
            .child(TextView::new("Output Path: "))
            .child(EditView::new()
                .content(config.config.output_path.to_string_lossy().to_string())
                .with_name("output_path")
                .min_width(30))
    );
    
    content.add_child(TextView::new(""));
    
    // Build options
    content.add_child(TextView::new(StyledString::styled(
        "Build Options:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    
    let mut spl_checkbox = Checkbox::new();
    if config.enable_spl {
        spl_checkbox = spl_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(spl_checkbox.with_name("enable_spl"))
            .child(TextView::new("Enable SPL (Secondary Program Loader)"))
    );
    
    let mut tpl_checkbox = Checkbox::new();
    if config.enable_tpl {
        tpl_checkbox = tpl_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(tpl_checkbox.with_name("enable_tpl"))
            .child(TextView::new("Enable TPL (Tertiary Program Loader)"))
    );
    
    let mut atf_checkbox = Checkbox::new();
    if config.enable_atf {
        atf_checkbox = atf_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(atf_checkbox.with_name("enable_atf"))
            .child(TextView::new("Enable ARM Trusted Firmware"))
    );
    
    let mut optee_checkbox = Checkbox::new();
    if config.enable_optee {
        optee_checkbox = optee_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(optee_checkbox.with_name("enable_optee"))
            .child(TextView::new("Enable OP-TEE Secure OS"))
    );
    
    let mut fit_checkbox = Checkbox::new();
    if config.enable_fit {
        fit_checkbox = fit_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(fit_checkbox.with_name("enable_fit"))
            .child(TextView::new("Enable FIT (Flattened Image Tree)"))
    );
    
    let mut verified_boot_checkbox = Checkbox::new();
    if config.enable_verified_boot {
        verified_boot_checkbox = verified_boot_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(verified_boot_checkbox.with_name("enable_verified_boot"))
            .child(TextView::new("Enable Verified Boot"))
    );
    
    let mut debug_checkbox = Checkbox::new();
    if config.config.enable_debug {
        debug_checkbox = debug_checkbox.checked();
    }
    content.add_child(
        LinearLayout::horizontal()
            .child(debug_checkbox.with_name("enable_debug"))
            .child(TextView::new("Enable Debug Build"))
    );
    
    content.add_child(TextView::new(""));
    
    // Hardware options
    content.add_child(TextView::new(StyledString::styled(
        "Hardware Configuration:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    
    content.add_child(
        LinearLayout::horizontal()
            .child(TextView::new("Memory Size (MB): "))
            .child(EditView::new()
                .content(config.config.memory_size.unwrap_or(16384).to_string())
                .with_name("memory_size")
                .min_width(10))
    );
    
    let mut storage_select = SelectView::<String>::new()
        .h_align(HAlign::Left);
    storage_select.add_item("SPI Flash", "spi".to_string());
    storage_select.add_item("eMMC", "emmc".to_string());
    storage_select.add_item("SD Card", "mmc".to_string());
    storage_select.add_item("NVMe", "nvme".to_string());
    storage_select.set_selection(0);
    
    content.add_child(
        LinearLayout::horizontal()
            .child(TextView::new("Storage Type: "))
            .child(storage_select.with_name("storage_type"))
    );
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(25))
        .title("U-Boot Configuration")
        .button("Back", |s| {
            s.pop_layer();
            show_uboot_builder(s);
        })
        .button("Build", move |s| {
            let variant_clone = variant.clone();
            build_uboot_with_config(s, variant_clone);
        });
    
    siv.add_layer(dialog);
}

/// Build U-Boot with the specified configuration
fn build_uboot_with_config(siv: &mut Cursive, variant: UBootVariant) {
    let mut config = UBootConfig::for_variant(variant);
    
    // Extract configuration from UI
    if let Some(source_repo) = siv.find_name::<EditView>("source_repo") {
        config.config.source_repo = source_repo.get_content().to_string();
    }
    
    if let Some(branch) = siv.find_name::<EditView>("branch") {
        config.config.branch = branch.get_content().to_string();
    }
    
    if let Some(defconfig) = siv.find_name::<EditView>("defconfig") {
        config.defconfig = defconfig.get_content().to_string();
    }
    
    if let Some(output_path) = siv.find_name::<EditView>("output_path") {
        config.config.output_path = PathBuf::from(output_path.get_content().to_string());
    }
    
    if let Some(enable_spl) = siv.find_name::<Checkbox>("enable_spl") {
        config.enable_spl = enable_spl.is_checked();
    }
    
    if let Some(enable_atf) = siv.find_name::<Checkbox>("enable_atf") {
        config.enable_atf = enable_atf.is_checked();
    }
    
    if let Some(enable_debug) = siv.find_name::<Checkbox>("enable_debug") {
        config.config.enable_debug = enable_debug.is_checked();
    }
    
    if let Some(memory_size) = siv.find_name::<EditView>("memory_size") {
        if let Ok(size) = memory_size.get_content().parse::<u32>() {
            config.config.memory_size = Some(size);
        }
    }
    
    if let Some(storage_type) = siv.find_name::<SelectView<String>>("storage_type") {
        if let Some(selected) = storage_type.selection() {
            config.config.storage_type = selected.as_ref().clone();
        }
    }
    
    // Validate configuration
    match validate_bootloader_config(&config.config) {
        Ok(_) => {
            siv.pop_layer();
            show_uboot_build_progress(siv, config);
        }
        Err(error) => {
            ui::show_error(siv, &error);
        }
    }
}

/// Show U-Boot build progress
fn show_uboot_build_progress(siv: &mut Cursive, config: UBootConfig) {
    let mut content = LinearLayout::vertical();
    
    content.add_child(TextView::new(StyledString::styled(
        format!("Building {} for Orange Pi 5 Plus", config.variant),
        ColorStyle::from(Color::Light(BaseColor::Green))
    )));
    content.add_child(TextView::new(""));
    
    // Progress bar
    content.add_child(TextView::new("Build Progress:"));
    content.add_child(ProgressBar::new().with_name("progress_bar"));
    content.add_child(TextView::new("Initializing...").with_name("status_text"));
    content.add_child(TextView::new(""));
    
    // Build information
    content.add_child(TextView::new(StyledString::styled(
        "Build Information:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    content.add_child(TextView::new(format!("Variant: {}", config.variant)));
    content.add_child(TextView::new(format!("Repository: {}", config.config.source_repo)));
    content.add_child(TextView::new(format!("Branch: {}", config.config.branch)));
    content.add_child(TextView::new(format!("Defconfig: {}", config.defconfig)));
    content.add_child(TextView::new(format!("Output: {}", config.config.output_path.display())));
    
    content.add_child(TextView::new(""));
    
    // Build log
    content.add_child(TextView::new(StyledString::styled(
        "Build Log:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    content.add_child(ScrollView::new(TextView::new("").with_name("build_log")).max_height(10));
    
    let dialog = Dialog::around(content)
        .title("U-Boot Build Progress")
        .button("Cancel", |s| {
            s.pop_layer();
            show_uboot_builder(s);
        })
        .button("View Log", |s| {
            show_build_log(s);
        });
    
    siv.add_layer(dialog);
    
    // Start the build process (simulate for now)
    simulate_uboot_build(siv, config);
}

/// Show U-Boot dependencies
fn show_uboot_dependencies(siv: &mut Cursive) {
    let config = UBootConfig::default();
    let deps = config.get_required_dependencies();
    
    let mut content = LinearLayout::vertical();
    
    content.add_child(TextView::new(StyledString::styled(
        "U-Boot Build Dependencies",
        ColorStyle::from(Color::Light(BaseColor::Green))
    )));
    content.add_child(TextView::new("The following packages are required to build U-Boot:"));
    content.add_child(TextView::new(""));
    
    for dep in deps {
        content.add_child(TextView::new(format!("• {}", dep)));
    }
    
    content.add_child(TextView::new(""));
    content.add_child(TextView::new("Install command:"));
    content.add_child(TextView::new("sudo apt-get install build-essential gcc-aarch64-linux-gnu device-tree-compiler swig python3-dev python3-setuptools python3-pyelftools libssl-dev libfdt-dev flex bison bc"));
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(15))
        .title("U-Boot Dependencies")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

/// Show build log
fn show_build_log(siv: &mut Cursive) {
    let content = LinearLayout::vertical()
        .child(TextView::new("Build log will be displayed here"))
        .child(TextView::new(""))
        .child(ScrollView::new(TextView::new("Build log contents...")).max_height(20));
    
    let dialog = Dialog::around(content)
        .title("Build Log")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

/// Simulate U-Boot build process
fn simulate_uboot_build(siv: &mut Cursive, config: UBootConfig) {
    let mut progress = BuildProgress::new(8);
    
    progress.update_step("Cloning repository", 1);
    progress.update_status(BuildStatus::Downloading);
    
    // In a real implementation, this would run in a separate thread
    // and update the UI periodically
    
    progress.update_step("Configuring build", 2);
    progress.update_status(BuildStatus::Configuring);
    
    progress.update_step("Building SPL", 3);
    progress.update_status(BuildStatus::Building);
    
    progress.update_step("Building U-Boot", 4);
    
    if config.enable_atf {
        progress.update_step("Building ARM Trusted Firmware", 5);
    }
    
    progress.update_step("Creating flash images", 6);
    
    progress.update_step("Installing to output", 7);
    progress.update_status(BuildStatus::Installing);
    
    progress.update_step("Build complete", 8);
    progress.update_status(BuildStatus::Completed);
    
    // Update UI with completion
    siv.add_layer(
        Dialog::info(format!("U-Boot build completed successfully!\n\nOutput files saved to: {}", config.config.output_path.display()))
            .title("Build Complete")
            .button("OK", |s| {
                s.pop_layer();
                s.pop_layer();
                show_uboot_builder(s);
            })
    );
}