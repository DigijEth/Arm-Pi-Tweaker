use std::fmt;
use std::io;
use std::path::PathBuf;

pub mod recovery;
pub mod logging;

/// Custom error types for the Orange Pi Builder application
#[derive(Debug, Clone)]
pub enum BuilderError {
    // IO related errors
    IoError(String),
    FileNotFound(PathBuf),
    PermissionDenied(PathBuf),
    DiskFull(PathBuf),
    
    // Configuration errors
    ConfigLoadError(String),
    ConfigSaveError(String),
    InvalidConfig(String),
    
    // Build errors
    BuildFailed(String),
    DependencyMissing(String),
    CrossCompilationError(String),
    
    // Hardware interface errors
    SPIError(String),
    FlashError(String),
    DeviceNotFound(String),
    
    // Network errors
    NetworkError(String),
    DownloadFailed(String),
    UrlParseError(String),
    
    // Validation errors
    InvalidInput(String),
    ValidationError(String),
    
    // System errors
    SystemCommandFailed(String, i32),
    ProcessTimeout(String),
    
    // Custom errors
    Custom(String),
}

impl fmt::Display for BuilderError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            BuilderError::IoError(e) => write!(f, "IO error: {}", e),
            BuilderError::FileNotFound(path) => write!(f, "File not found: {}", path.display()),
            BuilderError::PermissionDenied(path) => write!(f, "Permission denied: {}", path.display()),
            BuilderError::DiskFull(path) => write!(f, "Disk full: {}", path.display()),
            
            BuilderError::ConfigLoadError(msg) => write!(f, "Configuration load error: {}", msg),
            BuilderError::ConfigSaveError(msg) => write!(f, "Configuration save error: {}", msg),
            BuilderError::InvalidConfig(msg) => write!(f, "Invalid configuration: {}", msg),
            
            BuilderError::BuildFailed(msg) => write!(f, "Build failed: {}", msg),
            BuilderError::DependencyMissing(dep) => write!(f, "Missing dependency: {}", dep),
            BuilderError::CrossCompilationError(msg) => write!(f, "Cross-compilation error: {}", msg),
            
            BuilderError::SPIError(msg) => write!(f, "SPI error: {}", msg),
            BuilderError::FlashError(msg) => write!(f, "Flash error: {}", msg),
            BuilderError::DeviceNotFound(device) => write!(f, "Device not found: {}", device),
            
            BuilderError::NetworkError(msg) => write!(f, "Network error: {}", msg),
            BuilderError::DownloadFailed(url) => write!(f, "Download failed: {}", url),
            BuilderError::UrlParseError(url) => write!(f, "Invalid URL: {}", url),
            
            BuilderError::InvalidInput(msg) => write!(f, "Invalid input: {}", msg),
            BuilderError::ValidationError(msg) => write!(f, "Validation error: {}", msg),
            
            BuilderError::SystemCommandFailed(cmd, code) => write!(f, "Command failed: {} (exit code: {})", cmd, code),
            BuilderError::ProcessTimeout(cmd) => write!(f, "Process timeout: {}", cmd),
            
            BuilderError::Custom(msg) => write!(f, "{}", msg),
        }
    }
}

impl std::error::Error for BuilderError {}

impl From<io::Error> for BuilderError {
    fn from(error: io::Error) -> Self {
        match error.kind() {
            io::ErrorKind::NotFound => BuilderError::FileNotFound(PathBuf::from("unknown")),
            io::ErrorKind::PermissionDenied => BuilderError::PermissionDenied(PathBuf::from("unknown")),
            _ => BuilderError::IoError(error.to_string()),
        }
    }
}

impl From<toml::de::Error> for BuilderError {
    fn from(error: toml::de::Error) -> Self {
        BuilderError::ConfigLoadError(error.to_string())
    }
}

impl From<toml::ser::Error> for BuilderError {
    fn from(error: toml::ser::Error) -> Self {
        BuilderError::ConfigSaveError(error.to_string())
    }
}

pub type Result<T> = std::result::Result<T, BuilderError>;

/// Error context for better error reporting
#[derive(Debug)]
pub struct ErrorContext {
    pub operation: String,
    pub component: String,
    pub suggestion: Option<String>,
}

