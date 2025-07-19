mod ui;
mod wizard;
mod flasher;
mod burner;
mod utils;
mod updates;
mod config;
mod error;
mod bootloader;
mod realtime_writer;
mod devicetree;
mod download;
mod builder;
mod gui;
mod armpi_tweaker;

use crate::error::Result;
use clap::Parser;
use cursive::{Cursive, CursiveExt};
use log::{info, warn};
use std::process::{Command, Stdio};

#[derive(Parser, Debug)]
#[command(
    name = "builder",
    about = "Setec Labs Orange Pi 5 Plus Builder",
    long_about = "Cross-platform tool for building custom distros, flashing SPI, and burning images for Orange Pi 5 Plus"
)]
struct Args {
    #[arg(short, long, help = "Enable verbose logging")]
    verbose: bool,
    
    #[arg(long, help = "Launch GUI interface for GameScope wizard")]
    gui: bool,
}

fn main() -> Result<()> {
    let args = Args::parse();
    
    // Initialize standard logger
    let log_level = if args.verbose { "debug" } else { "info" };
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or(log_level))
        .init();
    
    // Initialize custom UI logger
    ui::logger::init_global_logger();
    ui::logger::log_info("Application starting...");
    
    // Initialize error logging system
    let error_log_config = crate::error::logging::ErrorLoggerConfig {
        log_level: if args.verbose { 
            crate::error::logging::LogLevel::Debug 
        } else { 
            crate::error::logging::LogLevel::Info 
        },
        log_to_console: args.verbose,
        ..Default::default()
    };
    
    crate::error::logging::init_global_logger(error_log_config)?;
    
    info!("Starting Setec Labs Orange Pi 5 Plus Builder...");
    
    // Initialize download directory structure
    if let Err(e) = crate::download::init_download_system() {
        ui::logger::log_error(&format!("Failed to initialize download system: {}", e));
        warn!("Download system initialization failed: {}", e);
    } else {
        info!("Download directory structure initialized");
    }
    
    // Check if GUI mode is requested
    if args.gui {
        info!("Starting GUI interface for GameScope wizard...");
        if let Err(e) = gui::run_gamescope_gui() {
            eprintln!("GUI failed to start: {}", e);
            std::process::exit(1);
        }
        return Ok(());
    }
    
    // Create the cursive instance for CLI interface
    let mut siv = Cursive::default();
    
    // Set up comprehensive event logging using available callbacks
    siv.add_global_callback('*', |_s| {
        ui::logger::log_info("Any key pressed");
    });
    
    // Add global event handlers for logging common events
    siv.add_global_callback(cursive::event::Key::F10, |_s| {
        ui::logger::log_info("F10 key pressed");
    });
    
    // Set up the UI
    ui::setup_main_menu(&mut siv);
    
    // Run the application
    siv.run();
    
    ui::logger::log_info("Application shutting down...");
    
    Ok(())
}

// Utility function to execute commands with logging
pub fn execute_command_with_logging(command: &str, args: &[&str]) -> std::process::Output {
    ui::logger::log_process(command, args);
    
    let output = Command::new(command)
        .args(args)
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .output()
        .unwrap_or_else(|e| {
            ui::logger::log_error(&format!("Failed to execute command '{}': {}", command, e));
            
            // Create a simple error exit status
            let dummy_status = Command::new("echo").arg("error").status().unwrap();
            
            std::process::Output {
                status: dummy_status,
                stdout: Vec::new(),
                stderr: format!("Failed to execute: {}", e).into_bytes(),
            }
        });
    
    let stdout = String::from_utf8_lossy(&output.stdout);
    let stderr = String::from_utf8_lossy(&output.stderr);
    let exit_code = output.status.code();
    
    ui::logger::log_process_output(command, &stdout, &stderr, exit_code);
    
    output
}
