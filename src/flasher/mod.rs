use cursive::Cursive;
use cursive::views::{Dialog, TextView, LinearLayout, DummyView, SelectView, EditView, Checkbox};
use cursive::traits::*;
use cursive::theme::{ColorStyle, BaseColor, Color};
use log::info;

pub fn show_spi_flasher(siv: &mut Cursive) {
    info!("Opening SPI Flasher");
    
    // Show main flasher menu
    show_flasher_main_menu(siv);
}

fn show_flasher_main_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("💾 SPI Flasher - RK3588 Orange Pi 5 Plus").style(ColorStyle::from(Color::Light(BaseColor::Cyan))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Hardware info
    layout.add_child(TextView::new("🔧 Setec Labs -- Rockchip USB Development Tool").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(TextView::new("Target Hardware: RK3588S Orange Pi 5 Plus"));
    layout.add_child(TextView::new("Storage Support: eMMC, SD Card, SPI NOR Flash"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Select flasher operation:"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("📋 Device Management - List, test, and manage devices", "device-mgmt")
        .item("🔄 Loader Operations - Download/upgrade bootloaders", "loader-ops")
        .item("💾 Flash Operations - Read/write flash memory", "flash-ops")
        .item("🗂️ Partition Operations - Manage partitions and GPT", "partition-ops")
        .item("🔍 Device Information - Read device/chip/flash info", "device-info")
        .item("⚙️ Advanced Operations - Reset, storage change, etc.", "advanced-ops");
    
    operation_select.set_on_select(|s, operation| {
        update_operation_description(s, operation);
    });
    
    // Set initial selection
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("operation_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("operation_description"));
    
    let dialog = Dialog::around(layout.fixed_width(85))
        .title("SPI Flasher - Setec Labs Development Tool")
        .button("Next", |s| {
            let selected_operation = s.call_on_name("operation_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(operation) = selected_operation {
                match operation {
                    "device-mgmt" => {
                        s.pop_layer();
                        show_device_management(s);
                    }
                    "loader-ops" => {
                        s.pop_layer();
                        show_loader_operations(s);
                    }
                    "flash-ops" => {
                        s.pop_layer();
                        show_flash_operations(s);
                    }
                    "partition-ops" => {
                        s.pop_layer();
                        show_partition_operations(s);
                    }
                    "device-info" => {
                        s.pop_layer();
                        show_device_info(s);
                    }
                    "advanced-ops" => {
                        s.pop_layer();
                        show_advanced_operations(s);
                    }
                    _ => {}
                }
            }
        })
        .button("Cancel", |s| {
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_operation_description(siv, &"device-mgmt");
}

fn update_operation_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "device-mgmt" => "List connected devices, test device connectivity, and perform basic device management operations. Equivalent to 'ld' and 'td' commands.",
        "loader-ops" => "Download bootloaders to device, upgrade existing loaders, and manage bootloader operations. Equivalent to 'db' and 'ul' commands.",
        "flash-ops" => "Read and write flash memory sectors, handle LBA operations, and manage flash content. Equivalent to 'rl', 'wl', and 'wlx' commands.",
        "partition-ops" => "Manage GPT partition tables, write parameters, print partition information, and erase flash. Equivalent to 'gpt', 'prm', 'ppt', and 'ef' commands.",
        "device-info" => "Read device information including chip info, flash info, flash ID, and device capabilities. Equivalent to 'rci', 'rfi', 'rid', and 'rcb' commands.",
        "advanced-ops" => "Reset device, change storage type, pack/unpack bootloaders, and tag SPL. Equivalent to 'rd', 'cs', 'pack', 'unpack', and 'tagspl' commands.",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("operation_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_device_management(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("📋 Device Management").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Available Operations:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("📱 List Devices (ld) - Show all connected RK3588 devices", "list-devices")
        .item("🔧 Test Device (td) - Test device connectivity and status", "test-device")
        .item("🔄 Refresh Device List - Rescan for devices", "refresh-devices");
    
    operation_select.set_on_select(|s, op| {
        update_device_mgmt_description(s, op);
    });
    
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("device_mgmt_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("device_mgmt_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Device Management")
        .button("Execute", |s| {
            let selected_op = s.call_on_name("device_mgmt_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(op) = selected_op {
                match op {
                    "list-devices" => execute_command(s, "List Devices", "setecdevtool ld"),
                    "test-device" => execute_command(s, "Test Device", "setecdevtool td"),
                    "refresh-devices" => execute_command(s, "Refresh Devices", "Rescanning for RK3588 devices..."),
                    _ => {}
                }
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_flasher_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_device_mgmt_description(siv, &"list-devices");
}

fn update_device_mgmt_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "list-devices" => "List all connected Rockchip devices. Shows device ID, connection status, and basic device information.",
        "test-device" => "Test connectivity with the selected device. Verifies USB communication and device readiness.",
        "refresh-devices" => "Rescan USB ports for newly connected or disconnected RK3588 devices.",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("device_mgmt_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_loader_operations(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🔄 Loader Operations").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Bootloader Management:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("📥 Download Boot (db) - Download bootloader to device", "download-boot")
        .item("⬆️ Upgrade Loader (ul) - Upgrade existing bootloader", "upgrade-loader")
        .item("📦 Pack Bootloader (pack) - Pack bootloader files", "pack-bootloader")
        .item("📂 Unpack Bootloader (unpack) - Unpack bootloader files", "unpack-bootloader")
        .item("🏷️ Tag SPL (tagspl) - Tag Secondary Program Loader", "tag-spl");
    
    operation_select.set_on_select(|s, op| {
        update_loader_ops_description(s, op);
    });
    
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("loader_ops_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("loader_ops_description"));
    layout.add_child(DummyView.fixed_height(1));
    
    // File selection for operations that need files
    layout.add_child(TextView::new("Loader File:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    let loader_file = EditView::new()
        .content("RK3588Loader.bin")
        .with_name("loader_file")
        .fixed_width(30);
    layout.add_child(loader_file);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Loader Operations")
        .button("Execute", |s| {
            let selected_op = s.call_on_name("loader_ops_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            let loader_file = s.call_on_name("loader_file", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            if let Some(op) = selected_op {
                match op {
                    "download-boot" => execute_command(s, "Download Boot", &format!("setecdevtool db {}", loader_file)),
                    "upgrade-loader" => execute_command(s, "Upgrade Loader", &format!("setecdevtool ul {}", loader_file)),
                    "pack-bootloader" => execute_command(s, "Pack Bootloader", "setecdevtool pack"),
                    "unpack-bootloader" => execute_command(s, "Unpack Bootloader", &format!("setecdevtool unpack {}", loader_file)),
                    "tag-spl" => show_tag_spl_dialog(s),
                    _ => {}
                }
            }
        })
        .button("Browse File", |s| {
            show_file_browser(s, "loader_file");
        })
        .button("Back", |s| {
            s.pop_layer();
            show_flasher_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_loader_ops_description(siv, &"download-boot");
}

fn update_loader_ops_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "download-boot" => "Download bootloader to device. Device must be in MaskROM mode. Required for initial bootloader installation.",
        "upgrade-loader" => "Upgrade existing bootloader on device. Device must be in loader mode. Used for bootloader updates.",
        "pack-bootloader" => "Pack bootloader files into a single image. Creates a combined bootloader package.",
        "unpack-bootloader" => "Unpack bootloader files from a combined image. Extracts individual components.",
        "tag-spl" => "Tag Secondary Program Loader with specific tag. Used for SPL identification and verification.",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("loader_ops_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_flash_operations(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("💾 Flash Operations").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Flash Memory Operations:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("📖 Read LBA (rl) - Read Logical Block Address", "read-lba")
        .item("✍️ Write LBA (wl) - Write Logical Block Address", "write-lba")
        .item("📝 Write LBA by Partition (wlx) - Write to specific partition", "write-lba-partition")
        .item("🧹 Erase Flash (ef) - Erase entire flash memory", "erase-flash");
    
    operation_select.set_on_select(|s, op| {
        update_flash_ops_description(s, op);
    });
    
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("flash_ops_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("flash_ops_description"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Parameters for flash operations
    layout.add_child(TextView::new("Parameters:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    
    layout.add_child(TextView::new("Begin Sector (hex):"));
    let begin_sector = EditView::new()
        .content("0x8000")
        .with_name("begin_sector")
        .fixed_width(15);
    layout.add_child(begin_sector);
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Sector Length (for read):"));
    let sector_length = EditView::new()
        .content("0x1000")
        .with_name("sector_length")
        .fixed_width(15);
    layout.add_child(sector_length);
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("File Path:"));
    let file_path = EditView::new()
        .content("image.img")
        .with_name("file_path")
        .fixed_width(30);
    layout.add_child(file_path);
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Partition Name (for wlx):"));
    let partition_name = EditView::new()
        .content("boot")
        .with_name("partition_name")
        .fixed_width(20);
    layout.add_child(partition_name);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Flash Operations")
        .button("Execute", |s| {
            let selected_op = s.call_on_name("flash_ops_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            let begin_sector = s.call_on_name("begin_sector", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            let sector_length = s.call_on_name("sector_length", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            let file_path = s.call_on_name("file_path", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            let partition_name = s.call_on_name("partition_name", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            if let Some(op) = selected_op {
                match op {
                    "read-lba" => execute_command(s, "Read LBA", &format!("setecdevtool rl {} {} {}", begin_sector, sector_length, file_path)),
                    "write-lba" => execute_command(s, "Write LBA", &format!("setecdevtool wl {} {}", begin_sector, file_path)),
                    "write-lba-partition" => execute_command(s, "Write LBA to Partition", &format!("setecdevtool wlx {} {}", partition_name, file_path)),
                    "erase-flash" => show_erase_confirmation(s),
                    _ => {}
                }
            }
        })
        .button("Browse File", |s| {
            show_file_browser(s, "file_path");
        })
        .button("Back", |s| {
            s.pop_layer();
            show_flasher_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_flash_ops_description(siv, &"read-lba");
}

fn update_flash_ops_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "read-lba" => "Read data from flash memory at specified LBA (Logical Block Address). Specify start sector and length.",
        "write-lba" => "Write data to flash memory at specified LBA. Specify start sector and image file.",
        "write-lba-partition" => "Write data to a specific partition by name. Automatically finds partition offset.",
        "erase-flash" => "⚠️ DANGEROUS: Erase entire flash memory. This will delete all data including bootloader.",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("flash_ops_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_partition_operations(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🗂️ Partition Operations").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Partition Management:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("📋 Print Partition Table (ppt) - Show current partition layout", "print-partition")
        .item("🗂️ Write GPT (gpt) - Write GUID Partition Table", "write-gpt")
        .item("⚙️ Write Parameter (prm) - Write device parameters", "write-parameter");
    
    operation_select.set_on_select(|s, op| {
        update_partition_ops_description(s, op);
    });
    
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("partition_ops_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("partition_ops_description"));
    layout.add_child(DummyView.fixed_height(1));
    
    // File input for operations that need files
    layout.add_child(TextView::new("File Path:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    let partition_file = EditView::new()
        .content("parameter.txt")
        .with_name("partition_file")
        .fixed_width(30);
    layout.add_child(partition_file);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Partition Operations")
        .button("Execute", |s| {
            let selected_op = s.call_on_name("partition_ops_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            let partition_file = s.call_on_name("partition_file", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            if let Some(op) = selected_op {
                match op {
                    "print-partition" => execute_command(s, "Print Partition Table", "setecdevtool ppt"),
                    "write-gpt" => execute_command(s, "Write GPT", &format!("setecdevtool gpt {}", partition_file)),
                    "write-parameter" => execute_command(s, "Write Parameter", &format!("setecdevtool prm {}", partition_file)),
                    _ => {}
                }
            }
        })
        .button("Browse File", |s| {
            show_file_browser(s, "partition_file");
        })
        .button("Back", |s| {
            s.pop_layer();
            show_flasher_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_partition_ops_description(siv, &"print-partition");
}

fn update_partition_ops_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "print-partition" => "Display current partition table layout. Shows partition names, sizes, and offsets.",
        "write-gpt" => "Write GUID Partition Table to device. Requires GPT file with partition layout.",
        "write-parameter" => "Write device parameters. Used for device configuration and partition information.",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("partition_ops_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_device_info(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🔍 Device Information").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Device Information Commands:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("🔧 Read Chip Info (rci) - Read RK3588 chip information", "read-chip-info")
        .item("💾 Read Flash Info (rfi) - Read flash memory information", "read-flash-info")
        .item("🆔 Read Flash ID (rid) - Read flash memory ID", "read-flash-id")
        .item("📊 Read Capability (rcb) - Read device capabilities", "read-capability");
    
    operation_select.set_on_select(|s, op| {
        update_device_info_description(s, op);
    });
    
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("device_info_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("device_info_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Device Information")
        .button("Execute", |s| {
            let selected_op = s.call_on_name("device_info_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(op) = selected_op {
                match op {
                    "read-chip-info" => execute_command(s, "Read Chip Info", "setecdevtool rci"),
                    "read-flash-info" => execute_command(s, "Read Flash Info", "setecdevtool rfi"),
                    "read-flash-id" => execute_command(s, "Read Flash ID", "setecdevtool rid"),
                    "read-capability" => execute_command(s, "Read Capability", "setecdevtool rcb"),
                    _ => {}
                }
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_flasher_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_device_info_description(siv, &"read-chip-info");
}

fn update_device_info_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "read-chip-info" => "Read RK3588 chip information including chip ID, version, and capabilities.",
        "read-flash-info" => "Read flash memory information including size, block size, and manufacturer details.",
        "read-flash-id" => "Read flash memory ID for identification and compatibility verification.",
        "read-capability" => "Read device capabilities including supported operations and features.",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("device_info_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_advanced_operations(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("⚙️ Advanced Operations").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Advanced Device Operations:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut operation_select = SelectView::<&str>::new()
        .item("🔄 Reset Device (rd) - Reset device with optional subcode", "reset-device")
        .item("💾 Change Storage (cs) - Switch between eMMC/SD/SPI NOR", "change-storage");
    
    operation_select.set_on_select(|s, op| {
        update_advanced_ops_description(s, op);
    });
    
    operation_select.set_selection(0);
    
    layout.add_child(operation_select.with_name("advanced_ops_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("advanced_ops_description"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Reset subcode options
    layout.add_child(TextView::new("Reset Subcode:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    let reset_subcode = SelectView::<&str>::new()
        .item("None", "none")
        .item("Reset MSC", "resetmsc")
        .item("Power Off", "poweroff")
        .item("Reset to MaskROM", "resetmaskrom")
        .with_name("reset_subcode");
    layout.add_child(reset_subcode);
    layout.add_child(DummyView.fixed_height(1));
    
    // Storage type options
    layout.add_child(TextView::new("Storage Type:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    let storage_type = SelectView::<&str>::new()
        .item("eMMC", "1")
        .item("SD Card", "2")
        .item("SPI NOR Flash", "9")
        .with_name("storage_type");
    layout.add_child(storage_type);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Advanced Operations")
        .button("Execute", |s| {
            let selected_op = s.call_on_name("advanced_ops_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(op) = selected_op {
                match op {
                    "reset-device" => {
                        let subcode = s.call_on_name("reset_subcode", |view: &mut SelectView<&str>| {
                            view.selection().map(|sel| *sel)
                        }).flatten().unwrap_or("none");
                        
                        let cmd = if subcode == "none" {
                            "setecdevtool rd".to_string()
                        } else {
                            format!("setecdevtool rd {}", subcode)
                        };
                        
                        execute_command(s, "Reset Device", &cmd);
                    }
                    "change-storage" => {
                        let storage = s.call_on_name("storage_type", |view: &mut SelectView<&str>| {
                            view.selection().map(|sel| *sel)
                        }).flatten().unwrap_or("1");
                        
                        execute_command(s, "Change Storage", &format!("setecdevtool cs {}", storage));
                    }
                    _ => {}
                }
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_flasher_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_advanced_ops_description(siv, &"reset-device");
}

fn update_advanced_ops_description(siv: &mut Cursive, operation: &&str) {
    let description = match *operation {
        "reset-device" => "Reset the device with optional subcode. Use subcodes for specific reset types like MaskROM mode.",
        "change-storage" => "Change active storage type. Switch between eMMC (1), SD Card (2), or SPI NOR Flash (9).",
        _ => "Unknown operation.",
    };
    
    siv.call_on_name("advanced_ops_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

// Helper functions
fn execute_command(siv: &mut Cursive, title: &str, command: &str) {
    let message = format!("Executing: {}\n\n⚠️ This is a placeholder for actual command execution.\n\nIn the full implementation, this would:\n• Execute the setecdevtool command\n• Show real-time progress\n• Display command output\n• Handle errors and status codes\n• Provide user feedback", command);
    
    siv.add_layer(
        Dialog::text(message)
            .title(title)
            .button("OK", |s| { s.pop_layer(); })
    );
}

fn show_file_browser(siv: &mut Cursive, field_name: &str) {
    let message = format!("File browser for {}...\n\nThis is a placeholder for file selection.\n\nIn the full implementation, this would:\n• Open native file dialog\n• Filter by relevant file types\n• Validate file selection\n• Update the {} field", field_name, field_name);
    
    siv.add_layer(
        Dialog::text(message)
            .title("File Browser")
            .button("OK", |s| { s.pop_layer(); })
    );
}

fn show_tag_spl_dialog(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🏷️ Tag SPL Configuration").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Tag:"));
    let tag_input = EditView::new()
        .content("RK35")
        .with_name("spl_tag")
        .fixed_width(20);
    layout.add_child(tag_input);
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("U-Boot SPL File:"));
    let spl_file = EditView::new()
        .content("u-boot-spl.bin")
        .with_name("spl_file")
        .fixed_width(30);
    layout.add_child(spl_file);
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Tag SPL")
        .button("Execute", |s| {
            let tag = s.call_on_name("spl_tag", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            let spl_file = s.call_on_name("spl_file", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_default();
            
            execute_command(s, "Tag SPL", &format!("setecdevtool tagspl {} {}", tag, spl_file));
            s.pop_layer();
        })
        .button("Cancel", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
}

fn show_erase_confirmation(siv: &mut Cursive) {
    siv.add_layer(
        Dialog::text("⚠️ WARNING: ERASE FLASH MEMORY\n\nThis operation will completely erase all data on the flash memory including:\n• Bootloader\n• Operating System\n• User data\n• All partitions\n\nThis action is IRREVERSIBLE and will require a complete reflashing of the device.\n\nAre you absolutely sure you want to proceed?")
            .title("⚠️ DANGEROUS OPERATION")
            .button("Yes, Erase Everything", |s| {
                execute_command(s, "Erase Flash", "setecdevtool ef");
                s.pop_layer();
            })
            .button("Cancel", |s| {
                s.pop_layer();
            })
    );
}