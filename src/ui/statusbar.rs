use cursive::views::{LinearLayout, TextView, DummyView, Layer, MenuPopup, Dialog, SelectView};
use cursive::{Cursive, View, Vec2, Printer};
use cursive::theme::{ColorStyle, BaseColor, Color};
use cursive::traits::*;
use cursive::menu::Tree;
use cursive::event::{Event, EventResult, MouseButton, MouseEvent, Key};
use chrono::Local;
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;
use std::rc::Rc;

pub struct StatusBar {
    time: Arc<Mutex<String>>,
    network_status: Arc<Mutex<String>>,
}

impl StatusBar {
    pub fn new() -> Self {
        let time = Arc::new(Mutex::new(Self::get_current_time()));
        let network_status = Arc::new(Mutex::new("🌐 Connected".to_string()));
        
        // Start time update thread
        let time_clone = Arc::clone(&time);
        thread::spawn(move || loop {
            thread::sleep(Duration::from_secs(1));
            *time_clone.lock().unwrap() = Self::get_current_time();
        });
        
        StatusBar { time, network_status }
    }
    
    fn get_current_time() -> String {
        Local::now().format("%H:%M:%S").to_string()
    }
    
    pub fn create_view(&self) -> impl View {
        let mut layout = LinearLayout::horizontal();
        
        // Combined menu button (penguin + text)
        let menu_button = CombinedMenuButton::new();
        layout.add_child(menu_button);
        
        // Spacer to push clock and network to the right
        layout.add_child(DummyView.full_width());
        
        // Clock on the right
        let time_view = TextView::new(self.time.lock().unwrap().clone())
            .with_name("clock")
            .fixed_width(10);
        layout.add_child(time_view);
        
        // Network indicator
        layout.add_child(TextView::new(" | "));
        let network_view = TextView::new(self.network_status.lock().unwrap().clone())
            .with_name("network")
            .fixed_width(15);
        layout.add_child(network_view);
        
        // Wrap in a layer with fixed height and styling
        Layer::new(layout)
            .fixed_height(1)
            .full_width()
    }
    
    pub fn update_time(&self, siv: &mut Cursive) {
        let time = self.time.lock().unwrap().clone();
        siv.call_on_name("clock", |view: &mut TextView| {
            view.set_content(time);
        });
    }
}

// Combined menu button that includes both penguin icon and menu text
struct CombinedMenuButton {
    hovered: bool,
}

impl CombinedMenuButton {
    fn new() -> Self {
        CombinedMenuButton { hovered: false }
    }
    
    fn create_menu() -> Rc<Tree> {
        let mut menu = Tree::new();
        
        // Terminal option
        menu.add_leaf("Terminal", |s| {
            s.add_layer(
                Dialog::text("Terminal functionality coming soon!")
                    .title("Terminal")
                    .button("OK", |s| { s.pop_layer(); })
            );
        });
        
        // Themes option
        menu.add_leaf("Themes", |s| {
            s.add_layer(
                Dialog::text("Theme selector coming soon!\n\nWill include:\n• Dark theme\n• Light theme\n• High contrast\n• Custom themes")
                    .title("Themes")
                    .button("OK", |s| { s.pop_layer(); })
            );
        });
        
        // GNU Parted option
        menu.add_leaf("Disk Partitioner", |s| {
            show_disk_partitioner_menu(s);
        });
        
        // Games submenu
        let mut games_menu = Tree::new();
        games_menu.add_leaf("Solitaire", |s| {
            s.add_layer(
                Dialog::text("Classic Solitaire (Klondike) coming soon!")
                    .title("Solitaire")
                    .button("OK", |s| { s.pop_layer(); })
            );
        });
        games_menu.add_leaf("Blackjack", |s| {
            s.add_layer(
                Dialog::text("Blackjack card game coming soon!")
                    .title("Blackjack")
                    .button("OK", |s| { s.pop_layer(); })
            );
        });
        menu.add_subtree("Games", games_menu);
        
        Rc::new(menu)
    }
}

impl View for CombinedMenuButton {
    fn draw(&self, printer: &Printer) {
        let style = if self.hovered {
            ColorStyle::new(
                Color::Light(BaseColor::Black),
                Color::Light(BaseColor::White)
            )
        } else {
            ColorStyle::primary()
        };
        
        printer.with_color(style, |printer| {
            printer.print((0, 0), " 🐧 Menu");
        });
    }
    
    fn required_size(&mut self, _constraint: Vec2) -> Vec2 {
        Vec2::new(9, 1) // " 🐧 Menu" = 9 chars
    }
    
