use std::path::PathBuf;
use std::fs;
use directories::ProjectDirs;
use crate::error::{BuilderError, Result};

pub fn get_config_dir() -> Result<PathBuf> {
    ProjectDirs::from("com", "seteclabs", "orangepi-builder")
        .map(|dirs| dirs.config_dir().to_path_buf())
        .ok_or_else(|| BuilderError::Custom("Failed to determine config directory".to_string()))
}

pub fn get_data_dir() -> Result<PathBuf> {
    ProjectDirs::from("com", "seteclabs", "orangepi-builder")
        .map(|dirs| dirs.data_dir().to_path_buf())
        .ok_or_else(|| BuilderError::Custom("Failed to determine data directory".to_string()))
}

pub fn ensure_dir_exists(path: &PathBuf) -> Result<()> {
    if !path.exists() {
        fs::create_dir_all(path)
            .map_err(|e| BuilderError::from(e))?;
    }
    Ok(())
}

pub fn validate_file_exists(path: &PathBuf) -> Result<()> {
    if !path.exists() {
        return Err(BuilderError::FileNotFound(path.clone()));
    }
    if !path.is_file() {
        return Err(BuilderError::Custom(format!("Path is not a file: {}", path.display())));
    }
    Ok(())
}

pub fn validate_dir_exists(path: &PathBuf) -> Result<()> {
    if !path.exists() {
        return Err(BuilderError::FileNotFound(path.clone()));
    }
    if !path.is_dir() {
        return Err(BuilderError::Custom(format!("Path is not a directory: {}", path.display())));
    }
    Ok(())
}

pub fn check_disk_space(path: &PathBuf, required_bytes: u64) -> Result<()> {
    // This is a simplified implementation
    // In a real-world scenario, you'd use platform-specific APIs
    match fs::metadata(path) {
        Ok(_) => Ok(()), // Simplified - assume space is available
        Err(_) => Err(BuilderError::DiskFull(path.clone()))
    }
}

pub fn format_size(bytes: u64) -> String {
    const UNITS: &[&str] = &["B", "KB", "MB", "GB", "TB"];
    let mut size = bytes as f64;
    let mut unit_index = 0;
    
    while size >= 1024.0 && unit_index < UNITS.len() - 1 {
        size /= 1024.0;
        unit_index += 1;
    }
    
    if unit_index == 0 {
        format!("{} {}", size as u64, UNITS[unit_index])
    } else {
        format!("{:.2} {}", size, UNITS[unit_index])
    }
}