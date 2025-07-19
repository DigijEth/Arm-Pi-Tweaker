use std::fs::{File, OpenOptions};
use std::io::{BufWriter, Write};
use std::path::PathBuf;
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};
use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use crate::error::{BuilderError, Result, ContextualError};
use crate::utils;

/// Log level for error messages
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum LogLevel {
    Error,
    Warning,
    Info,
    Debug,
    Trace,
}

impl std::fmt::Display for LogLevel {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            LogLevel::Error => write!(f, "ERROR"),
            LogLevel::Warning => write!(f, "WARN"),
            LogLevel::Info => write!(f, "INFO"),
            LogLevel::Debug => write!(f, "DEBUG"),
            LogLevel::Trace => write!(f, "TRACE"),
        }
    }
}

/// Error log entry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ErrorLogEntry {
    pub timestamp: DateTime<Utc>,
    pub level: LogLevel,
    pub component: String,
    pub operation: String,
    pub error_type: String,
    pub message: String,
    pub context: Option<String>,
    pub suggestion: Option<String>,
    pub stack_trace: Option<String>,
    pub user_action: Option<String>,
    pub recovery_attempted: bool,
    pub recovery_successful: bool,
    pub session_id: String,
}

impl ErrorLogEntry {
    pub fn new(
        level: LogLevel,
        component: &str,
        operation: &str,
        error: &BuilderError,
        session_id: &str,
    ) -> Self {
        let error_type = match error {
            BuilderError::IoError(_) => "IoError",
            BuilderError::FileNotFound(_) => "FileNotFound",
            BuilderError::PermissionDenied(_) => "PermissionDenied",
            BuilderError::DiskFull(_) => "DiskFull",
            BuilderError::ConfigLoadError(_) => "ConfigLoadError",
            BuilderError::ConfigSaveError(_) => "ConfigSaveError",
            BuilderError::InvalidConfig(_) => "InvalidConfig",
            BuilderError::BuildFailed(_) => "BuildFailed",
            BuilderError::DependencyMissing(_) => "DependencyMissing",
            BuilderError::CrossCompilationError(_) => "CrossCompilationError",
            BuilderError::SPIError(_) => "SPIError",
            BuilderError::FlashError(_) => "FlashError",
            BuilderError::DeviceNotFound(_) => "DeviceNotFound",
            BuilderError::NetworkError(_) => "NetworkError",
            BuilderError::DownloadFailed(_) => "DownloadFailed",
            BuilderError::UrlParseError(_) => "UrlParseError",
            BuilderError::InvalidInput(_) => "InvalidInput",
            BuilderError::ValidationError(_) => "ValidationError",
            BuilderError::SystemCommandFailed(_, _) => "SystemCommandFailed",
            BuilderError::ProcessTimeout(_) => "ProcessTimeout",
            BuilderError::Custom(_) => "Custom",
        };

        Self {
            timestamp: Utc::now(),
            level,
            component: component.to_string(),
            operation: operation.to_string(),
            error_type: error_type.to_string(),
            message: error.to_string(),
            context: None,
            suggestion: None,
            stack_trace: None,
            user_action: None,
            recovery_attempted: false,
            recovery_successful: false,
            session_id: session_id.to_string(),
        }
    }

    pub fn from_contextual_error(
        level: LogLevel,
        error: &ContextualError,
        session_id: &str,
    ) -> Self {
        let mut entry = Self::new(level, &error.context.component, &error.context.operation, &error.error, session_id);
        entry.context = Some(format!("{}: {}", error.context.component, error.context.operation));
        entry.suggestion = error.context.suggestion.clone();
        entry
    }

    pub fn with_context(mut self, context: &str) -> Self {
        self.context = Some(context.to_string());
        self
    }

    pub fn with_suggestion(mut self, suggestion: &str) -> Self {
        self.suggestion = Some(suggestion.to_string());
        self
    }

    pub fn with_stack_trace(mut self, stack_trace: &str) -> Self {
        self.stack_trace = Some(stack_trace.to_string());
        self
    }

    pub fn with_user_action(mut self, user_action: &str) -> Self {
        self.user_action = Some(user_action.to_string());
        self
    }

    pub fn with_recovery_attempt(mut self, attempted: bool, successful: bool) -> Self {
        self.recovery_attempted = attempted;
        self.recovery_successful = successful;
        self
    }

