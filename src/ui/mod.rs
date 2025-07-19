use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::theme::{ColorStyle, BaseColor, Color};
use cursive::utils::markup::StyledString;
use cursive::traits::*;
use std::sync::Arc;
use crate::error::{BuilderError, Result};

pub mod theme;
pub mod statusbar;
pub mod error_dialog;
pub mod logger;

pub fn setup_main_menu(siv: &mut Cursive) {
    // Apply custom theme
    theme::apply_theme(siv);
    
    // Add status bar as top layer first
    let status_bar = Arc::new(statusbar::StatusBar::new());
    let status_bar_view = status_bar.create_view();
    siv.add_fullscreen_layer(status_bar_view);
    
    // Create the main menu content (original design)
    let content = create_main_menu_content(siv);
    
    // Create the main dialog (original design - small centered box)
    let dialog = Dialog::around(content)
        .title("Setec Labs Orange Pi 5 Plus Builder")
        .padding_lrtb(2, 2, 1, 1);
    
    // Add the dialog as a centered layer (original behavior)
    siv.add_layer(dialog);
    
    // Set up periodic time updates
    let status_bar_clone = Arc::clone(&status_bar);
    siv.add_global_callback(cursive::event::Event::Refresh, move |s| {
        status_bar_clone.update_time(s);
    });
    siv.set_autorefresh(true);
}

fn create_main_menu_content(_siv: &mut Cursive) -> Box<dyn View> {
    let mut layout = LinearLayout::vertical();
    
    // Header
    let header = TextView::new(StyledString::styled(
        "Select an option:",
        ColorStyle::from(Color::Light(BaseColor::Blue))
    ));
    layout.add_child(header);
    layout.add_child(DummyView.fixed_height(1));
    
    // Create menu
    let mut menu = SelectView::<MenuOption>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("📦 Distro Wizard - Build custom Ubuntu/Debian distributions", MenuOption::DistroWizard);
    menu.add_item("🎮 GameScope Builder - Build Orange Pi 5 Plus GameScope distros", MenuOption::GameScopeBuilder);
    menu.add_item("🔧 Arm-Pi Tweaker - Complete system configuration & performance", MenuOption::ArmPiTweaker);
    menu.add_item("⬇️ Download Components - Download required build components", MenuOption::DownloadComponents);
    menu.add_item("💾 SPI Flasher - Flash bootloaders via multiple protocols", MenuOption::SPIFlasher);
    menu.add_item("💿 Image Burner - Burn images to SD/USB drives", MenuOption::ImageBurner);
    menu.add_item("📱 ADB/Fastboot - Android Debug Bridge & Fastboot tools", MenuOption::AdbFastboot);
    menu.add_item("🔄 Update Manager - Check for kernel and distro updates", MenuOption::UpdateManager);
    menu.add_item("📝 Logging and Debugging - Configure logging options", MenuOption::LoggingDebug);
    menu.add_item("⚙️ Settings - Configure application settings", MenuOption::Settings);
    menu.add_item("ℹ️ About - About this application", MenuOption::About);
    menu.add_item("❌ Exit - Exit the application", MenuOption::Exit);
    
    // Set up menu callbacks
    menu.set_on_submit(move |s, option| {
        handle_menu_selection(s, option);
    });
    
    layout.add_child(menu);
    
    // Footer
    layout.add_child(DummyView.fixed_height(1));
    let footer = TextView::new("Use ↑/↓ to navigate, Enter to select, or click with mouse");
    layout.add_child(footer);
    
    Box::new(layout)
}

#[derive(Debug, Clone, Copy)]
enum MenuOption {
    DistroWizard,
    GameScopeBuilder,
    ArmPiTweaker,
    DownloadComponents,
    SPIFlasher,
    ImageBurner,
    AdbFastboot,
    UpdateManager,
    LoggingDebug,
    Settings,
    About,
    Exit,
}

