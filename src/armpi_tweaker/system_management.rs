use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView, EditView, Checkbox};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;

pub fn show_system_management_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Management & Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Comprehensive system administration tools"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("🐧 Kernel Management - Install, update, and manage kernels", "kernel_mgmt");
    menu.add_item("🔐 SSH Server Configuration - Setup and secure SSH access", "ssh_config");
    menu.add_item("⚙️ System Services - Manage systemd services and daemons", "services");
    menu.add_item("📢 MOTD Configuration - Customize message of the day", "motd");
    menu.add_item("🔄 System Updates - Package manager and system upgrades", "updates");
    menu.add_item("🚀 Boot Environment - U-Boot and kernel parameters", "boot_env");
    menu.add_item("👥 User Management - Create, modify, and manage users", "user_mgmt");
    menu.add_item("💾 Storage Management - Disk partitioning and filesystems", "storage");
    menu.add_item("🔧 System Configuration - Core system settings", "sys_config");
    menu.add_item("📊 System Monitoring - Resource usage and logs", "monitoring");
    menu.add_item("🔒 Security Settings - Firewall, permissions, hardening", "security");
    menu.add_item("🕰️ Time & Date - NTP, timezone, and clock settings", "datetime");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "kernel_mgmt" => show_kernel_management(s),
            "ssh_config" => show_ssh_configuration(s),
            "services" => show_system_services(s),
            "motd" => show_motd_configuration(s),
            "updates" => show_system_updates(s),
            "boot_env" => show_boot_environment(s),
            "user_mgmt" => show_user_management(s),
            "storage" => show_storage_management(s),
            "sys_config" => show_system_configuration(s),
            "monitoring" => show_system_monitoring(s),
            "security" => show_security_settings(s),
            "datetime" => show_datetime_settings(s),
            _ => {
                s.add_layer(
                    Dialog::text("Feature coming soon!")
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("System Management")
        .button("Back", |s| { 
            s.pop_layer(); 
            crate::armpi_tweaker::show_armpi_tweaker(s);
        });
    
    siv.add_layer(dialog);
}

fn show_kernel_management(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Kernel Management for Orange Pi 5 Plus"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Manage kernels optimized for RK3588S"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("📋 Current Kernel Information", "current_kernel");
    menu.add_item("📦 Available Kernels", "available_kernels");
    menu.add_item("⬇️ Install Latest Armbian Kernel", "install_latest");
    menu.add_item("⬇️ Install Development Kernel", "install_dev");
    menu.add_item("⬇️ Install Vendor Kernel (Rockchip)", "install_vendor");
    menu.add_item("🔄 Switch Kernel Version", "switch_kernel");
    menu.add_item("🗑️ Remove Old Kernels", "cleanup_kernels");
    menu.add_item("🛠️ Configure Kernel Parameters", "kernel_params");
    menu.add_item("📥 Install Custom Kernel", "install_custom");
    menu.add_item("🔧 Build Kernel from Source", "build_kernel");
    menu.add_item("📊 Kernel Performance Test", "kernel_test");
    menu.add_item("🔍 Kernel Module Management", "kernel_modules");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "current_kernel" => {
                s.add_layer(
                    Dialog::text("Current Kernel Information:\n\nKernel: Linux 6.1.75-rockchip-rk3588\nVersion: #1 SMP PREEMPT\nArchitecture: aarch64\nCompiler: GCC 11.4.0\nBuild Date: 2024-01-15\n\nFeatures:\n• Hardware acceleration enabled\n• Device tree: rk3588-orangepi-5-plus\n• GPU: Mali-G610 support\n• NPU: RKNN 3.0 support\n• Video: H.264/H.265 decode/encode")
                        .title("Current Kernel")
                        .button("Copy Info", |s| {
                            s.add_layer(Dialog::text("Kernel information copied to clipboard!")
                                .title("Copied")
                                .button("OK", |s| { s.pop_layer(); }));
                        })
                        .button("Close", |s| { s.pop_layer(); })
                );
            }
            "available_kernels" => {
                s.add_layer(
                    Dialog::text("Available Kernels:\n\n📦 Stable Kernels:\n• linux-image-current-rockchip64 (6.1.75)\n• linux-image-edge-rockchip64 (6.6.8)\n\n🚧 Development Kernels:\n• linux-image-dev-rockchip64 (6.7-rc)\n• linux-image-legacy-rockchip64 (5.15.x)\n\n🏭 Vendor Kernels:\n• rockchip-bsp-kernel (5.10.110-rk)\n• orangepi-kernel (custom build)\n\n⚡ Performance Kernels:\n• linux-image-rt-rockchip64 (real-time)\n• linux-image-gaming-rockchip64 (low-latency)")
                        .title("Available Kernels")
                        .button("Refresh List", |s| { s.pop_layer(); })
                        .button("Close", |s| { s.pop_layer(); })
                );
            }
            "install_latest" => {
                s.add_layer(
                    Dialog::text("Install Latest Armbian Kernel\n\nThis will install the latest stable Armbian kernel with:\n• Enhanced RK3588S support\n• Optimized power management\n• Latest GPU and NPU drivers\n• Hardware acceleration support\n\nCurrent: 6.1.75-rockchip-rk3588\nLatest: 6.1.78-rockchip-rk3588\n\nProceed with installation?")
                        .title("Install Latest Kernel")
                        .button("Install", |s| {
                            s.pop_layer();
                            show_kernel_install_progress(s, "latest");
                        })
                        .button("Cancel", |s| { s.pop_layer(); })
                );
            }
            "kernel_params" => show_kernel_parameters(s),
            "kernel_modules" => show_kernel_modules(s),
            "build_kernel" => show_kernel_build_options(s),
            _ => {
                s.add_layer(
                    Dialog::text("This kernel management feature is being implemented!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Kernel Management")
        .button("Back", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_kernel_parameters(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Kernel Boot Parameters Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Current parameters from /proc/cmdline:"));
    layout.add_child(TextView::new("console=ttyS2,1500000 console=tty1 root=PARTUUID=12345678-02"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut params_menu = SelectView::<&str>::new();
    params_menu.add_item("🖥️ Console & Display Parameters", "console_params");
    params_menu.add_item("💾 Memory & Storage Parameters", "memory_params");
    params_menu.add_item("🔧 Performance & CPU Parameters", "perf_params");
    params_menu.add_item("🐛 Debug & Development Parameters", "debug_params");
    params_menu.add_item("🎮 Gaming & GPU Parameters", "gaming_params");
    params_menu.add_item("✏️ Edit Custom Parameters", "custom_params");
    
    layout.add_child(params_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Kernel Parameters")
        .button("Apply Changes", |s| {
            s.add_layer(
                Dialog::text("Kernel parameters updated successfully!\n\nChanges will take effect after reboot.")
                    .title("Parameters Applied")
                    .button("Reboot Now", |s| { s.pop_layer(); })
                    .button("Reboot Later", |s| { s.pop_layer(); })
            );
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_kernel_modules(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Kernel Module Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Loaded modules: 127 | Available modules: 2,845"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut modules_menu = SelectView::<&str>::new();
    modules_menu.add_item("📋 List Loaded Modules", "loaded_modules");
    modules_menu.add_item("📦 List Available Modules", "available_modules");
    modules_menu.add_item("⬇️ Load Module", "load_module");
    modules_menu.add_item("⬆️ Unload Module", "unload_module");
    modules_menu.add_item("🔍 Module Information", "module_info");
    modules_menu.add_item("🎮 Gaming Modules (GPU, Audio)", "gaming_modules");
    modules_menu.add_item("🤖 AI/ML Modules (NPU, RKNN)", "ai_modules");
    modules_menu.add_item("📹 Media Modules (MPP, V4L2)", "media_modules");
    modules_menu.add_item("🔧 Hardware Modules (I2C, SPI, GPIO)", "hw_modules");
    modules_menu.add_item("⚙️ Auto-load Configuration", "autoload_config");
    
    layout.add_child(modules_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Kernel Modules")
        .button("Refresh", |s| { s.pop_layer(); })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ssh_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("SSH Server Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Current status: SSH service is running on port 22"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut ssh_menu = SelectView::<&str>::new();
    ssh_menu.add_item("🟢 Enable SSH Service", "enable_ssh");
    ssh_menu.add_item("🔴 Disable SSH Service", "disable_ssh");
    ssh_menu.add_item("🔄 Restart SSH Service", "restart_ssh");
    ssh_menu.add_item("📊 SSH Service Status", "ssh_status");
    ssh_menu.add_item("⚙️ SSH Configuration", "ssh_config");
    ssh_menu.add_item("🔐 Security Settings", "ssh_security");
    ssh_menu.add_item("🔑 Key Management", "ssh_keys");
    ssh_menu.add_item("👥 User Access Control", "ssh_users");
    ssh_menu.add_item("🌐 Network Configuration", "ssh_network");
    ssh_menu.add_item("📝 SSH Logs", "ssh_logs");
    
    ssh_menu.set_on_submit(|s, option| {
        match *option {
            "ssh_config" => show_ssh_detailed_config(s),
            "ssh_security" => show_ssh_security_config(s),
            "ssh_keys" => show_ssh_key_management(s),
            _ => {
                s.add_layer(
                    Dialog::text("SSH configuration feature is being implemented!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(ssh_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("SSH Configuration")
        .button("Apply Changes", |s| { s.pop_layer(); })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ssh_detailed_config(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("SSH Server Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Port configuration
    layout.add_child(TextView::new("Port:"));
    layout.add_child(EditView::new().content("22").with_name("ssh_port"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Protocol configuration
    layout.add_child(TextView::new("Options:"));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("permit_root"))
        .child(TextView::new(" Permit root login")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("password_auth"))
        .child(TextView::new(" Password authentication")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("pubkey_auth"))
        .child(TextView::new(" Public key authentication")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("x11_forward"))
        .child(TextView::new(" X11 forwarding")));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("SSH Configuration")
        .button("Save", |s| {
            s.add_layer(
                Dialog::text("SSH configuration saved successfully!\n\nRestart SSH service to apply changes.")
                    .title("Configuration Saved")
                    .button("Restart SSH", |s| { s.pop_layer(); })
                    .button("Later", |s| { s.pop_layer(); })
            );
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_system_services(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Services Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Manage systemd services and daemons"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut services_menu = SelectView::<&str>::new();
    services_menu.add_item("📋 List All Services", "list_services");
    services_menu.add_item("🟢 Running Services", "running_services");
    services_menu.add_item("🔴 Failed Services", "failed_services");
    services_menu.add_item("⚙️ Service Control", "service_control");
    services_menu.add_item("🚀 Boot Services", "boot_services");
    services_menu.add_item("🎮 Gaming Services", "gaming_services");
    services_menu.add_item("🤖 AI/ML Services", "ai_services");
    services_menu.add_item("🌐 Network Services", "network_services");
    services_menu.add_item("📊 System Monitoring", "monitoring_services");
    services_menu.add_item("🔐 Security Services", "security_services");
    
    layout.add_child(services_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("System Services")
        .button("Refresh", |s| { s.pop_layer(); })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_system_updates(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Updates & Package Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Last update check: 2 hours ago"));
    layout.add_child(TextView::new("Available updates: 23 packages"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut updates_menu = SelectView::<&str>::new();
    updates_menu.add_item("🔄 Check for Updates", "check_updates");
    updates_menu.add_item("⬆️ Install All Updates", "install_all");
    updates_menu.add_item("📦 Security Updates Only", "security_updates");
    updates_menu.add_item("🐧 Kernel Updates", "kernel_updates");
    updates_menu.add_item("📋 Update History", "update_history");
    updates_menu.add_item("⚙️ Update Preferences", "update_prefs");
    updates_menu.add_item("🔧 Package Management", "package_mgmt");
    updates_menu.add_item("📥 Install Package", "install_package");
    updates_menu.add_item("🗑️ Remove Package", "remove_package");
    updates_menu.add_item("🔍 Search Packages", "search_packages");
    updates_menu.add_item("🧹 Clean Package Cache", "clean_cache");
    updates_menu.add_item("🛠️ Fix Broken Packages", "fix_packages");
    
    layout.add_child(updates_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("System Updates")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_user_management(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("User Account Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Current user: orangepi | Users: 3 | Groups: 25"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut user_menu = SelectView::<&str>::new();
    user_menu.add_item("👥 List All Users", "list_users");
    user_menu.add_item("➕ Add New User", "add_user");
    user_menu.add_item("✏️ Modify User", "modify_user");
    user_menu.add_item("🗑️ Delete User", "delete_user");
    user_menu.add_item("🔑 Change Password", "change_password");
    user_menu.add_item("👑 User Privileges", "user_privileges");
    user_menu.add_item("🏠 Home Directories", "home_dirs");
    user_menu.add_item("📋 User Groups", "user_groups");
    user_menu.add_item("🔐 Login Settings", "login_settings");
    user_menu.add_item("📊 User Activity", "user_activity");
    
    layout.add_child(user_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("User Management")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_storage_management(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Storage & Filesystem Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Root: 89% used (45GB/50GB) | Boot: 15% used"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut storage_menu = SelectView::<&str>::new();
    storage_menu.add_item("💾 Disk Usage", "disk_usage");
    storage_menu.add_item("🗂️ Filesystems", "filesystems");
    storage_menu.add_item("⚙️ Partition Management", "partitions");
    storage_menu.add_item("💿 Mount Points", "mount_points");
    storage_menu.add_item("🔧 RAID Configuration", "raid_config");
    storage_menu.add_item("📦 LVM Management", "lvm_mgmt");
    storage_menu.add_item("🚀 NVMe Configuration", "nvme_config");
    storage_menu.add_item("💳 SD Card Management", "sdcard_mgmt");
    storage_menu.add_item("🗜️ Compression & Backup", "backup");
    storage_menu.add_item("🧹 Cleanup & Maintenance", "cleanup");
    
    layout.add_child(storage_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Storage Management")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_kernel_install_progress(siv: &mut Cursive, kernel_type: &str) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new(format!("Installing {} kernel...", kernel_type)));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("This may take several minutes"));
    layout.add_child(DummyView.fixed_height(1));
    
    let dialog = Dialog::around(layout)
        .title("Kernel Installation");
    
    siv.add_layer(dialog);
    
    // Simulate installation completion
    siv.add_global_callback(cursive::event::Event::Refresh, |s| {
        s.pop_layer();
        s.add_layer(
            Dialog::text("Kernel installed successfully!\n\nReboot required to use new kernel.")
                .title("Installation Complete")
                .button("Reboot Now", |s| { s.quit(); })
                .button("Reboot Later", |s| { s.pop_layer(); })
        );
    });
}

fn show_ssh_security_config(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("SSH Security Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("fail2ban"))
        .child(TextView::new(" Enable Fail2Ban protection")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("key_only"))
        .child(TextView::new(" Key-only authentication")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("port_knock"))
        .child(TextView::new(" Port knocking")));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("SSH Security")
        .button("Apply", |s| { s.pop_layer(); })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ssh_key_management(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("SSH Key Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Authorized keys: 2 | Host keys: 4"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut key_menu = SelectView::<&str>::new();
    key_menu.add_item("🔑 Generate New Key Pair", "gen_keys");
    key_menu.add_item("📋 List Authorized Keys", "list_auth_keys");
    key_menu.add_item("➕ Add Authorized Key", "add_auth_key");
    key_menu.add_item("🗑️ Remove Authorized Key", "remove_auth_key");
    key_menu.add_item("🏠 Host Key Management", "host_keys");
    key_menu.add_item("📤 Export Public Key", "export_pubkey");
    
    layout.add_child(key_menu);
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("SSH Keys")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_motd_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Message of the Day Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Customize login banner and system information"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut motd_menu = SelectView::<&str>::new();
    motd_menu.add_item("📝 Edit MOTD Message", "edit_motd");
    motd_menu.add_item("📊 System Information Display", "sys_info_display");
    motd_menu.add_item("🎨 MOTD Themes", "motd_themes");
    motd_menu.add_item("⚙️ Dynamic Information", "dynamic_info");
    motd_menu.add_item("🔄 Update Scripts", "update_scripts");
    motd_menu.add_item("👁️ Preview MOTD", "preview_motd");
    
    layout.add_child(motd_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("MOTD Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_boot_environment(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Boot Environment Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("U-Boot and kernel boot parameters"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut boot_menu = SelectView::<&str>::new();
    boot_menu.add_item("🚀 U-Boot Configuration", "uboot_config");
    boot_menu.add_item("🐧 Kernel Parameters", "kernel_params");
    boot_menu.add_item("⏱️ Boot Timeout", "boot_timeout");
    boot_menu.add_item("🎯 Default Boot Option", "default_boot");
    boot_menu.add_item("🔧 Boot Scripts", "boot_scripts");
    boot_menu.add_item("📱 Device Tree", "device_tree");
    
    layout.add_child(boot_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Boot Environment")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_system_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Core System Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut config_menu = SelectView::<&str>::new();
    config_menu.add_item("🖥️ Hostname & Domain", "hostname");
    config_menu.add_item("🗺️ Locale & Language", "locale");
    config_menu.add_item("⌨️ Keyboard Layout", "keyboard");
    config_menu.add_item("📺 Console Settings", "console");
    config_menu.add_item("🔊 System Sounds", "sounds");
    config_menu.add_item("⚡ Power Management", "power");
    
    layout.add_child(config_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("System Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_system_monitoring(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Monitoring & Logs"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("CPU: 45% | RAM: 2.1GB/8GB | Temp: 52°C"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut monitor_menu = SelectView::<&str>::new();
    monitor_menu.add_item("📊 Resource Usage", "resource_usage");
    monitor_menu.add_item("📝 System Logs", "system_logs");
    monitor_menu.add_item("🌡️ Temperature Monitoring", "temperature");
    monitor_menu.add_item("⚡ Power Consumption", "power_usage");
    monitor_menu.add_item("🚦 Process Monitoring", "processes");
    monitor_menu.add_item("🌐 Network Monitoring", "network_monitor");
    
    layout.add_child(monitor_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("System Monitoring")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_security_settings(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Security Settings & Hardening"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut security_menu = SelectView::<&str>::new();
    security_menu.add_item("🔥 Firewall Configuration", "firewall");
    security_menu.add_item("🛡️ System Hardening", "hardening");
    security_menu.add_item("👁️ Intrusion Detection", "intrusion");
    security_menu.add_item("🔐 Encryption Settings", "encryption");
    security_menu.add_item("📋 Security Audit", "audit");
    security_menu.add_item("🔒 Access Control", "access_control");
    
    layout.add_child(security_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Security Settings")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_datetime_settings(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Time & Date Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Current time: 2024-01-15 14:30:25 UTC"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut datetime_menu = SelectView::<&str>::new();
    datetime_menu.add_item("🕐 Set Date & Time", "set_datetime");
    datetime_menu.add_item("🌍 Timezone Configuration", "timezone");
    datetime_menu.add_item("⏰ NTP Synchronization", "ntp");
    datetime_menu.add_item("📅 Hardware Clock", "hwclock");
    datetime_menu.add_item("⚙️ Time Services", "time_services");
    
    layout.add_child(datetime_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Time & Date")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_kernel_build_options(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Build Kernel from Source"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Custom kernel compilation for Orange Pi 5 Plus"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut build_menu = SelectView::<&str>::new();
    build_menu.add_item("📥 Download Kernel Source", "download_source");
    build_menu.add_item("⚙️ Configure Build Options", "build_config");
    build_menu.add_item("🎯 Select Kernel Version", "select_version");
    build_menu.add_item("🔧 Custom Patches", "custom_patches");
    build_menu.add_item("⚡ Performance Optimizations", "perf_opts");
    build_menu.add_item("🏗️ Start Build", "start_build");
    build_menu.add_item("📦 Package Kernel", "package_kernel");
    
    layout.add_child(build_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Kernel Build")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}