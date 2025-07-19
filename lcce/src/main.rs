use anyhow::Result;
use log::{info, warn, error};
use std::sync::{Arc, Mutex};
use tokio::time::{sleep, Duration};

slint::include_modules!();

mod modules;
mod system;
mod config;

use modules::*;
use system::SystemInfo;
use config::LcceConfig;

#[derive(Clone)]
pub struct AppState {
    pub system_info: Arc<Mutex<SystemInfo>>,
    pub config: Arc<Mutex<LcceConfig>>,
    pub installation_active: Arc<Mutex<bool>>,
}

impl AppState {
    pub fn new() -> Result<Self> {
        let system_info = SystemInfo::detect()?;
        let config = LcceConfig::default();
        
        Ok(Self {
            system_info: Arc::new(Mutex::new(system_info)),
            config: Arc::new(Mutex::new(config)),
            installation_active: Arc::new(Mutex::new(false)),
        })
    }
}

#[tokio::main]
async fn main() -> Result<()> {
    env_logger::init();
    info!("Starting Arm-Pi Tweaker: Live Custom Creation Edition");
    
    // Initialize application state
    let app_state = AppState::new()?;
    
    // Create the main window
    let ui = MainWindow::new()?;
    
    // Initialize UI with system information
    {
        let system_info = app_state.system_info.lock().unwrap();
        ui.set_device_name(system_info.device_name.clone().into());
        ui.set_current_status("Ready - LCCE initialized successfully".into());
    }
    
    // Set up callbacks for each module
    setup_callbacks(&ui, app_state.clone())?;
    
    info!("LCCE GUI initialized, showing main window");
    ui.run()?;
    
    Ok(())
}

fn setup_callbacks(ui: &MainWindow, app_state: AppState) -> Result<()> {
    // Kernel modification module
    {
        let state = app_state.clone();
        let ui_weak = ui.as_weak();
        ui.on_show_kernel_module(move || {
            info!("Opening Kernel Modification module");
            if let Some(ui) = ui_weak.upgrade() {
                ui.set_current_status("Opening Kernel Modification module...".into());
                
                // Launch kernel module in a separate task
                let state_clone = state.clone();
                tokio::spawn(async move {
                    if let Err(e) = kernel::show_kernel_module(state_clone).await {
                        error!("Kernel module error: {}", e);
                    }
                });
            }
        });
    }
    
    // Video driver module
    {
        let state = app_state.clone();
        let ui_weak = ui.as_weak();
        ui.on_show_video_driver_module(move || {
            info!("Opening Video Driver module");
            if let Some(ui) = ui_weak.upgrade() {
                ui.set_current_status("Opening Video Driver module...".into());
                
                let state_clone = state.clone();
                tokio::spawn(async move {
                    if let Err(e) = video_driver::show_video_driver_module(state_clone).await {
                        error!("Video driver module error: {}", e);
                    }
                });
            }
        });
    }
    
    // Emulation/Multimedia module
    {
        let state = app_state.clone();
        let ui_weak = ui.as_weak();
        ui.on_show_emulation_module(move || {
            info!("Opening Emulation & Multimedia module");
            if let Some(ui) = ui_weak.upgrade() {
                ui.set_current_status("Opening Emulation & Multimedia module...".into());
                
                let state_clone = state.clone();
                tokio::spawn(async move {
                    if let Err(e) = emulation::show_emulation_module(state_clone).await {
                        error!("Emulation module error: {}", e);
                    }
                });
            }
        });
    }
    
    // Storage installation module
    {
        let state = app_state.clone();
        let ui_weak = ui.as_weak();
        ui.on_show_storage_module(move || {
            info!("Opening Storage Installation module");
            if let Some(ui) = ui_weak.upgrade() {
                ui.set_current_status("Opening Storage Installation module...".into());
                
                let state_clone = state.clone();
                tokio::spawn(async move {
                    if let Err(e) = storage::show_storage_module(state_clone).await {
                        error!("Storage module error: {}", e);
                    }
                });
            }
        });
    }
    
    // Arm-Pi Tweaker integration
    {
        let state = app_state.clone();
        let ui_weak = ui.as_weak();
        ui.on_show_armpi_tweaker_module(move || {
            info!("Opening Arm-Pi Tweaker module");
            if let Some(ui) = ui_weak.upgrade() {
                ui.set_current_status("Opening Arm-Pi Tweaker suite...".into());
                
                let state_clone = state.clone();
                tokio::spawn(async move {
                    if let Err(e) = armpi_tweaker::show_armpi_tweaker_module(state_clone).await {
                        error!("Arm-Pi Tweaker module error: {}", e);
                    }
                });
            }
        });
    }
    
    // About dialog
    {
        let ui_weak = ui.as_weak();
        ui.on_show_about_dialog(move || {
            info!("Showing about dialog");
            if let Some(_ui) = ui_weak.upgrade() {
                // TODO: Implement about dialog
                println!("Arm-Pi Tweaker: Live Custom Creation Edition v0.1.0");
                println!("Create custom Orange Pi 5 Plus installations with live modifications");
            }
        });
    }
    
    // Quit application
    {
        let ui_weak = ui.as_weak();
        ui.on_quit_application(move || {
            info!("User requested application quit");
            if let Some(ui) = ui_weak.upgrade() {
                ui.hide().unwrap();
            }
            std::process::exit(0);
        });
    }
    
    Ok(())
}