impl ErrorContext {
    pub fn new(operation: &str, component: &str) -> Self {
        Self {
            operation: operation.to_string(),
            component: component.to_string(),
            suggestion: None,
        }
    }
    
    pub fn with_suggestion(mut self, suggestion: &str) -> Self {
        self.suggestion = Some(suggestion.to_string());
        self
    }
}

/// Enhanced error with context information
#[derive(Debug)]
pub struct ContextualError {
    pub error: BuilderError,
    pub context: ErrorContext,
}

impl fmt::Display for ContextualError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "[{}] {}: {}", self.context.component, self.context.operation, self.error)?;
        if let Some(suggestion) = &self.context.suggestion {
            write!(f, "\nSuggestion: {}", suggestion)?;
        }
        Ok(())
    }
}

impl std::error::Error for ContextualError {}

// Removed ErrorExt trait - no longer used

/// User-friendly error messages for the UI
pub fn user_friendly_error(error: &BuilderError) -> String {
    match error {
        BuilderError::FileNotFound(path) => {
            format!("The file '{}' was not found. Please check the path and try again.", path.display())
        }
        BuilderError::PermissionDenied(path) => {
            format!("Permission denied accessing '{}'. You may need to run as administrator or check file permissions.", path.display())
        }
        BuilderError::DiskFull(_) => {
            "Not enough disk space. Please free up space and try again.".to_string()
        }
        BuilderError::DependencyMissing(dep) => {
            format!("Required dependency '{}' is missing. Please install it and try again.", dep)
        }
        BuilderError::NetworkError(_) => {
            "Network connection failed. Please check your internet connection and try again.".to_string()
        }
        BuilderError::DownloadFailed(url) => {
            format!("Failed to download from '{}'. Please check the URL and your network connection.", url)
        }
        BuilderError::DeviceNotFound(device) => {
            format!("Device '{}' not found. Please check the connection and try again.", device)
        }
        BuilderError::SystemCommandFailed(cmd, code) => {
            format!("System command failed: '{}' (exit code: {}). Please check the command and try again.", cmd, code)
        }
        BuilderError::ProcessTimeout(cmd) => {
            format!("Process '{}' timed out. The operation may take longer than expected.", cmd)
        }
        _ => error.to_string(),
    }
}

/// Error recovery suggestions
pub fn get_recovery_suggestions(error: &BuilderError) -> Vec<String> {
    match error {
        BuilderError::FileNotFound(_) => vec![
            "Check if the file path is correct".to_string(),
            "Verify the file exists".to_string(),
            "Try using absolute path instead of relative".to_string(),
        ],
        BuilderError::PermissionDenied(_) => vec![
            "Run the application with administrator privileges".to_string(),
            "Check file/directory permissions".to_string(),
            "Ensure you have read/write access to the location".to_string(),
        ],
        BuilderError::DiskFull(_) => vec![
            "Free up disk space".to_string(),
            "Choose a different location with more space".to_string(),
            "Clean up temporary files".to_string(),
        ],
        BuilderError::DependencyMissing(_) => vec![
            "Install the missing dependency".to_string(),
            "Check the system requirements".to_string(),
            "Update your package manager".to_string(),
        ],
        BuilderError::NetworkError(_) => vec![
            "Check your internet connection".to_string(),
            "Try again later".to_string(),
            "Check firewall settings".to_string(),
        ],
        BuilderError::DeviceNotFound(_) => vec![
            "Check device connection".to_string(),
            "Verify device is powered on".to_string(),
            "Try a different USB port".to_string(),
            "Check device drivers".to_string(),
        ],
        _ => vec!["Try the operation again".to_string()],
    }
}

/// Macro for easy error creation with context
#[macro_export]
macro_rules! builder_error {
    ($kind:expr, $msg:expr) => {
        BuilderError::Custom(format!("{}: {}", stringify!($kind), $msg))
    };
    ($kind:expr, $fmt:expr, $($arg:tt)*) => {
        BuilderError::Custom(format!("{}: {}", stringify!($kind), format!($fmt, $($arg)*)))
    };
}