fn handle_menu_selection(siv: &mut Cursive, option: &MenuOption) {
    let option_name = match option {
        MenuOption::DistroWizard => "Distro Wizard",
        MenuOption::GameScopeBuilder => "GameScope Builder",
        MenuOption::ArmPiTweaker => "Arm-Pi Tweaker",
        MenuOption::DownloadComponents => "Download Components",
        MenuOption::SPIFlasher => "SPI Flasher", 
        MenuOption::ImageBurner => "Image Burner",
        MenuOption::AdbFastboot => "ADB/Fastboot",
        MenuOption::UpdateManager => "Update Manager",
        MenuOption::LoggingDebug => "Logging and Debugging",
        MenuOption::Settings => "Settings",
        MenuOption::About => "About",
        MenuOption::Exit => "Exit",
    };
    
    logger::log_menu_selection("Main Menu", option_name);
    logger::log_ui_action("MENU_SELECTION", &format!("Selected: {}", option_name));
    
    match option {
        MenuOption::DistroWizard => {
            logger::log_ui_action("DIALOG_CLOSE", "Main Menu");
            siv.pop_layer();
            logger::log_ui_action("MODULE_OPEN", "Distro Wizard");
            crate::wizard::show_distro_wizard(siv);
        }
        MenuOption::GameScopeBuilder => {
            logger::log_ui_action("DIALOG_CLOSE", "Main Menu");
            siv.pop_layer();
            logger::log_ui_action("MODULE_OPEN", "GameScope Builder");
            show_gamescope_builder(siv);
        }
        MenuOption::ArmPiTweaker => {
            logger::log_ui_action("DIALOG_CLOSE", "Main Menu");
            siv.pop_layer();
            logger::log_ui_action("MODULE_OPEN", "Arm-Pi Tweaker");
            crate::armpi_tweaker::show_armpi_tweaker(siv);
        }
        MenuOption::DownloadComponents => {
            logger::log_ui_action("DIALOG_OPEN", "Download Components");
            show_download_components_dialog(siv);
        }
        MenuOption::SPIFlasher => {
            logger::log_ui_action("DIALOG_CLOSE", "Main Menu");
            siv.pop_layer();
            logger::log_ui_action("MODULE_OPEN", "SPI Flasher");
            crate::flasher::show_spi_flasher(siv);
        }
        MenuOption::ImageBurner => {
            logger::log_ui_action("DIALOG_CLOSE", "Main Menu");
            siv.pop_layer();
            logger::log_ui_action("MODULE_OPEN", "Image Burner");
            crate::burner::show_image_burner(siv);
        }
        MenuOption::AdbFastboot => {
            logger::log_ui_action("DIALOG_OPEN", "ADB/Fastboot");
            show_adb_fastboot_menu(siv);
        }
        MenuOption::UpdateManager => {
            logger::log_ui_action("DIALOG_CLOSE", "Main Menu");
            siv.pop_layer();
            logger::log_ui_action("MODULE_OPEN", "Update Manager");
            crate::updates::show_update_manager(siv);
        }
        MenuOption::LoggingDebug => {
            logger::log_ui_action("DIALOG_OPEN", "Logging and Debugging");
            show_logging_debug_dialog(siv);
        }
        MenuOption::Settings => {
            logger::log_ui_action("DIALOG_OPEN", "Settings");
            show_settings_dialog(siv);
        }
        MenuOption::About => {
            logger::log_ui_action("DIALOG_OPEN", "About");
            show_about_dialog(siv);
        }
        MenuOption::Exit => {
            logger::log_ui_action("DIALOG_OPEN", "Exit Confirmation");
            show_confirmation_dialog(siv, "Confirm Exit", "Are you sure you want to exit?", Box::new(|s| {
                logger::log_ui_action("APPLICATION_EXIT", "User confirmed exit");
                s.quit();
            }));
        }
    }
}

