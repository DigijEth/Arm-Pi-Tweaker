use anyhow::Result;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::path::PathBuf;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LcceConfig {
    pub kernel_config: KernelConfig,
    pub video_config: VideoConfig,
    pub emulation_config: EmulationConfig,
    pub storage_config: StorageConfig,
    pub armpi_tweaker_config: ArmpiTweakerConfig,
    pub installation_config: InstallationConfig,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct KernelConfig {
    pub selected_kernel: String,
    pub kernel_modules: Vec<String>,
    pub disabled_modules: Vec<String>,
    pub custom_boot_parameters: String,
    pub device_tree_overlays: Vec<String>,
    pub kernel_source: KernelSource,
    pub custom_patches: Vec<PatchConfig>,
    pub compilation_options: CompilationOptions,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum KernelSource {
    Armbian { version: String, variant: String },
    Mainline { version: String },
    Vendor { version: String, source: String },
    Custom { path: PathBuf },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PatchConfig {
    pub name: String,
    pub description: String,
    pub patch_file: PathBuf,
    pub enabled: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CompilationOptions {
    pub optimization_level: String,
    pub debug_symbols: bool,
    pub custom_cflags: String,
    pub parallel_jobs: u32,
    pub cross_compile_toolchain: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VideoConfig {
    pub driver_type: VideoDriverType,
    pub mpp_enabled: bool,
    pub hardware_acceleration: HardwareAcceleration,
    pub vulkan_support: bool,
    pub opencl_support: bool,
    pub custom_driver_packages: Vec<String>,
    pub performance_profile: PerformanceProfile,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum VideoDriverType {
    Mali { variant: String, version: String },
    Panfrost { mesa_version: String },
    Custom { driver_path: PathBuf },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HardwareAcceleration {
    pub video_decode: Vec<String>, // H.264, H.265, VP9, etc.
    pub video_encode: Vec<String>,
    pub audio_acceleration: bool,
    pub ai_acceleration: bool,
    pub network_acceleration: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum PerformanceProfile {
    PowerSave,
    Balanced,
    Performance,
    Gaming,
    Custom { gpu_freq: u32, memory_freq: u32 },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EmulationConfig {
    pub frontend: Option<EmulationFrontend>,
    pub auto_start: bool,
    pub installed_emulators: Vec<EmulatorConfig>,
    pub media_center: Option<MediaCenterConfig>,
    pub custom_applications: Vec<CustomApp>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum EmulationFrontend {
    RetroPie,
    EmulationStation,
    Kodi,
    Custom { name: String, executable: String },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EmulatorConfig {
    pub name: String,
    pub system: String,
    pub core: String,
    pub enabled: bool,
    pub custom_config: HashMap<String, String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MediaCenterConfig {
    pub software: MediaCenterSoftware,
    pub auto_start: bool,
    pub plugins: Vec<String>,
    pub media_directories: Vec<PathBuf>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum MediaCenterSoftware {
    Kodi { version: String },
    Jellyfin { version: String },
    Plex { version: String },
    Custom { name: String, package: String },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CustomApp {
    pub name: String,
    pub package: String,
    pub auto_start: bool,
    pub desktop_entry: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StorageConfig {
    pub target_device: String,
    pub partition_scheme: PartitionScheme,
    pub filesystem_type: String,
    pub encryption_enabled: bool,
    pub compression_enabled: bool,
    pub backup_source: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum PartitionScheme {
    Simple { boot_size_mb: u32, swap_size_mb: u32 },
    Advanced { partitions: Vec<PartitionConfig> },
    Custom { script: PathBuf },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PartitionConfig {
    pub mount_point: String,
    pub size_mb: Option<u32>, // None means use remaining space
    pub filesystem: String,
    pub flags: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ArmpiTweakerConfig {
    pub gpu_optimizations: bool,
    pub cpu_governor: String,
    pub network_optimizations: bool,
    pub system_tweaks: HashMap<String, String>,
    pub security_hardening: bool,
    pub custom_services: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InstallationConfig {
    pub installation_mode: InstallationMode,
    pub verify_checksums: bool,
    pub create_backup: bool,
    pub post_install_scripts: Vec<PathBuf>,
    pub cleanup_after_install: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum InstallationMode {
    Clone, // Direct clone of current system
    Fresh, // Fresh installation with configurations
    Hybrid, // Clone + modifications
}

impl Default for LcceConfig {
    fn default() -> Self {
        Self {
            kernel_config: KernelConfig::default(),
            video_config: VideoConfig::default(),
            emulation_config: EmulationConfig::default(),
            storage_config: StorageConfig::default(),
            armpi_tweaker_config: ArmpiTweakerConfig::default(),
            installation_config: InstallationConfig::default(),
        }
    }
}

impl Default for KernelConfig {
    fn default() -> Self {
        Self {
            selected_kernel: "current".to_string(),
            kernel_modules: vec![
                "mali_kbase".to_string(),
                "rockchip_mpp".to_string(),
                "rockchip_rga".to_string(),
                "rockchip_iep".to_string(),
            ],
            disabled_modules: vec![],
            custom_boot_parameters: "".to_string(),
            device_tree_overlays: vec![],
            kernel_source: KernelSource::Armbian {
                version: "6.1.75".to_string(),
                variant: "current".to_string(),
            },
            custom_patches: vec![],
            compilation_options: CompilationOptions::default(),
        }
    }
}

impl Default for CompilationOptions {
    fn default() -> Self {
        Self {
            optimization_level: "-O2".to_string(),
            debug_symbols: false,
            custom_cflags: "".to_string(),
            parallel_jobs: num_cpus::get() as u32,
            cross_compile_toolchain: "aarch64-linux-gnu-".to_string(),
        }
    }
}

impl Default for VideoConfig {
    fn default() -> Self {
        Self {
            driver_type: VideoDriverType::Mali {
                variant: "g610".to_string(),
                version: "latest".to_string(),
            },
            mpp_enabled: true,
            hardware_acceleration: HardwareAcceleration {
                video_decode: vec!["H.264".to_string(), "H.265".to_string()],
                video_encode: vec!["H.264".to_string()],
                audio_acceleration: true,
                ai_acceleration: true,
                network_acceleration: false,
            },
            vulkan_support: true,
            opencl_support: true,
            custom_driver_packages: vec![],
            performance_profile: PerformanceProfile::Balanced,
        }
    }
}

impl Default for EmulationConfig {
    fn default() -> Self {
        Self {
            frontend: None,
            auto_start: false,
            installed_emulators: vec![],
            media_center: None,
            custom_applications: vec![],
        }
    }
}

impl Default for StorageConfig {
    fn default() -> Self {
        Self {
            target_device: "/dev/nvme0n1".to_string(),
            partition_scheme: PartitionScheme::Simple {
                boot_size_mb: 512,
                swap_size_mb: 2048,
            },
            filesystem_type: "ext4".to_string(),
            encryption_enabled: false,
            compression_enabled: false,
            backup_source: true,
        }
    }
}

impl Default for ArmpiTweakerConfig {
    fn default() -> Self {
        Self {
            gpu_optimizations: true,
            cpu_governor: "ondemand".to_string(),
            network_optimizations: false,
            system_tweaks: HashMap::new(),
            security_hardening: false,
            custom_services: vec![],
        }
    }
}

impl Default for InstallationConfig {
    fn default() -> Self {
        Self {
            installation_mode: InstallationMode::Hybrid,
            verify_checksums: true,
            create_backup: true,
            post_install_scripts: vec![],
            cleanup_after_install: true,
        }
    }
}

impl LcceConfig {
    pub fn save_to_file(&self, path: &PathBuf) -> Result<()> {
        let config_json = serde_json::to_string_pretty(self)?;
        std::fs::write(path, config_json)?;
        Ok(())
    }
    
    pub fn load_from_file(path: &PathBuf) -> Result<Self> {
        let config_data = std::fs::read_to_string(path)?;
        let config: Self = serde_json::from_str(&config_data)?;
        Ok(config)
    }
    
    pub fn validate(&self) -> Result<Vec<String>> {
        let mut warnings = Vec::new();
        
        // Validate kernel config
        if self.kernel_config.selected_kernel.is_empty() {
            warnings.push("No kernel selected".to_string());
        }
        
        // Validate storage config
        if !std::path::Path::new(&self.storage_config.target_device).exists() {
            warnings.push(format!("Target device {} does not exist", self.storage_config.target_device));
        }
        
        // Check for conflicting configurations
        if matches!(self.video_config.driver_type, VideoDriverType::Mali { .. }) && 
           matches!(self.video_config.driver_type, VideoDriverType::Panfrost { .. }) {
            warnings.push("Conflicting video driver configuration".to_string());
        }
        
        Ok(warnings)
    }
}