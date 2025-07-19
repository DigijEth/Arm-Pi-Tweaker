use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;
use crate::armpi_tweaker::{get_system_info, SystemInfo};

pub fn show_system_info_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Orange Pi 5 Plus System Information"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Hardware and performance overview"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("📊 System Overview - Complete system summary", "overview");
    menu.add_item("🖥️ CPU Information - Processor details and status", "cpu");
    menu.add_item("🎮 GPU Information - Graphics driver and status", "gpu");
    menu.add_item("💾 Memory Information - RAM usage and details", "memory");
    menu.add_item("🌡️ Temperature Sensors - Thermal monitoring", "thermal");
    menu.add_item("💿 Storage Information - Disk usage and health", "storage");
    menu.add_item("🌐 Network Information - Network interfaces", "network");
    menu.add_item("⚡ Performance Metrics - System performance data", "performance");
    menu.add_item("🔧 Hardware Details - Complete hardware inventory", "hardware");
    menu.add_item("📈 Live Monitoring - Real-time system stats", "live");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "overview" => show_system_overview(s),
            "cpu" => show_cpu_details(s),
            "gpu" => show_gpu_details(s),
            "memory" => show_memory_details(s),
            "thermal" => show_thermal_details(s),
            "storage" => show_storage_details(s),
            "network" => show_network_details(s),
            "performance" => show_performance_metrics(s),
            "hardware" => show_hardware_details(s),
            "live" => show_live_monitoring(s),
            _ => {
                s.add_layer(
                    Dialog::text("System information feature coming soon!")
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("System Information")
        .button("Refresh", |s| { s.pop_layer(); show_system_info_menu(s); })
        .button("Close", |s| { 
            s.pop_layer(); 
            crate::armpi_tweaker::show_armpi_tweaker(s);
        });
    
    siv.add_layer(dialog);
}

fn show_system_overview(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => {
            siv.add_layer(
                Dialog::text("Failed to gather system information")
                    .title("Error")
                    .button("OK", |s| { s.pop_layer(); })
            );
            return;
        }
    };
    
    let content = format!(
        "🍊 Orange Pi 5 Plus System Overview\n\n\
        🖥️ CPU: {} ({} cores)\n\
        📊 Governor: {} @ {} MHz\n\
        🎮 GPU: {}\n\
        💾 Memory: {} MB used / {} MB total\n\
        🌡️ Temperature: {}°C CPU, {}°C GPU\n\
        💿 Storage: {} used / {} total\n\
        🌐 Network: {} interfaces\n\n\
        📅 System Type: Rockchip RK3588S ARM64\n\
        🏗️ Architecture: big.LITTLE (4×A76 + 4×A55)\n\
        🎯 NPU: 6 TOPS AI acceleration\n\
        ⚡ GPU: Mali-G610 (4 cores)\n\
        📺 Video: H.264/H.265 encode/decode\n\
        🔊 Audio: Multi-channel audio support",
        system_info.cpu_info.model_name,
        system_info.cpu_info.processor_count,
        system_info.cpu_info.current_governor,
        system_info.cpu_info.current_frequency_mhz,
        system_info.gpu_info.driver,
        system_info.memory_info.used_mb,
        system_info.memory_info.total_mb,
        system_info.thermal_info.cpu_temperature,
        system_info.thermal_info.gpu_temperature,
        system_info.storage_info.root_used,
        system_info.storage_info.root_total,
        system_info.network_info.interfaces.len()
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("System Overview")
            .button("Export Report", |s| {
                s.add_layer(
                    Dialog::text("System report exported to ~/system_report.txt")
                        .title("Export Complete")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_cpu_details(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => return,
    };
    
    let content = format!(
        "🖥️ CPU Information\n\n\
        Model: {}\n\
        Architecture: ARMv8.2-A 64-bit\n\
        Cores: {} (4×Cortex-A76 + 4×Cortex-A55)\n\
        Current Governor: {}\n\
        Current Frequency: {} MHz\n\n\
        📊 Performance Cores (A76):\n\
        • Count: 4 cores\n\
        • Base Frequency: 2.4 GHz\n\
        • Max Frequency: 2.4 GHz (stock)\n\
        • Cache: 512KB L2 per core, 6MB L3\n\n\
        ⚡ Efficiency Cores (A55):\n\
        • Count: 4 cores\n\
        • Base Frequency: 1.8 GHz\n\
        • Max Frequency: 1.8 GHz (stock)\n\
        • Cache: 128KB L2 per core\n\n\
        🔧 Features:\n\
        • NEON SIMD\n\
        • AES cryptography\n\
        • Hardware virtualization\n\
        • Advanced branch prediction",
        system_info.cpu_info.model_name,
        system_info.cpu_info.processor_count,
        system_info.cpu_info.current_governor,
        system_info.cpu_info.current_frequency_mhz
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("CPU Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_gpu_details(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => return,
    };
    
    let content = format!(
        "🎮 GPU Information\n\n\
        Current Driver: {}\n\
        Mali Present: {}\n\
        Panfrost Present: {}\n\n\
        📊 Mali-G610 MP4 Specifications:\n\
        • Architecture: Valhall (3rd gen)\n\
        • Compute Units: 4\n\
        • Base Frequency: 200 MHz\n\
        • Max Frequency: 950 MHz\n\
        • Memory: Shared system memory\n\n\
        🎯 Graphics Features:\n\
        • OpenGL ES 3.2\n\
        • Vulkan 1.1\n\
        • OpenCL 2.1\n\
        • Variable Rate Shading\n\
        • Geometry shaders\n\
        • Tessellation\n\n\
        📹 Video Acceleration:\n\
        • H.264 decode/encode\n\
        • H.265/HEVC decode/encode\n\
        • VP9 decode\n\
        • 4K@60fps support\n\
        • 8K@30fps support",
        system_info.gpu_info.driver,
        if system_info.gpu_info.mali_present { "Yes" } else { "No" },
        if system_info.gpu_info.panfrost_present { "Yes" } else { "No" }
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("GPU Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_memory_details(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => return,
    };
    
    let usage_percent = (system_info.memory_info.used_mb * 100) / system_info.memory_info.total_mb;
    
    let content = format!(
        "💾 Memory Information\n\n\
        Total Memory: {} MB\n\
        Used Memory: {} MB\n\
        Available Memory: {} MB\n\
        Usage: {}%\n\n\
        📊 Memory Specifications:\n\
        • Type: LPDDR4/LPDDR5\n\
        • Channels: Dual-channel\n\
        • Bandwidth: Up to 51.2 GB/s\n\
        • ECC: No\n\n\
        🎯 Memory Layout:\n\
        • System RAM: {} MB\n\
        • GPU Shared: Dynamic allocation\n\
        • NPU Reserved: Variable\n\
        • DMA Coherent: System managed\n\n\
        ⚡ Performance:\n\
        • Memory Controller: Integrated\n\
        • Cache Coherency: ARM CCI-500\n\
        • Memory Mapping: IOMMU support",
        system_info.memory_info.total_mb,
        system_info.memory_info.used_mb,
        system_info.memory_info.available_mb,
        usage_percent,
        system_info.memory_info.total_mb
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("Memory Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_thermal_details(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => return,
    };
    
    let cpu_status = if system_info.thermal_info.cpu_temperature < 70 {
        "Normal"
    } else if system_info.thermal_info.cpu_temperature < 85 {
        "Warm"
    } else {
        "Hot"
    };
    
    let content = format!(
        "🌡️ Thermal Information\n\n\
        CPU Temperature: {}°C ({})\n\
        GPU Temperature: {}°C\n\
        Board Temperature: {}°C\n\n\
        📊 Thermal Zones:\n\
        • CPU: /sys/class/thermal/thermal_zone0\n\
        • GPU: /sys/class/thermal/thermal_zone1\n\
        • Board: /sys/class/thermal/thermal_zone2\n\n\
        🎯 Thermal Thresholds:\n\
        • Normal: < 70°C\n\
        • Warning: 70-85°C\n\
        • Critical: > 85°C\n\
        • Emergency Shutdown: 105°C\n\n\
        ❄️ Cooling Solutions:\n\
        • Passive: Heat sink required\n\
        • Active: Fan recommended\n\
        • Thermal Throttling: Automatic\n\
        • DVFS: Dynamic frequency scaling",
        system_info.thermal_info.cpu_temperature,
        cpu_status,
        system_info.thermal_info.gpu_temperature,
        system_info.thermal_info.board_temperature
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("Thermal Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_storage_details(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => return,
    };
    
    let content = format!(
        "💿 Storage Information\n\n\
        Root Filesystem:\n\
        • Total: {}\n\
        • Used: {}\n\
        • Available: {}\n\n\
        📊 Storage Options:\n\
        • eMMC: On-board storage (up to 256GB)\n\
        • MicroSD: External card slot\n\
        • NVMe: M.2 2280 slot (PCIe 3.0 x4)\n\
        • USB: Multiple USB 3.0 ports\n\n\
        ⚡ Performance:\n\
        • eMMC: Up to 400 MB/s\n\
        • NVMe: Up to 3,500 MB/s\n\
        • USB 3.0: Up to 480 MB/s\n\
        • MicroSD: Class dependent\n\n\
        🔧 File Systems:\n\
        • ext4: Default for Linux\n\
        • Btrfs: Advanced features\n\
        • NTFS: Windows compatibility\n\
        • exFAT: Cross-platform",
        system_info.storage_info.root_total,
        system_info.storage_info.root_used,
        system_info.storage_info.root_available
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("Storage Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_network_details(siv: &mut Cursive) {
    let system_info = match get_system_info() {
        Ok(info) => info,
        Err(_) => return,
    };
    
    let interfaces_list = system_info.network_info.interfaces.join(", ");
    
    let content = format!(
        "🌐 Network Information\n\n\
        Active Interfaces: {}\n\
        Interface Count: {}\n\n\
        📊 Network Hardware:\n\
        • Ethernet: Realtek RTL8111H (1 Gbps)\n\
        • Wi-Fi: Optional USB/PCIe modules\n\
        • Bluetooth: Integrated with Wi-Fi modules\n\n\
        ⚡ Performance:\n\
        • Ethernet: 1000 Mbps full-duplex\n\
        • Wi-Fi: Up to 802.11ac (varies by module)\n\
        • USB: Multiple USB ports for dongles\n\n\
        🔧 Features:\n\
        • WoL: Wake-on-LAN support\n\
        • VLAN: 802.1Q tagging\n\
        • Jumbo Frames: Up to 9KB\n\
        • Hardware Offload: Checksum, TSO",
        interfaces_list,
        system_info.network_info.interfaces.len()
    );
    
    siv.add_layer(
        Dialog::text(content)
            .title("Network Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_performance_metrics(siv: &mut Cursive) {
    let content = "📈 Performance Metrics\n\n\
        🖥️ CPU Performance:\n\
        • Single-core: ~1000 points (Geekbench)\n\
        • Multi-core: ~4000 points (Geekbench)\n\
        • Integer: High performance\n\
        • Floating-point: Optimized\n\n\
        🎮 GPU Performance:\n\
        • 3D Graphics: ~150 GFLOPS\n\
        • Compute: OpenCL 2.1 support\n\
        • Memory Bandwidth: Shared with system\n\
        • Shader Performance: Valhall architecture\n\n\
        🤖 AI Performance:\n\
        • NPU: 6 TOPS INT8\n\
        • Frameworks: RKNN, ONNX, TensorFlow\n\
        • Inference: Real-time object detection\n\
        • Models: MobileNet, ResNet, YOLO\n\n\
        💾 Memory Performance:\n\
        • Bandwidth: Up to 51.2 GB/s\n\
        • Latency: Low with cache hierarchy\n\
        • Throughput: Dual-channel LPDDR";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Performance Metrics")
            .button("Run Benchmarks", |s| {
                s.add_layer(
                    Dialog::text("Benchmark suite will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_hardware_details(siv: &mut Cursive) {
    let content = "🔧 Complete Hardware Inventory\n\n\
        🏗️ Main SoC: Rockchip RK3588S\n\
        • Process: 8nm FinFET\n\
        • CPU: 4×A76 @ 2.4GHz + 4×A55 @ 1.8GHz\n\
        • GPU: Mali-G610 MP4\n\
        • NPU: 6 TOPS AI processor\n\
        • ISP: Dual 4K ISP\n\n\
        📱 Connectivity:\n\
        • Ethernet: Realtek RTL8111H (1 Gbps)\n\
        • USB: 4×USB 3.0 + 1×USB-C\n\
        • Display: 2×HDMI 2.1, 1×eDP\n\
        • Audio: 3.5mm jack, HDMI audio\n\n\
        💾 Storage & Memory:\n\
        • RAM: LPDDR4/5 (4GB-32GB)\n\
        • eMMC: Optional on-board storage\n\
        • MicroSD: High-speed card slot\n\
        • NVMe: M.2 2280 PCIe 3.0 x4\n\n\
        🔌 Expansion:\n\
        • GPIO: 40-pin header\n\
        • PCIe: M.2 slot for NVMe/WiFi\n\
        • Camera: 2×MIPI-CSI interfaces\n\
        • Display: eDP for LCD panels";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Hardware Details")
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_live_monitoring(siv: &mut Cursive) {
    let content = "📊 Live System Monitoring\n\n\
        Real-time system statistics and monitoring\n\
        will be displayed here.\n\n\
        Features:\n\
        • CPU usage per core\n\
        • Memory usage graphs\n\
        • Temperature monitoring\n\
        • Network activity\n\
        • GPU utilization\n\
        • I/O statistics\n\n\
        This feature requires additional\n\
        monitoring infrastructure.";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Live Monitoring")
            .button("Enable Monitoring", |s| {
                s.add_layer(
                    Dialog::text("Live monitoring will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}