    pub fn to_formatted_string(&self) -> String {
        let mut output = format!(
            "[{}] [{}] [{}:{}] {}",
            self.timestamp.format("%Y-%m-%d %H:%M:%S UTC"),
            self.level,
            self.component,
            self.operation,
            self.message
        );

        if let Some(context) = &self.context {
            output.push_str(&format!("\n  Context: {}", context));
        }

        if let Some(suggestion) = &self.suggestion {
            output.push_str(&format!("\n  Suggestion: {}", suggestion));
        }

        if let Some(user_action) = &self.user_action {
            output.push_str(&format!("\n  User Action: {}", user_action));
        }

        if self.recovery_attempted {
            output.push_str(&format!("\n  Recovery: {} ({})", 
                if self.recovery_attempted { "Attempted" } else { "Not Attempted" },
                if self.recovery_successful { "Successful" } else { "Failed" }
            ));
        }

        if let Some(stack_trace) = &self.stack_trace {
            output.push_str(&format!("\n  Stack Trace:\n{}", stack_trace));
        }

        output.push_str(&format!("\n  Session: {}", self.session_id));
        output.push_str(&format!("\n  Error Type: {}", self.error_type));

        output
    }
}

/// Error logger configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ErrorLoggerConfig {
    pub log_level: LogLevel,
    pub log_to_file: bool,
    pub log_to_console: bool,
    pub log_file_path: Option<PathBuf>,
    pub max_log_file_size: u64,
    pub max_log_files: u32,
    pub include_stack_trace: bool,
    pub include_user_actions: bool,
    pub session_id: String,
}

impl Default for ErrorLoggerConfig {
    fn default() -> Self {
        Self {
            log_level: LogLevel::Info,
            log_to_file: true,
            log_to_console: true,
            log_file_path: None,
            max_log_file_size: 10 * 1024 * 1024, // 10MB
            max_log_files: 5,
            include_stack_trace: true,
            include_user_actions: true,
            session_id: Self::generate_session_id(),
        }
    }
}

impl ErrorLoggerConfig {
    fn generate_session_id() -> String {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap_or_default()
            .as_secs();
        format!("session_{}", timestamp)
    }

    pub fn get_log_file_path(&self) -> Result<PathBuf> {
        match &self.log_file_path {
            Some(path) => Ok(path.clone()),
            None => {
                let data_dir = utils::get_data_dir()?;
                let log_dir = data_dir.join("logs");
                utils::ensure_dir_exists(&log_dir)?;
                Ok(log_dir.join("error.log"))
            }
        }
    }
}

/// Error logger implementation
pub struct ErrorLogger {
    config: ErrorLoggerConfig,
    file_writer: Option<Arc<Mutex<BufWriter<File>>>>,
}

impl ErrorLogger {
    pub fn new(config: ErrorLoggerConfig) -> Result<Self> {
        let file_writer = if config.log_to_file {
            let log_file_path = config.get_log_file_path()?;
            let file = OpenOptions::new()
                .create(true)
                .append(true)
                .open(&log_file_path)
                .map_err(BuilderError::from)?;
            
            Some(Arc::new(Mutex::new(BufWriter::new(file))))
        } else {
            None
        };

        Ok(Self {
            config,
            file_writer,
        })
    }

    pub fn log_error(&self, error: &BuilderError, component: &str, operation: &str) -> Result<()> {
        let entry = ErrorLogEntry::new(LogLevel::Error, component, operation, error, &self.config.session_id);
        self.log_entry(&entry)
    }

    pub fn log_contextual_error(&self, error: &ContextualError) -> Result<()> {
        let entry = ErrorLogEntry::from_contextual_error(LogLevel::Error, error, &self.config.session_id);
        self.log_entry(&entry)
    }

    pub fn log_warning(&self, message: &str, component: &str, operation: &str) -> Result<()> {
        let error = BuilderError::Custom(message.to_string());
        let entry = ErrorLogEntry::new(LogLevel::Warning, component, operation, &error, &self.config.session_id);
        self.log_entry(&entry)
    }

    pub fn log_info(&self, message: &str, component: &str, operation: &str) -> Result<()> {
        let error = BuilderError::Custom(message.to_string());
        let entry = ErrorLogEntry::new(LogLevel::Info, component, operation, &error, &self.config.session_id);
        self.log_entry(&entry)
    }

    pub fn log_debug(&self, message: &str, component: &str, operation: &str) -> Result<()> {
        if self.config.log_level as u8 <= LogLevel::Debug as u8 {
            let error = BuilderError::Custom(message.to_string());
            let entry = ErrorLogEntry::new(LogLevel::Debug, component, operation, &error, &self.config.session_id);
            self.log_entry(&entry)
        } else {
            Ok(())
        }
    }

    pub fn log_trace(&self, message: &str, component: &str, operation: &str) -> Result<()> {
        if self.config.log_level as u8 <= LogLevel::Trace as u8 {
            let error = BuilderError::Custom(message.to_string());
            let entry = ErrorLogEntry::new(LogLevel::Trace, component, operation, &error, &self.config.session_id);
            self.log_entry(&entry)
        } else {
            Ok(())
        }
    }