    fn on_event(&mut self, event: Event) -> EventResult {
        match event {
            Event::Mouse {
                event: MouseEvent::Press(MouseButton::Left),
                ..
            } => {
                EventResult::with_cb(|s| {
                    crate::ui::logger::log_ui_action("MENU_OPEN", "Status Bar Penguin Menu");
                    let menu = CombinedMenuButton::create_menu();
                    let menu_popup = MenuPopup::new(menu)
                        .on_dismiss(|s| {
                            crate::ui::logger::log_ui_action("MENU_CLOSE", "Status Bar Penguin Menu");
                            s.pop_layer();
                        });
                    
                    s.add_layer(menu_popup);
                })
            }
            Event::Mouse {
                event: MouseEvent::Hold(_),
                ..
            } => {
                self.hovered = true;
                EventResult::Consumed(None)
            }
            Event::Key(key) => {
                // Allow keyboard activation
                match key {
                    Key::Enter | Key::F10 => {
                        EventResult::with_cb(|s| {
                            let menu = CombinedMenuButton::create_menu();
                            let menu_popup = MenuPopup::new(menu)
                                .on_dismiss(|s| {
                                    s.pop_layer();
                                });
                            
                            s.add_layer(menu_popup);
                        })
                    }
                    _ => EventResult::Ignored,
                }
            }
            _ => EventResult::Ignored,
        }
    }
    
    fn take_focus(&mut self, _: cursive::direction::Direction) -> Result<EventResult, cursive::view::CannotFocus> {
        Ok(EventResult::Consumed(None))
    }
}


struct MenuButton {
    hovered: bool,
}

impl MenuButton {
    fn new() -> Self {
        MenuButton { hovered: false }
    }
}

impl View for MenuButton {
    fn draw(&self, printer: &Printer) {
        let style = if self.hovered {
            ColorStyle::new(
                Color::Light(BaseColor::Black),
                Color::Light(BaseColor::White)
            )
        } else {
            ColorStyle::primary()
        };
        
        printer.with_color(style, |printer| {
            printer.print((0, 0), " Menu");
        });
    }
    
    fn required_size(&mut self, _constraint: Vec2) -> Vec2 {
        Vec2::new(5, 1)
    }
    
    fn on_event(&mut self, event: Event) -> EventResult {
        match event {
            Event::Mouse {
                event: MouseEvent::Press(MouseButton::Left),
                position,
                offset,
            } => {
                // Check if click is within our bounds
                if position.x >= offset.x && position.x < offset.x + 5 &&
                   position.y >= offset.y && position.y < offset.y + 1 {
                    EventResult::with_cb(|s| {
                        let menu = CombinedMenuButton::create_menu();
                        let menu_popup = MenuPopup::new(menu)
                            .on_dismiss(|s| {
                                s.pop_layer();
                            });
                        
                        s.add_layer(menu_popup);
                    })
                } else {
                    EventResult::Ignored
                }
            }
            Event::Mouse {
                event: MouseEvent::Hold(_),
                position,
                offset,
            } => {
                let was_hovered = self.hovered;
                self.hovered = position.x >= offset.x && position.x < offset.x + 5 &&
                              position.y >= offset.y && position.y < offset.y + 1;
                if was_hovered != self.hovered {
                    EventResult::Consumed(None)
                } else {
                    EventResult::Ignored
                }
            }
            Event::Key(key) => {
                // Allow keyboard activation
                match key {
                    Key::Enter => {
                        EventResult::with_cb(|s| {
                            let menu = CombinedMenuButton::create_menu();
                            let menu_popup = MenuPopup::new(menu)
                                .on_dismiss(|s| {
                                    s.pop_layer();
                                });
                            
                            s.add_layer(menu_popup);
                        })
                    }
                    _ => EventResult::Ignored,
                }
            }
            _ => EventResult::Ignored,
        }
    }
    
    fn take_focus(&mut self, _: cursive::direction::Direction) -> Result<EventResult, cursive::view::CannotFocus> {
        Ok(EventResult::Consumed(None))
    }
}

