use std::fs::{self, File};
use std::io::Write;
use std::path::Path;
use std::process::Command;
use log::{info, warn, error};
use crate::error::BuilderError;

#[derive(Debug, Clone)]
pub struct DeviceTreeConfig {
    pub build_type: BuildType,
    pub kernel_version: String,
    pub distro_name: String,
    pub distro_version: String,
    pub gpu_driver: String,
    pub enable_av1: bool,
    pub enable_gpu_oc: bool,
    pub target_freq_mhz: u32,
}

#[derive(Debug, Clone, PartialEq)]
pub enum BuildType {
    GameScopePi,
    OpenScopePi,
    KodiMediaCenter,
    DesktopServer,
}

impl Default for DeviceTreeConfig {
    fn default() -> Self {
        Self {
            build_type: BuildType::DesktopServer,
            kernel_version: "6.6".to_string(),
            distro_name: "debian".to_string(),
            distro_version: "12".to_string(),
            gpu_driver: "g13p0".to_string(),
            enable_av1: true,
            enable_gpu_oc: true,
            target_freq_mhz: 1000,
        }
    }
}

pub struct DeviceTreeManager {
    output_dir: String,
}

impl DeviceTreeManager {
    pub fn new() -> Self {
        Self {
            output_dir: "kernel/devicetree".to_string(),
        }
    }

    /// Create device tree output directory
    pub fn init_directory(&self) -> Result<(), BuilderError> {
        fs::create_dir_all(&self.output_dir)
            .map_err(|e| BuilderError::IoError(format!("Failed to create device tree directory: {}", e)))?;
        
        info!("Device tree directory created at: {}", self.output_dir);
        Ok(())
    }

    /// Generate device tree source file for Orange Pi 5 Plus
    pub fn generate_dts(&self, config: &DeviceTreeConfig) -> Result<String, BuilderError> {
        let build_suffix = match config.build_type {
            BuildType::GameScopePi => "gamescope",
            BuildType::OpenScopePi => "openscope", 
            BuildType::KodiMediaCenter => "kodi",
            BuildType::DesktopServer => "desktop",
        };

        let filename = format!(
            "rk3588s-orangepi-5-plus-{}-{}-{}-{}.dts",
            config.distro_name,
            config.distro_version,
            config.kernel_version.replace(".", "-"),
            build_suffix
        );
        
        let dts_path = format!("{}/{}", self.output_dir, filename);
        
        let dts_content = self.create_dts_content(config)?;
        
        let mut file = File::create(&dts_path)
            .map_err(|e| BuilderError::IoError(format!("Failed to create DTS file: {}", e)))?;
        
        file.write_all(dts_content.as_bytes())
            .map_err(|e| BuilderError::IoError(format!("Failed to write DTS content: {}", e)))?;
        
        info!("Generated DTS file: {}", dts_path);
        Ok(dts_path)
    }

    /// Create device tree source content based on configuration
    fn create_dts_content(&self, config: &DeviceTreeConfig) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        // DTS header
        content.push_str("/dts-v1/;\n");
        content.push_str("#include <dt-bindings/gpio/gpio.h>\n");
        content.push_str("#include <dt-bindings/pinctrl/rockchip.h>\n");
        content.push_str("#include \"rk3588s.dtsi\"\n");
        content.push_str("#include \"rk3588s-orangepi-5.dtsi\"\n\n");
        
        // Root node with metadata
        content.push_str("/ {\n");
        content.push_str(&format!("    model = \"Orange Pi 5 Plus - {} {} - Kernel {} - {}\";\n", 
            config.distro_name, config.distro_version, config.kernel_version, 
            self.get_build_type_description(config)));
        content.push_str("    compatible = \"xunlong,orangepi-5-plus\", \"rockchip,rk3588s\";\n\n");
        
        // Build-specific memory configuration
        content.push_str(&self.generate_memory_config(config)?);
        content.push_str("};\n\n");
        
        // GPU configuration
        content.push_str(&self.generate_gpu_config(config)?);
        
        // AV1 decoder configuration (always enabled for media builds)
        if config.enable_av1 || config.build_type == BuildType::KodiMediaCenter {
            content.push_str(&self.generate_av1_config()?);
        }
        