    pub fn log_entry(&self, entry: &ErrorLogEntry) -> Result<()> {
        let formatted = entry.to_formatted_string();

        // Log to console if enabled
        if self.config.log_to_console {
            match entry.level {
                LogLevel::Error => eprintln!("{}", formatted),
                LogLevel::Warning => eprintln!("{}", formatted),
                _ => println!("{}", formatted),
            }
        }

        // Log to file if enabled
        if let Some(ref file_writer) = self.file_writer {
            let mut writer = file_writer.lock().map_err(|_| {
                BuilderError::Custom("Failed to acquire file writer lock".to_string())
            })?;

            writeln!(writer, "{}", formatted)
                .map_err(BuilderError::from)?;

            writer.flush()
                .map_err(BuilderError::from)?;
        }

        Ok(())
    }

    pub fn log_recovery_attempt(&self, error: &BuilderError, component: &str, operation: &str, successful: bool) -> Result<()> {
        let entry = ErrorLogEntry::new(LogLevel::Info, component, operation, error, &self.config.session_id)
            .with_recovery_attempt(true, successful)
            .with_context(&format!("Recovery attempt for {}", operation));
        
        self.log_entry(&entry)
    }

    pub fn log_user_action(&self, action: &str, component: &str, operation: &str) -> Result<()> {
        let error = BuilderError::Custom(format!("User action: {}", action));
        let entry = ErrorLogEntry::new(LogLevel::Info, component, operation, &error, &self.config.session_id)
            .with_user_action(action);
        
        self.log_entry(&entry)
    }

    pub fn rotate_logs(&self) -> Result<()> {
        let log_file_path = self.config.get_log_file_path()?;
        
        // Check if log rotation is needed
        if let Ok(metadata) = std::fs::metadata(&log_file_path) {
            if metadata.len() > self.config.max_log_file_size {
                // Rotate log files
                for i in (1..self.config.max_log_files).rev() {
                    let old_path = log_file_path.with_extension(format!("log.{}", i));
                    let new_path = log_file_path.with_extension(format!("log.{}", i + 1));
                    
                    if old_path.exists() {
                        std::fs::rename(&old_path, &new_path)
                            .map_err(BuilderError::from)?;
                    }
                }
                
                // Move current log to .1
                let backup_path = log_file_path.with_extension("log.1");
                std::fs::rename(&log_file_path, &backup_path)
                    .map_err(BuilderError::from)?;
            }
        }
        
        Ok(())
    }

    pub fn get_recent_errors(&self, limit: usize) -> Result<Vec<ErrorLogEntry>> {
        let log_file_path = self.config.get_log_file_path()?;
        let content = std::fs::read_to_string(&log_file_path)
            .map_err(BuilderError::from)?;

        let mut entries = Vec::new();
        for line in content.lines().rev().take(limit) {
            // This is a simplified parser - in practice, you'd want a more robust approach
            if line.starts_with('[') && line.contains("ERROR") {
                // Parse the log entry - this is simplified
                let parts: Vec<&str> = line.split("] [").collect();
                if parts.len() >= 4 {
                    let timestamp_str = parts[0].trim_start_matches('[');
                    let level_str = parts[1];
                    let component_operation = parts[2];
                    let message = parts[3..].join("] [");
                    let message = message.trim_end_matches(']');

                    if let Ok(timestamp) = DateTime::parse_from_str(timestamp_str, "%Y-%m-%d %H:%M:%S UTC") {
                        let level = match level_str {
                            "ERROR" => LogLevel::Error,
                            "WARN" => LogLevel::Warning,
                            "INFO" => LogLevel::Info,
                            "DEBUG" => LogLevel::Debug,
                            "TRACE" => LogLevel::Trace,
                            _ => LogLevel::Info,
                        };

                        let parts: Vec<&str> = component_operation.split(':').collect();
                        let component = parts.get(0).unwrap_or(&"Unknown").to_string();
                        let operation = parts.get(1).unwrap_or(&"Unknown").to_string();

                        let entry = ErrorLogEntry {
                            timestamp: timestamp.with_timezone(&Utc),
                            level,
                            component,
                            operation,
                            error_type: "Unknown".to_string(),
                            message: message.to_string(),
                            context: None,
                            suggestion: None,
                            stack_trace: None,
                            user_action: None,
                            recovery_attempted: false,
                            recovery_successful: false,
                            session_id: self.config.session_id.clone(),
                        };

                        entries.push(entry);
                    }
                }
            }
        }

        entries.reverse();
        Ok(entries)
    }

    pub fn clear_logs(&self) -> Result<()> {
        let log_file_path = self.config.get_log_file_path()?;
        std::fs::remove_file(&log_file_path)
            .map_err(BuilderError::from)?;
        Ok(())
    }
}

