use std::path::PathBuf;
use std::thread;
use slint::*;

use crate::builder::gamescope_builder::{GameScopeBuilder, GameScopeConfig, KernelChoice, DesktopChoice};
use crate::error::BuilderError;

slint::include_modules!();

pub struct GameScopeGuiApp {
    ui: GameScopeWizard,
    build_thread: Option<thread::JoinHandle<()>>,
}

impl GameScopeGuiApp {
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let ui = GameScopeWizard::new()?;
        
        // Set up default values
        ui.set_hostname("orangepi-gamescope".into());
        ui.set_username("gamer".into());
        ui.set_locale("en_US.UTF-8".into());
        ui.set_timezone("UTC".into());
        ui.set_output_path("/tmp/gamescope-build".into());
        ui.set_kernel_choice(1); // Default to Rockchip 6.1 for gaming
        ui.set_desktop_choice(0); // Default to LXQt + GameScope
        
        Ok(Self {
            ui,
            build_thread: None,
        })
    }
    
    pub fn run(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        // Set up callbacks
        self.setup_callbacks()?;
        
        // Run the UI
        self.ui.run()?;
        
        // Wait for build thread to complete if it's running
        if let Some(handle) = self.build_thread.take() {
            let _ = handle.join();
        }
        
        Ok(())
    }
    
    fn setup_callbacks(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let ui_weak = self.ui.as_weak();
        
        // Start build callback
        self.ui.on_start_build(move || {
            let ui = ui_weak.upgrade().unwrap();
            
            // Validate inputs
            if ui.get_password().is_empty() || ui.get_root_password().is_empty() {
                // Show error dialog (would need to implement error dialogs)
                eprintln!("Password and root password are required");
                return;
            }
            
            // Create build configuration
            let config = Self::create_build_config(&ui);
            
            // Start the build in a separate thread
            ui.set_build_in_progress(true);
            ui.set_build_status("Initializing build...".into());
            ui.set_build_progress(0.0);
            
            let ui_weak_clone = ui_weak.clone();
            thread::spawn(move || {
                Self::run_build(config, ui_weak_clone);
            });
        });
        
        // Cancel build callback
        let ui_weak_cancel = self.ui.as_weak();
        self.ui.on_cancel_build(move || {
            let ui = ui_weak_cancel.upgrade().unwrap();
            ui.set_build_in_progress(false);
            ui.set_build_status("Build cancelled".into());
            ui.set_build_progress(0.0);
            // TODO: Actually cancel the build process
        });
        
        // Browse output path callback
        let ui_weak_browse = self.ui.as_weak();
        self.ui.on_browse_output_path(move || {
            // TODO: Implement file dialog
            // For now, just set a default path
            let ui = ui_weak_browse.upgrade().unwrap();
            ui.set_output_path("/tmp/gamescope-build".into());
        });
        
        Ok(())
    }
    
    fn create_build_config(ui: &GameScopeWizard) -> crate::builder::BuildConfig {
        let kernel_choice = match ui.get_kernel_choice() {
            0 => "5.1",
            1 => "6.1",
            _ => "6.1",
        };
        
        let desktop_choice = match ui.get_desktop_choice() {
            0 => "lxqt-gamescope",
            1 => "gamescope-retroarch",
            _ => "lxqt-gamescope",
        };
        
        crate::builder::BuildConfig {
            distro: "debian".to_string(),
            distro_version: "12".to_string(),
            kernel_version: kernel_choice.to_string(),
            desktop_environment: Some(desktop_choice.to_string()),
            gpu_driver: Some("mali".to_string()),
            bootloader: "u-boot".to_string(),
            hostname: ui.get_hostname().as_str().to_string(),
            username: ui.get_username().as_str().to_string(),
            password: ui.get_password().as_str().to_string(),
            root_password: ui.get_root_password().as_str().to_string(),
            locale: ui.get_locale().as_str().to_string(),
            timezone: ui.get_timezone().as_str().to_string(),
            packages: vec![],
            image_size_gb: 8,
            output_path: ui.get_output_path().as_str().to_string(),
        }
    }
    
    fn run_build(config: crate::builder::BuildConfig, ui_weak: slint::Weak<GameScopeWizard>) {
        let result = Self::execute_build(config, ui_weak.clone());
        
        if let Some(ui) = ui_weak.upgrade() {
            match result {
                Ok(_) => {
                    ui.set_build_in_progress(false);
                    ui.set_build_status("Build completed successfully!".into());
                    ui.set_build_progress(1.0);
                }
                Err(e) => {
                    ui.set_build_in_progress(false);
                    ui.set_build_status(std::format!("Build failed: {}", e).into());
                    ui.set_build_progress(0.0);
                }
            }
        }
    }
    
    fn execute_build(config: crate::builder::BuildConfig, ui_weak: slint::Weak<GameScopeWizard>) -> Result<(), BuilderError> {
        let builder = GameScopeBuilder::new(config)?;
        
        // Update progress during build
        if let Some(ui) = ui_weak.upgrade() {
            ui.set_build_status("Creating Orange-Pi directory structure...".into());
            ui.set_build_progress(0.1);
        }
        
        // This would be a simplified version - in practice you'd want to
        // add progress callbacks to the builder methods
        let result = builder.build();
        
        // Update progress incrementally
        let progress_steps = vec![
            (0.2, "Running debootstrap..."),
            (0.3, "Installing multiarch packages..."),
            (0.5, "Building kernel..."),
            (0.6, "Installing multimedia packages..."),
            (0.7, "Installing desktop environment..."),
            (0.8, "Building U-Boot..."),
            (0.9, "Configuring system..."),
            (1.0, "Finalizing build..."),
        ];
        
        for (progress, status) in progress_steps {
            if let Some(ui) = ui_weak.upgrade() {
                ui.set_build_status(status.into());
                ui.set_build_progress(progress);
            }
            // In a real implementation, you'd sync this with actual build progress
            thread::sleep(std::time::Duration::from_millis(500));
        }
        
        result
    }
}

pub fn run_gamescope_gui() -> Result<(), Box<dyn std::error::Error>> {
    let mut app = GameScopeGuiApp::new()?;
    app.run()
}