fn show_disk_partitioner_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🗂️ GNU Parted TUI Frontend").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Detect available disks
    layout.add_child(TextView::new("Available Storage Devices:").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut device_select = SelectView::<String>::new();
    
    // Add detected devices (simulation)
    device_select.add_item("💾 /dev/nvme0n1 - Samsung SSD 970 EVO Plus 1TB (931.5 GB)", "/dev/nvme0n1".to_string());
    device_select.add_item("💿 /dev/mmcblk0 - eMMC Storage (29.1 GB)", "/dev/mmcblk0".to_string());
    device_select.add_item("🔌 /dev/sda - SanDisk USB 3.0 (14.9 GB)", "/dev/sda".to_string());
    device_select.add_item("💾 /dev/nvme1n1 - WD Blue SN550 500GB (465.8 GB)", "/dev/nvme1n1".to_string());
    
    device_select.set_on_select(|s, device| {
        update_device_info(s, device);
    });
    
    layout.add_child(device_select.with_name("device_select"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Device info display
    layout.add_child(TextView::new("Device Information:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    layout.add_child(TextView::new("Select a device to view details").with_name("device_info"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Partition operations
    layout.add_child(TextView::new("Partition Operations:").style(ColorStyle::from(Color::Light(BaseColor::Cyan))));
    layout.add_child(TextView::new("⚠️ WARNING: These operations can destroy data!").style(ColorStyle::from(Color::Light(BaseColor::Red))));
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("GNU Parted - Disk Partitioner")
        .button("📋 View Partition Table", |s| {
            let selected_device = s.call_on_name("device_select", |view: &mut SelectView<String>| {
                view.selection().map(|sel| sel.clone())
            }).flatten();
            
            if let Some(device) = selected_device {
                show_partition_table(s, &device);
            } else {
                s.add_layer(
                    Dialog::text("Please select a device first.")
                        .title("No Device Selected")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        })
        .button("🔧 Partition Operations", |s| {
            let selected_device = s.call_on_name("device_select", |view: &mut SelectView<String>| {
                view.selection().map(|sel| sel.clone())
            }).flatten();
            
            if let Some(device) = selected_device {
                show_partition_operations(s, &device);
            } else {
                s.add_layer(
                    Dialog::text("Please select a device first.")
                        .title("No Device Selected")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        })
        .button("🏷️ Format Partition", |s| {
            let selected_device = s.call_on_name("device_select", |view: &mut SelectView<String>| {
                view.selection().map(|sel| sel.clone())
            }).flatten();
            
            if let Some(device) = selected_device {
                show_format_menu(s, &device);
            } else {
                s.add_layer(
                    Dialog::text("Please select a device first.")
                        .title("No Device Selected")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        })
        .button("❌ Close", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
    update_device_info(siv, &"/dev/nvme0n1".to_string());
}

fn update_device_info(siv: &mut Cursive, device: &String) {
    let info = match device.as_str() {
        "/dev/nvme0n1" => "📊 Samsung SSD 970 EVO Plus 1TB\n💾 Size: 931.5 GB (1,000,204,886,016 bytes)\n🔌 Interface: NVMe PCIe 3.0 x4\n📈 Health: Good (95% life remaining)\n🏷️ Model: MZ-V7S1T0BW",
        "/dev/mmcblk0" => "📊 eMMC Storage\n💾 Size: 29.1 GB (31,268,536,320 bytes)\n🔌 Interface: eMMC 5.1\n📈 Health: Good\n🏷️ Model: Orange Pi 5 Plus Internal",
        "/dev/sda" => "📊 SanDisk USB 3.0\n💾 Size: 14.9 GB (15,931,539,456 bytes)\n🔌 Interface: USB 3.0\n📈 Health: Good\n🏷️ Model: SanDisk Ultra",
        "/dev/nvme1n1" => "📊 WD Blue SN550 500GB\n💾 Size: 465.8 GB (500,107,862,016 bytes)\n🔌 Interface: NVMe PCIe 3.0 x4\n📈 Health: Excellent (98% life remaining)\n🏷️ Model: WDS500G2B0C",
        _ => "No device information available",
    };
    
    siv.call_on_name("device_info", |view: &mut TextView| {
        view.set_content(info);
    });
}

fn show_partition_table(siv: &mut Cursive, device: &str) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new(format!("📋 Partition Table for {}", device)).style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Simulate partition table display
    let partition_info = match device {
        "/dev/nvme0n1" => "Partition Table: GPT\n\nPartition  Start    End      Size     Type                 Name\n─────────────────────────────────────────────────────────────────\n1          1MiB     513MiB   512MiB   EFI System           EFI System\n2          513MiB   100GiB   99.5GiB  Linux filesystem     Ubuntu Root\n3          100GiB   931GiB   831GiB   Linux filesystem     Home\n\n✅ Table is valid and consistent",
        "/dev/mmcblk0" => "Partition Table: MBR\n\nPartition  Start    End      Size     Type                 Bootable\n─────────────────────────────────────────────────────────────────\n1          1MiB     513MiB   512MiB   FAT32               *\n2          513MiB   29.1GiB  28.6GiB  ext4                 \n\n✅ Table is valid and consistent",
        "/dev/sda" => "Partition Table: MBR\n\nPartition  Start    End      Size     Type                 Bootable\n─────────────────────────────────────────────────────────────────\n1          1MiB     14.9GiB  14.9GiB  FAT32               *\n\n✅ Table is valid and consistent",
        _ => "No partition table information available",
    };
    
    layout.add_child(TextView::new(partition_info).style(ColorStyle::from(Color::Light(BaseColor::White))));
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Partition Table Information")
        .button("🔄 Refresh", |s| {
            s.add_layer(
                Dialog::text("Partition table refreshed!")
                    .title("Refreshed")
                    .button("OK", |s| { s.pop_layer(); })
            );
        })
        .button("💾 Save to File", |s| {
            s.add_layer(
                Dialog::text("Partition table saved to ~/partition_table.txt")
                    .title("Saved")
                    .button("OK", |s| { s.pop_layer(); })
            );
        })
        .button("❌ Close", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
}

fn show_partition_operations(siv: &mut Cursive, device: &str) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new(format!("🔧 Partition Operations for {}", device)).style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("⚠️ DANGER ZONE ⚠️").style(ColorStyle::from(Color::Light(BaseColor::Red))));
    layout.add_child(TextView::new("These operations can permanently destroy data!"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut ops_select = SelectView::<&str>::new();
    ops_select.add_item("➕ Create New Partition", "create");
    ops_select.add_item("🗑️ Delete Partition", "delete");
    ops_select.add_item("📐 Resize Partition", "resize");
    ops_select.add_item("🔄 Move Partition", "move");
    ops_select.add_item("🏷️ Set Partition Label", "label");
    ops_select.add_item("🏁 Set Boot Flag", "boot");
    ops_select.add_item("🔄 Convert Partition Table (MBR ↔ GPT)", "convert");
    
    layout.add_child(ops_select.with_name("ops_select"));
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Partition Operations")
        .button("▶️ Execute Operation", |s| {
            let selected_op = s.call_on_name("ops_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(op) = selected_op {
                show_operation_confirmation(s, op);
            }
        })
        .button("❌ Cancel", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
}

fn show_operation_confirmation(siv: &mut Cursive, operation: &str) {
    let (title, message) = match operation {
        "create" => ("Create Partition", "This will create a new partition.\nAll data in the allocated space will be lost."),
        "delete" => ("Delete Partition", "⚠️ This will permanently delete the partition and ALL data on it!\nThis operation cannot be undone."),
        "resize" => ("Resize Partition", "This will resize the partition.\nData may be lost if shrinking."),
        "move" => ("Move Partition", "This will move the partition to a new location.\nThis operation can take a long time."),
        "label" => ("Set Label", "This will set a new label for the partition."),
        "boot" => ("Set Boot Flag", "This will toggle the boot flag on the partition."),
        "convert" => ("Convert Partition Table", "⚠️ This will convert between MBR and GPT.\nAll data will be lost!"),
        _ => ("Unknown Operation", "Unknown operation selected."),
    };
    
    let dialog = Dialog::text(format!("{}\n\nAre you sure you want to continue?", message))
        .title(title)
        .button("✅ Confirm", |s| {
            s.pop_layer();
            s.add_layer(
                Dialog::text("Operation completed successfully!\n\n(This is a simulation - no actual changes were made)")
                    .title("Success")
                    .button("OK", |s| { s.pop_layer(); })
            );
        })
        .button("❌ Cancel", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
}

fn show_format_menu(siv: &mut Cursive, device: &str) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new(format!("🏷️ Format Partition on {}", device)).style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Select filesystem type:"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut fs_select = SelectView::<&str>::new();
    fs_select.add_item("📁 ext4 - Linux default (recommended)", "ext4");
    fs_select.add_item("📁 ext3 - Linux legacy", "ext3");
    fs_select.add_item("📁 ext2 - Linux basic", "ext2");
    fs_select.add_item("📁 FAT32 - Universal compatibility", "fat32");
    fs_select.add_item("📁 NTFS - Windows compatibility", "ntfs");
    fs_select.add_item("📁 exFAT - Large file support", "exfat");
    fs_select.add_item("📁 XFS - High performance", "xfs");
    fs_select.add_item("📁 Btrfs - Modern features", "btrfs");
    
    layout.add_child(fs_select.with_name("fs_select"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("⚠️ WARNING: Formatting will erase all data on the partition!").style(ColorStyle::from(Color::Light(BaseColor::Red))));
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Format Partition")
        .button("🔥 Format Now", |s| {
            let selected_fs = s.call_on_name("fs_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(fs) = selected_fs {
                s.add_layer(
                    Dialog::text(format!("Are you absolutely sure you want to format with {}?\n\nALL DATA WILL BE PERMANENTLY LOST!", fs))
                        .title("⚠️ FINAL WARNING")
                        .button("YES, FORMAT NOW", |s| {
                            s.pop_layer();
                            s.add_layer(
                                Dialog::text("Formatting completed successfully!\n\n(This is a simulation - no actual formatting occurred)")
                                    .title("Format Complete")
                                    .button("OK", |s| { s.pop_layer(); })
                            );
                        })
                        .button("Cancel", |s| { s.pop_layer(); })
                );
            }
        })
        .button("❌ Cancel", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
}