/// Global error logger instance
static mut GLOBAL_LOGGER: Option<ErrorLogger> = None;
static LOGGER_INIT: std::sync::Once = std::sync::Once::new();

/// Initialize the global error logger
pub fn init_global_logger(config: ErrorLoggerConfig) -> Result<()> {
    LOGGER_INIT.call_once(|| {
        unsafe {
            if let Ok(logger) = ErrorLogger::new(config) {
                GLOBAL_LOGGER = Some(logger);
            }
        }
    });
    Ok(())
}

/// Get the global error logger
pub fn get_global_logger() -> Option<&'static ErrorLogger> {
    unsafe { GLOBAL_LOGGER.as_ref() }
}

/// Convenience macros for logging
#[macro_export]
macro_rules! log_error {
    ($error:expr, $component:expr, $operation:expr) => {
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_error(&$error, $component, $operation);
        }
    };
}

#[macro_export]
macro_rules! log_warning {
    ($message:expr, $component:expr, $operation:expr) => {
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_warning($message, $component, $operation);
        }
    };
}

#[macro_export]
macro_rules! log_info {
    ($message:expr, $component:expr, $operation:expr) => {
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_info($message, $component, $operation);
        }
    };
}

#[macro_export]
macro_rules! log_debug {
    ($message:expr, $component:expr, $operation:expr) => {
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_debug($message, $component, $operation);
        }
    };
}

#[macro_export]
macro_rules! log_trace {
    ($message:expr, $component:expr, $operation:expr) => {
        if let Some(logger) = crate::error::logging::get_global_logger() {
            let _ = logger.log_trace($message, $component, $operation);
        }
    };
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn test_error_log_entry_creation() {
        let error = BuilderError::FileNotFound(std::path::PathBuf::from("test.txt"));
        let entry = ErrorLogEntry::new(LogLevel::Error, "TestComponent", "test_operation", &error, "test_session");
        
        assert_eq!(entry.level, LogLevel::Error);
        assert_eq!(entry.component, "TestComponent");
        assert_eq!(entry.operation, "test_operation");
        assert_eq!(entry.error_type, "FileNotFound");
        assert_eq!(entry.session_id, "test_session");
    }

    #[test]
    fn test_error_log_entry_formatting() {
        let error = BuilderError::Custom("Test error message".to_string());
        let entry = ErrorLogEntry::new(LogLevel::Error, "TestComponent", "test_operation", &error, "test_session")
            .with_context("Test context")
            .with_suggestion("Test suggestion");
        
        let formatted = entry.to_formatted_string();
        assert!(formatted.contains("Test error message"));
        assert!(formatted.contains("Test context"));
        assert!(formatted.contains("Test suggestion"));
        assert!(formatted.contains("test_session"));
    }

    #[test]
    fn test_error_logger_creation() {
        let temp_dir = tempdir().unwrap();
        let config = ErrorLoggerConfig {
            log_file_path: Some(temp_dir.path().join("test.log")),
            ..Default::default()
        };
        
        let logger = ErrorLogger::new(config);
        assert!(logger.is_ok());
    }

    #[test]
    fn test_error_logger_logging() {
        let temp_dir = tempdir().unwrap();
        let config = ErrorLoggerConfig {
            log_file_path: Some(temp_dir.path().join("test.log")),
            log_to_console: false,
            ..Default::default()
        };
        
        let logger = ErrorLogger::new(config).unwrap();
        let error = BuilderError::Custom("Test error".to_string());
        
        let result = logger.log_error(&error, "TestComponent", "test_operation");
        assert!(result.is_ok());
        
        // Check if log file was created and contains the error
        let log_content = std::fs::read_to_string(temp_dir.path().join("test.log")).unwrap();
        assert!(log_content.contains("Test error"));
        assert!(log_content.contains("TestComponent"));
    }

    #[test]
    fn test_log_level_filtering() {
        let temp_dir = tempdir().unwrap();
        let config = ErrorLoggerConfig {
            log_file_path: Some(temp_dir.path().join("test.log")),
            log_to_console: false,
            log_level: LogLevel::Warning,
            ..Default::default()
        };
        
        let logger = ErrorLogger::new(config).unwrap();
        
        // Debug message should not be logged
        let result = logger.log_debug("Debug message", "TestComponent", "test_operation");
        assert!(result.is_ok());
        
        // Warning message should be logged
        let result = logger.log_warning("Warning message", "TestComponent", "test_operation");
        assert!(result.is_ok());
        
        let log_content = std::fs::read_to_string(temp_dir.path().join("test.log")).unwrap();
        assert!(!log_content.contains("Debug message"));
        assert!(log_content.contains("Warning message"));
    }
}