        // Build-specific configurations
        match config.build_type {
            BuildType::GameScopePi => {
                content.push_str(&self.generate_gamescope_config()?);
            },
            BuildType::KodiMediaCenter => {
                content.push_str(&self.generate_kodi_config()?);
            },
            BuildType::OpenScopePi => {
                content.push_str(&self.generate_openscope_config()?);
            },
            BuildType::DesktopServer => {
                content.push_str(&self.generate_desktop_config()?);
            }
        }
        
        // Common peripheral configuration
        content.push_str(&self.generate_peripheral_config()?);
        
        Ok(content)
    }

    fn get_build_type_description(&self, config: &DeviceTreeConfig) -> &str {
        match config.build_type {
            BuildType::GameScopePi => "GameScope-Pi Gaming",
            BuildType::OpenScopePi => "OpenScope-Pi Open Source Gaming",
            BuildType::KodiMediaCenter => "Kodi Media Center",
            BuildType::DesktopServer => "Desktop/Server",
        }
    }

    /// Generate build-specific memory configuration
    fn generate_memory_config(&self, config: &DeviceTreeConfig) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("    memory@0 {\n");
        content.push_str("        device_type = \"memory\";\n");
        content.push_str("        reg = <0x0 0x00000000 0x0 0x80000000>,\n");
        content.push_str("              <0x0 0x100000000 0x1 0x80000000>;\n");
        content.push_str("    };\n\n");
        
        content.push_str("    reserved-memory {\n");
        content.push_str("        #address-cells = <2>;\n");
        content.push_str("        #size-cells = <2>;\n");
        content.push_str("        ranges;\n\n");
        
        // CMA size based on build type
        let cma_size = match config.build_type {
            BuildType::GameScopePi => "0x20000000", // 512MB for gaming
            BuildType::KodiMediaCenter => "0x30000000", // 768MB for 4K video
            BuildType::OpenScopePi => "0x20000000", // 512MB for emulation
            BuildType::DesktopServer => "0x10000000", // 256MB default
        };
        
        content.push_str(&format!("        /* Reserve {}MB for CMA ({}) */\n", 
            match cma_size {
                "0x10000000" => "256",
                "0x20000000" => "512", 
                "0x30000000" => "768",
                _ => "512",
            },
            self.get_build_type_description(config)
        ));
        content.push_str("        linux,cma {\n");
        content.push_str("            compatible = \"shared-dma-pool\";\n");
        content.push_str("            reusable;\n");
        content.push_str(&format!("            size = <0x0 {}>;\n", cma_size));
        content.push_str("            linux,cma-default;\n");
        content.push_str("        };\n");
        content.push_str("    };\n\n");
        
        Ok(content)
    }

    /// Generate GPU-specific device tree configuration
    fn generate_gpu_config(&self, config: &DeviceTreeConfig) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("&gpu {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    mali-supply = <&vdd_gpu_s0>;\n");
        
        // GPU frequency configuration based on build type
        if config.enable_gpu_oc || config.build_type == BuildType::GameScopePi {
            content.push_str("    operating-points-v2 = <&gpu_opp_table_custom>;\n");
        } else {
            content.push_str("    operating-points-v2 = <&gpu_opp_table>;\n");
        }
        
        // Driver-specific configuration
        match config.gpu_driver.as_str() {
            "g13p0" | "g6p0" => {
                content.push_str("    /* Mali proprietary driver configuration */\n");
                content.push_str("    mali,power-policy = \"coarse_demand\";\n");
                
                // Build-specific Mali settings
                match config.build_type {
                    BuildType::GameScopePi => {
                        content.push_str("    mali,js-scheduling-period = <50>; /* Gaming optimized */\n");
                        content.push_str("    mali,power-policy = \"always_on\";\n");
                    },
                    BuildType::KodiMediaCenter => {
                        content.push_str("    mali,js-scheduling-period = <100>; /* Media optimized */\n");
                        content.push_str("    mali,dvfs-period = <200>;\n");
                    },
                    _ => {
                        content.push_str("    mali,js-scheduling-period = <100>;\n");
                    }
                }
                
                content.push_str("    mali,shader-present = <0xff>;\n");
                content.push_str("    mali,tiler-present = <0x1>;\n");
                content.push_str("    mali,l2-present = <0xf>;\n");
            },
            "panfrost" | "mesa-panfrost" => {
                content.push_str("    /* Panfrost open source driver configuration */\n");
                content.push_str("    interrupt-names = \"job\", \"mmu\", \"gpu\";\n");
                content.push_str("    clocks = <&cru CLK_GPU>, <&cru CLK_GPU_COREGROUP>,\n");
                content.push_str("             <&cru CLK_GPU_STACKS>;\n");
                content.push_str("    clock-names = \"gpu\", \"bus\", \"core\";\n");
            },
            _ => {
                content.push_str("    /* Default GPU configuration */\n");
            }
        }
        
        content.push_str("};\n\n");
        
        // Custom GPU OPP table for gaming/media builds
        if config.enable_gpu_oc || config.build_type == BuildType::GameScopePi || config.build_type == BuildType::KodiMediaCenter {
            content.push_str(&self.generate_gpu_opp_table(config)?);
        }
        
        Ok(content)
    }

    /// Generate custom GPU OPP table based on build type
    fn generate_gpu_opp_table(&self, config: &DeviceTreeConfig) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("gpu_opp_table_custom: opp-table-gpu {\n");
        content.push_str("    compatible = \"operating-points-v2\";\n");
        content.push_str("    opp-shared;\n\n");
        
        // Define frequency/voltage pairs based on build type
        let opp_points = match config.build_type {
            BuildType::GameScopePi => vec![
                (300, 675),   // Base frequency
                (400, 700),   
                (600, 750),   
                (800, 850),   
                (1000, 950),  // Max gaming frequency (from GameScope_BuildGuide.txt)
            ],
            BuildType::KodiMediaCenter => vec![
                (300, 675),   // Base frequency
                (400, 700),   
                (600, 750),   // Good for 4K decode
                (800, 850),   // High bitrate 4K
            ],
            BuildType::OpenScopePi => vec![
                (300, 675),   // Conservative for open source
                (400, 700),   
                (600, 750),   
                (800, 850),   // Max for stability
            ],
            _ => vec![
                (300, 675),   
                (600, 750),   
                (800, 850),   
            ],
        };
        
        for (freq_mhz, voltage_mv) in opp_points {
            content.push_str(&format!("    opp-{:09} {{\n", freq_mhz * 1_000_000));
            content.push_str(&format!("        opp-hz = /bits/ 64 <{}>;\n", freq_mhz * 1_000_000));
            content.push_str(&format!("        opp-microvolt = <{}>;\n", voltage_mv * 1000));
            content.push_str("    };\n\n");
        }
        
        content.push_str("};\n\n");
        Ok(content)
    }

    /// Generate AV1 decoder configuration (from the GitHub commit)
    fn generate_av1_config(&self) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("/* AV1 Hardware Decoder Configuration */\n");
        content.push_str("/* Required by RKMPP AV1 hardware decoding in Chromium, FFmpeg and Gstreamer */\n");
        content.push_str("&av1d {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        Ok(content)
    }

    /// Generate GameScope-Pi specific configuration (from GameScope_BuildGuide.txt)
    fn generate_gamescope_config(&self) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("/* GameScope-Pi Gaming Configuration */\n");
        content.push_str("/* Based on GameScope_BuildGuide.txt device tree optimizations */\n");
        
        // HDMI optimization for gaming (from guide)
        content.push_str("&hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&hdmim0_tx0_cec &hdmim0_tx0_hpd &hdmim0_tx0_scl &hdmim0_tx0_sda>;\n");
        content.push_str("};\n\n");
        
        content.push_str("&hdmi0_in_vp0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&route_hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // DisplayPort configuration for dual output
        content.push_str("&dp0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&dp0m2_pins>;\n");
        content.push_str("};\n\n");
        
        content.push_str("&dp0_in_vp2 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        Ok(content)
    }

    /// Generate Kodi Media Center configuration (from Kodi optimized.md)
    fn generate_kodi_config(&self) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("/* Kodi Media Center Configuration */\n");
        content.push_str("/* Based on Kodi optimized.md hardware acceleration setup */\n");
        
        // HDMI configuration for 4K HDR support
        content.push_str("&hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&hdmim0_tx0_cec &hdmim0_tx0_hpd &hdmim0_tx0_scl &hdmim0_tx0_sda>;\n");
        content.push_str("};\n\n");
        
        // CEC support for TV remote control (from guide)
        content.push_str("&hdmi0_cec {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&hdmim0_tx0_cec>;\n");
        content.push_str("};\n\n");
        
        content.push_str("&hdmi0_in_vp0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&route_hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // MPP (Media Process Platform) configuration for hardware decode
        content.push_str("&mpp_srv {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // RGA (Rockchip Graphics Accelerator) for video processing
        content.push_str("&rga3_core0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&rga3_core1 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&rga2 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // Video decoder nodes for H.264/H.265/VP9 support
        content.push_str("&vdpu121 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&vdpu_vp9 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&vepu121_0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&vepu121_1 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // I2S for audio output
        content.push_str("&i2s0_8ch {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    rockchip,clk-trcm = <1>;\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&i2s0_lrck &i2s0_sclk &i2s0_sdi0 &i2s0_sdo0>;\n");
        content.push_str("};\n\n");
        
        Ok(content)
    }

    /// Generate OpenScope-Pi configuration (from emulation_opensource_drivers.md)
    fn generate_openscope_config(&self) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("/* OpenScope-Pi Open Source Gaming Configuration */\n");
        content.push_str("/* Based on emulation_opensource_drivers.md open source setup */\n");
        
        // Basic HDMI for open source compatibility
        content.push_str("&hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&hdmim0_tx0_cec &hdmim0_tx0_hpd &hdmim0_tx0_scl &hdmim0_tx0_sda>;\n");
        content.push_str("};\n\n");
        
        content.push_str("&hdmi0_in_vp0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&route_hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // Open source VPU support for Mesa/Panfrost
        content.push_str("&vpu {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        Ok(content)
    }

    /// Generate desktop/server configuration
    fn generate_desktop_config(&self) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        content.push_str("/* Desktop/Server Configuration */\n");
        
        // Standard HDMI output
        content.push_str("&hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&hdmim0_tx0_cec &hdmim0_tx0_hpd &hdmim0_tx0_scl &hdmim0_tx0_sda>;\n");
        content.push_str("};\n\n");
        
        content.push_str("&hdmi0_in_vp0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&route_hdmi0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // DisplayPort for multi-monitor setups
        content.push_str("&dp0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    pinctrl-names = \"default\";\n");
        content.push_str("    pinctrl-0 = <&dp0m2_pins>;\n");
        content.push_str("};\n\n");
        
        content.push_str("&dp0_in_vp2 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        Ok(content)
    }

    /// Generate peripheral device configuration
    fn generate_peripheral_config(&self) -> Result<String, BuilderError> {
        let mut content = String::new();
        
        // USB configuration
        content.push_str("/* USB Configuration */\n");
        content.push_str("&usb_host0_ehci {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&usb_host0_ohci {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&usb_host1_ehci {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&usb_host1_ohci {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // Ethernet configuration
        content.push_str("/* Ethernet Configuration */\n");
        content.push_str("&gmac1 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    phy-mode = \"rgmii\";\n");
        content.push_str("    clock_in_out = \"output\";\n");
        content.push_str("    tx_delay = <0x43>;\n");
        content.push_str("    rx_delay = <0x43>;\n");
        content.push_str("};\n\n");
        
        // I2C configuration
        content.push_str("/* I2C Configuration */\n");
        content.push_str("&i2c0 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        content.push_str("&i2c2 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("};\n\n");
        
        // SPI configuration  
        content.push_str("/* SPI Configuration */\n");
        content.push_str("&spi2 {\n");
        content.push_str("    status = \"okay\";\n");
        content.push_str("    max-freq = <50000000>;\n");
        content.push_str("};\n\n");
        
        Ok(content)
    }

    /// Compile device tree source to binary
    pub fn compile_dts(&self, dts_path: &str) -> Result<String, BuilderError> {
        let dtb_path = dts_path.replace(".dts", ".dtb");
        
        info!("Compiling device tree: {} -> {}", dts_path, dtb_path);
        
        let args = vec![
            "-@",          // Enable generation of symbols
            "-I", "dts",   // Input format
            "-O", "dtb",   // Output format  
            "-o", &dtb_path,
            dts_path
        ];
        
        let output = crate::execute_command_with_logging("dtc", &args);
        
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            let error_msg = format!("DTC compilation failed: {}", stderr);
            crate::ui::logger::log_error(&error_msg);
            return Err(BuilderError::SystemCommandFailed(error_msg, output.status.code().unwrap_or(-1)));
        }
        
        info!("Device tree compiled successfully: {}", dtb_path);
        crate::ui::logger::log_info(&format!("Device tree compiled successfully: {}", dtb_path));
        Ok(dtb_path)
    }

    /// Generate all device tree variants for supported configurations
    pub fn generate_all_variants(&self) -> Result<Vec<String>, BuilderError> {
        let mut generated_files = Vec::new();
        
        // Kernel versions supported (from project analysis)
        let kernels = vec!["5.10.160", "6.1", "6.6", "6.8"];
        
        // Distro combinations (from wizard analysis)
        let distros = vec![
            ("debian", "11"),
            ("debian", "12"), 
            ("debian", "13"),
            ("ubuntu", "22.04"),
            ("ubuntu", "24.04"),
        ];
        
        // Build types with appropriate GPU drivers
        let build_configs = vec![
            (BuildType::GameScopePi, vec!["g13p0", "g6p0"]),
            (BuildType::KodiMediaCenter, vec!["g13p0", "g6p0"]),
            (BuildType::OpenScopePi, vec!["panfrost", "mesa-panfrost"]),
            (BuildType::DesktopServer, vec!["g13p0", "g6p0", "panfrost"]),
        ];
        
        for kernel in &kernels {
            for (distro_name, distro_version) in &distros {
                for (build_type, gpu_drivers) in &build_configs {
                    for gpu_driver in gpu_drivers {
                        let config = DeviceTreeConfig {
                            build_type: build_type.clone(),
                            kernel_version: kernel.to_string(),
                            distro_name: distro_name.to_string(),
                            distro_version: distro_version.to_string(),
                            gpu_driver: gpu_driver.to_string(),
                            enable_av1: true,
                            enable_gpu_oc: *gpu_driver != "panfrost" && *gpu_driver != "mesa-panfrost",
                            target_freq_mhz: match build_type {
                                BuildType::GameScopePi => 1000,
                                BuildType::KodiMediaCenter => 800,
                                _ => if *gpu_driver == "panfrost" || *gpu_driver == "mesa-panfrost" { 600 } else { 800 },
                            },
                        };
                        
                        match self.generate_dts(&config) {
                            Ok(dts_path) => {
                                info!("Generated DTS: {:?} {} {} with {} kernel and {} GPU", 
                                      build_type, distro_name, distro_version, kernel, gpu_driver);
                                generated_files.push(dts_path);
                            },
                            Err(e) => {
                                warn!("Failed to generate DTS for {:?} {} {} with {} kernel: {}", 
                                      build_type, distro_name, distro_version, kernel, e);
                            }
                        }
                    }
                }
            }
        }
        
        info!("Generated {} device tree files", generated_files.len());
        Ok(generated_files)
    }

    /// Check if dtc (device tree compiler) is available
    pub fn check_dtc_available(&self) -> bool {
        Command::new("dtc")
            .arg("--version")
            .output()
            .map(|output| output.status.success())
            .unwrap_or(false)
    }

    /// Install device tree compiler if not available
    pub fn install_dtc(&self) -> Result<(), BuilderError> {
        info!("Installing device tree compiler...");
        
        let args = vec!["install", "-y", "device-tree-compiler"];
        let output = crate::execute_command_with_logging("apt", &args);
        
        if !output.status.success() {
            let stderr = String::from_utf8_lossy(&output.stderr);
            let error_msg = format!("Failed to install dtc: {}", stderr);
            crate::ui::logger::log_error(&error_msg);
            return Err(BuilderError::SystemCommandFailed(error_msg, output.status.code().unwrap_or(-1)));
        }
        
        info!("Device tree compiler installed successfully");
        Ok(())
    }

    /// Generate device tree for specific build configuration
    pub fn generate_for_build(&self, kernel: &str, distro_name: &str, distro_version: &str, gpu_driver: &str, build_type: &str) -> Result<String, BuilderError> {
        let build_type_enum = match build_type {
            "gamescope-pi" => BuildType::GameScopePi,
            "openscope-pi" => BuildType::OpenScopePi,
            "kodi" => BuildType::KodiMediaCenter,
            _ => BuildType::DesktopServer,
        };

        let config = DeviceTreeConfig {
            build_type: build_type_enum.clone(),
            kernel_version: kernel.to_string(),
            distro_name: distro_name.to_string(),
            distro_version: distro_version.to_string(),
            gpu_driver: gpu_driver.to_string(),
            enable_av1: true,
            enable_gpu_oc: gpu_driver != "panfrost" && gpu_driver != "mesa-panfrost",
            target_freq_mhz: match build_type_enum {
                BuildType::GameScopePi => 1000,
                BuildType::KodiMediaCenter => 800,
                _ => if gpu_driver == "panfrost" || gpu_driver == "mesa-panfrost" { 600 } else { 800 },
            },
        };

        self.generate_dts(&config)
    }
}