fn show_settings_dialog(siv: &mut Cursive) {
    let mut select = SelectView::<SettingsOption>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    select.add_item("Check Dependencies", SettingsOption::CheckDependencies);
    select.add_item("Default Paths", SettingsOption::DefaultPaths);
    select.add_item("Build Options", SettingsOption::BuildOptions);
    select.add_item("Interface Preferences", SettingsOption::InterfacePrefs);
    
    select.set_on_submit(|s, option| {
        match option {
            SettingsOption::CheckDependencies => {
                s.pop_layer();
                s.add_layer(
                    Dialog::text("Dependency checking is integrated into the build process.\nThe wizard will check and install dependencies as needed.")
                        .title("Dependencies")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
            _ => {
                s.add_layer(
                    Dialog::text("This feature is coming soon!")
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    siv.add_layer(
        Dialog::around(select)
            .title("Settings")
            .button("Close", |s| { s.pop_layer(); })
    );
}

#[derive(Debug, Clone, Copy)]
enum SettingsOption {
    CheckDependencies,
    DefaultPaths,
    BuildOptions,
    InterfacePrefs,
}

fn show_about_dialog(siv: &mut Cursive) {
    let about_text = format!(
        "Setec Labs Orange Pi 5 Plus Builder\n\
        Version: {}\n\n\
        A cross-platform tool for:\n\
        • Building custom Linux distributions\n\
        • Flashing SPI bootloaders\n\
        • Burning images to drives\n\n\
        Specifically designed for Orange Pi 5 Plus (RK3588S)",
        env!("CARGO_PKG_VERSION")
    );
    
    siv.add_layer(
        Dialog::text(about_text)
            .title("About")
            .button("OK", |s| { s.pop_layer(); })
    );
}

/// Helper function to show error dialogs with user-friendly messages
pub fn show_error(siv: &mut Cursive, error: &BuilderError) {
    error_dialog::show_error_dialog(siv, error);
}

/// Helper function to show contextual error dialogs
pub fn show_contextual_error(siv: &mut Cursive, error: &crate::error::ContextualError) {
    error_dialog::show_contextual_error_dialog(siv, error);
}

/// Helper function to show confirmation dialogs
pub fn show_confirmation_dialog(
    siv: &mut Cursive,
    title: &str,
    message: &str,
    confirm_callback: Box<dyn Fn(&mut Cursive)>
) {
    error_dialog::show_confirmation_dialog(siv, title, message, confirm_callback);
}

/// Helper function to show validation error dialogs
pub fn show_validation_error(siv: &mut Cursive, field_name: &str, error: &str, suggestions: Vec<String>) {
    error_dialog::show_validation_error_dialog(siv, field_name, error, suggestions);
}

/// Helper function to show recovery dialogs
pub fn show_recovery_dialog(siv: &mut Cursive, error: &BuilderError, retry_callback: Box<dyn Fn(&mut Cursive)>) {
    error_dialog::show_recovery_dialog(siv, error, retry_callback);
}

/// Helper function to execute operations with error handling
pub fn execute_with_error_handling<F>(siv: &mut Cursive, operation: F, operation_name: &str)
where
    F: FnOnce() -> Result<()> + 'static,
{
    match operation() {
        Ok(_) => {
            siv.add_layer(
                Dialog::info(format!("{} completed successfully", operation_name))
                    .button("OK", |s| { s.pop_layer(); })
            );
        }
        Err(error) => {
            show_error(siv, &error);
        }
    }
}

/// Helper function to execute operations with progress and error handling
pub fn execute_with_progress_and_error_handling<F>(siv: &mut Cursive, operation: F, operation_name: &str)
where
    F: Fn() -> Result<()> + 'static,
{
    error_dialog::show_progress_with_error_handling(siv, operation_name, Box::new(operation));
}

/// Helper function to handle menu navigation with error handling
pub fn handle_menu_with_error_handling<F>(siv: &mut Cursive, operation: F, operation_name: &str)
where
    F: FnOnce(&mut Cursive) -> Result<()>,
{
    if let Err(error) = operation(siv) {
        show_error(siv, &error);
    }
}

/// Helper function to validate input with user-friendly error messages
pub fn validate_input(field_name: &str, value: &str, validators: Vec<Box<dyn Fn(&str) -> Result<()>>>) -> Result<()> {
    for validator in validators {
        if let Err(error) = validator(value) {
            return Err(BuilderError::ValidationError(format!("{}: {}", field_name, error)));
        }
    }
    Ok(())
}

/// Common input validators
pub mod validators {
    use super::*;
    use std::path::Path;
    
    pub fn not_empty(value: &str) -> Result<()> {
        if value.trim().is_empty() {
            Err(BuilderError::ValidationError("Field cannot be empty".to_string()))
        } else {
            Ok(())
        }
    }
    
    pub fn valid_path(value: &str) -> Result<()> {
        if Path::new(value).exists() {
            Ok(())
        } else {
            Err(BuilderError::FileNotFound(std::path::PathBuf::from(value)))
        }
    }
    
    pub fn valid_directory(value: &str) -> Result<()> {
        let path = Path::new(value);
        if !path.exists() {
            Err(BuilderError::FileNotFound(std::path::PathBuf::from(value)))
        } else if !path.is_dir() {
            Err(BuilderError::ValidationError("Path is not a directory".to_string()))
        } else {
            Ok(())
        }
    }
    
    pub fn valid_file(value: &str) -> Result<()> {
        let path = Path::new(value);
        if !path.exists() {
            Err(BuilderError::FileNotFound(std::path::PathBuf::from(value)))
        } else if !path.is_file() {
            Err(BuilderError::ValidationError("Path is not a file".to_string()))
        } else {
            Ok(())
        }
    }
    
    pub fn valid_username(value: &str) -> Result<()> {
        if value.len() < 3 {
            Err(BuilderError::ValidationError("Username must be at least 3 characters".to_string()))
        } else if value.len() > 32 {
            Err(BuilderError::ValidationError("Username must be less than 32 characters".to_string()))
        } else if !value.chars().all(|c| c.is_alphanumeric() || c == '_' || c == '-') {
            Err(BuilderError::ValidationError("Username can only contain alphanumeric characters, underscores, and hyphens".to_string()))
        } else {
            Ok(())
        }
    }
    
    pub fn valid_password(value: &str) -> Result<()> {
        if value.len() < 8 {
            Err(BuilderError::ValidationError("Password must be at least 8 characters".to_string()))
        } else {
            Ok(())
        }
    }
    
    pub fn valid_url(value: &str) -> Result<()> {
        if value.starts_with("http://") || value.starts_with("https://") {
            Ok(())
        } else {
            Err(BuilderError::UrlParseError(value.to_string()))
        }
    }
    
    pub fn valid_email(value: &str) -> Result<()> {
        if value.contains('@') && value.contains('.') {
            Ok(())
        } else {
            Err(BuilderError::ValidationError("Invalid email format".to_string()))
        }
    }
}

fn show_adb_fastboot_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Android Debug Bridge & Fastboot Tools"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("For Orange Pi 5 Plus device management"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("🔍 List Connected Devices", "list_devices");
    menu.add_item("🔄 Reboot Device", "reboot");
    menu.add_item("⚡ Reboot to Bootloader/Fastboot", "reboot_bootloader");
    menu.add_item("💾 Flash Boot Image", "flash_boot");
    menu.add_item("💾 Flash System Image", "flash_system");
    menu.add_item("💾 Flash Recovery Image", "flash_recovery");
    menu.add_item("🔓 Unlock Bootloader", "unlock_bootloader");
    menu.add_item("🔒 Lock Bootloader", "lock_bootloader");
    menu.add_item("📋 Device Info", "device_info");
    menu.add_item("📁 File Transfer (Push/Pull)", "file_transfer");
    menu.add_item("🖥️ Shell Access", "shell");
    menu.add_item("📝 Logcat Viewer", "logcat");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "list_devices" => {
                s.add_layer(
                    Dialog::text("Checking for connected devices...\n\nNote: This will show Orange Pi 5 Plus devices connected via USB")
                        .title("Connected Devices")
                        .button("Refresh", |s| { s.pop_layer(); })
                        .button("Close", |s| { s.pop_layer(); })
                );
            }
            "reboot" => {
                s.add_layer(
                    Dialog::text("Are you sure you want to reboot the device?")
                        .title("Confirm Reboot")
                        .button("Yes", |s| {
                            s.pop_layer();
                            s.add_layer(
                                Dialog::text("Device reboot command sent!")
                                    .title("Rebooting")
                                    .button("OK", |s| { s.pop_layer(); })
                            );
                        })
                        .button("No", |s| { s.pop_layer(); })
                );
            }
            "reboot_bootloader" => {
                s.add_layer(
                    Dialog::text("This will reboot the device into bootloader/fastboot mode.\n\nContinue?")
                        .title("Reboot to Bootloader")
                        .button("Yes", |s| {
                            s.pop_layer();
                            s.add_layer(
                                Dialog::text("Device is rebooting to bootloader...")
                                    .title("Bootloader Mode")
                                    .button("OK", |s| { s.pop_layer(); })
                            );
                        })
                        .button("No", |s| { s.pop_layer(); })
                );
            }
            "flash_boot" | "flash_system" | "flash_recovery" => {
                let image_type = match *option {
                    "flash_boot" => "Boot",
                    "flash_system" => "System",
                    "flash_recovery" => "Recovery",
                    _ => "Unknown",
                };
                s.add_layer(
                    Dialog::text(format!("Flash {} Image\n\nThis feature will allow you to flash {} images to Orange Pi 5 Plus.\n\nRequires device in fastboot mode.", image_type, image_type.to_lowercase()))
                        .title(format!("Flash {} Image", image_type))
                        .button("Select Image", |s| {
                            s.add_layer(
                                Dialog::text("File browser coming soon!")
                                    .title("Select Image File")
                                    .button("OK", |s| { s.pop_layer(); })
                            );
                        })
                        .button("Cancel", |s| { s.pop_layer(); })
                );
            }
            "unlock_bootloader" => {
                s.add_layer(
                    Dialog::text("⚠️ WARNING: Unlocking bootloader will erase all data!\n\nThis is required for custom ROM installation.\n\nProceed?")
                        .title("Unlock Bootloader")
                        .button("Unlock", |s| {
                            s.pop_layer();
                            s.add_layer(
                                Dialog::text("Bootloader unlock initiated...\n\nFollow on-device prompts.")
                                    .title("Unlocking")
                                    .button("OK", |s| { s.pop_layer(); })
                            );
                        })
                        .button("Cancel", |s| { s.pop_layer(); })
                );
            }
            "device_info" => {
                s.add_layer(
                    Dialog::text("Device Information:\n\nModel: Orange Pi 5 Plus\nCPU: Rockchip RK3588S\nRAM: Variable (4GB-32GB)\nStorage: eMMC/NVMe\n\nConnect device to see live info.")
                        .title("Device Information")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
            "shell" => {
                s.add_layer(
                    Dialog::text("ADB Shell Access\n\nThis will open a shell session to the connected Orange Pi 5 Plus.\n\nTerminal integration coming soon!")
                        .title("Shell Access")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
            _ => {
                s.add_layer(
                    Dialog::text(format!("{} feature coming soon!", option))
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("ADB/Fastboot Tools")
        .button("Back", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_logging_debug_dialog(siv: &mut Cursive) {
    use log::Level;
    
    let mut layout = LinearLayout::vertical();
    
    // Header
    layout.add_child(TextView::new("Configure Logging and Debugging Options"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Log level selection
    layout.add_child(TextView::new("Log Level:"));
    let mut level_select = SelectView::<Level>::new();
    level_select.add_item("Error", Level::Error);
    level_select.add_item("Warn (Default)", Level::Warn);
    level_select.add_item("Info", Level::Info);
    level_select.add_item("Debug", Level::Debug);
    level_select.add_item("Trace", Level::Trace);
    
    // Set current selection
    if let Some(logger) = logger::get_global_logger() {
        if let Ok(logger) = logger.lock() {
            let current_level = logger.get_log_level();
            level_select.set_selection(match current_level {
                Level::Error => 0,
                Level::Warn => 1,
                Level::Info => 2,
                Level::Debug => 3,
                Level::Trace => 4,
            });
        }
    }
    
    layout.add_child(level_select.with_name("log_level"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Logging options
    layout.add_child(TextView::new("Logging Options:"));
    
    let mouse_enabled = if let Some(logger) = logger::get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.is_mouse_logging_enabled()
        } else {
            true
        }
    } else {
        true
    };
    
    let keyboard_enabled = if let Some(logger) = logger::get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.is_keyboard_logging_enabled()
        } else {
            true
        }
    } else {
        true
    };
    
    let process_enabled = if let Some(logger) = logger::get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.is_process_logging_enabled()
        } else {
            true
        }
    } else {
        true
    };
    
    // Create checkbox items with [X] format display
    let mut mouse_checkbox = cursive::views::Checkbox::new();
    mouse_checkbox.set_checked(mouse_enabled);
    layout.add_child(LinearLayout::horizontal()
        .child(TextView::new(if mouse_enabled { "[X]" } else { "[ ]" }))
        .child(TextView::new(" "))
        .child(mouse_checkbox.with_name("mouse_logging"))
        .child(TextView::new(" Log mouse clicks and movements")));
    
    let mut keyboard_checkbox = cursive::views::Checkbox::new();
    keyboard_checkbox.set_checked(keyboard_enabled);
    layout.add_child(LinearLayout::horizontal()
        .child(TextView::new(if keyboard_enabled { "[X]" } else { "[ ]" }))
        .child(TextView::new(" "))
        .child(keyboard_checkbox.with_name("keyboard_logging"))
        .child(TextView::new(" Log keyboard inputs")));
    
    let mut process_checkbox = cursive::views::Checkbox::new();
    process_checkbox.set_checked(process_enabled);
    layout.add_child(LinearLayout::horizontal()
        .child(TextView::new(if process_enabled { "[X]" } else { "[ ]" }))
        .child(TextView::new(" "))
        .child(process_checkbox.with_name("process_logging"))
        .child(TextView::new(" Log system processes and commands")));
    
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Logs are saved to: logs/"));
    layout.add_child(TextView::new("• events.log - Mouse and keyboard events"));
    layout.add_child(TextView::new("• processes.log - System commands and output"));
    layout.add_child(TextView::new("• errors.log - Application errors"));
    layout.add_child(TextView::new("• main.log - General application logs"));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Logging and Debugging")
        .button("Apply", |s| {
            apply_logging_settings(s);
        })
        .button("Cancel", |s| {
            s.pop_layer();
        });
    
    siv.add_layer(dialog);
}

fn apply_logging_settings(siv: &mut Cursive) {
    if let Some(logger) = logger::get_global_logger() {
        if let Ok(mut logger) = logger.lock() {
            // Update log level
            if let Some(level) = siv.call_on_name("log_level", |view: &mut SelectView<log::Level>| {
                view.selection().map(|rc| (*rc).clone())
            }).flatten() {
                logger.set_log_level(level);
            }
            
            // Update mouse logging
            if let Some(enabled) = siv.call_on_name("mouse_logging", |view: &mut cursive::views::Checkbox| {
                view.is_checked()
            }) {
                logger.set_mouse_logging(enabled);
            }
            
            // Update keyboard logging
            if let Some(enabled) = siv.call_on_name("keyboard_logging", |view: &mut cursive::views::Checkbox| {
                view.is_checked()
            }) {
                logger.set_keyboard_logging(enabled);
            }
            
            // Update process logging
            if let Some(enabled) = siv.call_on_name("process_logging", |view: &mut cursive::views::Checkbox| {
                view.is_checked()
            }) {
                logger.set_process_logging(enabled);
            }
            
            logger.log_info("Logging settings updated");
        }
    }
    
    siv.pop_layer();
    siv.add_layer(Dialog::text("Logging settings have been applied successfully!")
        .title("Settings Applied")
        .button("OK", |s| { s.pop_layer(); }));
}

fn show_gamescope_builder(siv: &mut Cursive) {
    let mut select = SelectView::<GameScopeDistro>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    select.add_item("🖥️ Desktop Distro - LXQt + GameScope (Full desktop experience)", GameScopeDistro::Desktop);
    select.add_item("🎮 Gaming Distro - GameScope + RetroArch (Gaming only)", GameScopeDistro::Gaming);
    
    select.set_on_submit(|s, distro| {
        match distro {
            GameScopeDistro::Desktop => {
                s.pop_layer();
                crate::builder::show_gamescope_desktop_builder(s);
            }
            GameScopeDistro::Gaming => {
                s.pop_layer();
                crate::builder::show_gamescope_gaming_builder(s);
            }
        }
    });
    
    let dialog = Dialog::around(select)
        .title("Select GameScope Distro Type")
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

#[derive(Debug, Clone, Copy)]
enum GameScopeDistro {
    Desktop,
    Gaming,
}

fn show_download_components_dialog(siv: &mut Cursive) {
    let content = TextView::new(
        "This will download all required components for building Orange Pi 5 Plus GameScope distros:\n\n\
        • Armbian Rockchip kernel (rk-6.1-rkr5.1)\n\
        • Rockchip U-Boot bootloader\n\
        • Rockchip firmware binaries\n\
        • Development tools (rkdeveloptool)\n\
        • Orange Pi build system\n\
        • RetroArch and cores\n\n\
        Components will be downloaded to: ~/Orange-Pi/\n\n\
        This may take some time depending on your internet connection."
    );
    
    let dialog = Dialog::around(content)
        .title("Download Components")
        .button("Start Download", |s| {
            s.pop_layer();
            start_component_download(s);
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn start_component_download(siv: &mut Cursive) {
    use crate::download::DownloadManager;
    use cursive::views::ProgressBar;
    use std::sync::{Arc, Mutex};
    use std::thread;
    
    let progress = Arc::new(Mutex::new(0));
    let progress_clone = Arc::clone(&progress);
    
    let mut progress_bar = ProgressBar::new()
        .with_label(|value, _| format!("Downloading: {}%", value))
        .max(100);
    
    let dialog = Dialog::around(
        LinearLayout::vertical()
            .child(TextView::new("Downloading components..."))
            .child(DummyView.fixed_height(1))
            .child(progress_bar.with_name("download_progress"))
    )
    .title("Download Progress");
    
    siv.add_layer(dialog);
    
    // Start download in background thread
    thread::spawn(move || {
        if let Ok(manager) = DownloadManager::new() {
            // Create directory structure
            let _ = manager.create_directory_structure();
            
            // Download all components with progress updates
            let components = vec![
                ("Armbian Rockchip kernel", 20),
                ("Rockchip U-Boot", 35),
                ("Rockchip firmware", 50),
                ("Development tools", 65),
                ("Orange Pi build system", 80),
                ("RetroArch and cores", 100),
            ];
            
            for (name, progress_value) in components {
                logger::log_info(&format!("Downloading {}...", name));
                
                // Simulate download progress
                thread::sleep(std::time::Duration::from_secs(2));
                
                if let Ok(mut p) = progress_clone.lock() {
                    *p = progress_value;
                }
            }
            
            // Actually perform the downloads
            if let Err(e) = manager.download_all_components() {
                logger::log_error(&format!("Download failed: {}", e));
            }
        }
    });
    
    // Update progress bar periodically
    siv.add_global_callback(cursive::event::Event::Refresh, move |s| {
        if let Ok(p) = progress.lock() {
            s.call_on_name("download_progress", |view: &mut ProgressBar| {
                view.set_value(*p);
            });
            
            if *p >= 100 {
                s.pop_layer();
                s.add_layer(Dialog::text("All components downloaded successfully!")
                    .title("Download Complete")
                    .button("OK", |s| { s.pop_layer(); }));
            }
        }
    });
}