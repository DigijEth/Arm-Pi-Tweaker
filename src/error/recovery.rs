use std::time::Duration;
use std::thread;
use crate::error::{BuilderError, Result};

/// Retry configuration for operations that might fail
#[derive(Debug, Clone)]
pub struct RetryConfig {
    pub max_attempts: u32,
    pub base_delay: Duration,
    pub max_delay: Duration,
    pub backoff_multiplier: f64,
    pub jitter: bool,
}

impl Default for RetryConfig {
    fn default() -> Self {
        Self {
            max_attempts: 3,
            base_delay: Duration::from_millis(100),
            max_delay: Duration::from_secs(60),
            backoff_multiplier: 2.0,
            jitter: true,
        }
    }
}

impl RetryConfig {
    pub fn new(max_attempts: u32) -> Self {
        Self {
            max_attempts,
            ..Default::default()
        }
    }
    
    pub fn with_delay(mut self, base_delay: Duration) -> Self {
        self.base_delay = base_delay;
        self
    }
    
    pub fn with_max_delay(mut self, max_delay: Duration) -> Self {
        self.max_delay = max_delay;
        self
    }
    
    pub fn with_backoff_multiplier(mut self, multiplier: f64) -> Self {
        self.backoff_multiplier = multiplier;
        self
    }
    
    pub fn with_jitter(mut self, jitter: bool) -> Self {
        self.jitter = jitter;
        self
    }
}

/// Recovery strategies for different types of errors
#[derive(Debug)]
pub enum RecoveryStrategy {
    /// Retry the operation with exponential backoff
    Retry(RetryConfig),
    /// Ask user for input/confirmation
    UserInput,
    /// Ignore the error and continue
    Ignore,
    /// Fail immediately
    Fail,
}

/// Recovery context for tracking recovery attempts
#[derive(Debug)]
pub struct RecoveryContext {
    pub operation: String,
    pub component: String,
    pub attempt: u32,
    pub max_attempts: u32,
    pub last_error: Option<BuilderError>,
}

impl RecoveryContext {
    pub fn new(operation: &str, component: &str, max_attempts: u32) -> Self {
        Self {
            operation: operation.to_string(),
            component: component.to_string(),
            attempt: 0,
            max_attempts,
            last_error: None,
        }
    }
    
    pub fn increment_attempt(&mut self) {
        self.attempt += 1;
    }
    
    pub fn set_last_error(&mut self, error: BuilderError) {
        self.last_error = Some(error);
    }
    
    pub fn has_attempts_left(&self) -> bool {
        self.attempt < self.max_attempts
    }
    
    pub fn get_progress(&self) -> f64 {
        if self.max_attempts == 0 {
            return 1.0;
        }
        self.attempt as f64 / self.max_attempts as f64
    }
}

/// Trait for operations that can be retried
pub trait Retryable<T> {
    fn retry_with_config(self, config: RetryConfig) -> Result<T>;
    fn retry_with_context(self, context: &mut RecoveryContext) -> Result<T>;
}

impl<T, F> Retryable<T> for F
where
    F: Fn() -> Result<T>,
{
    fn retry_with_config(self, config: RetryConfig) -> Result<T> {
        let mut attempt = 0;
        let mut delay = config.base_delay;
        
        loop {
            attempt += 1;
            
            match self() {
                Ok(result) => return Ok(result),
                Err(error) => {
                    if attempt >= config.max_attempts {
                        return Err(error);
                    }
                    
                    // Apply exponential backoff
                    if attempt > 1 {
                        let mut actual_delay = delay;
                        
                        // Add jitter if enabled
                        if config.jitter {
                            let jitter_factor = 1.0 + (fastrand::f64() - 0.5) * 0.1;
                            actual_delay = Duration::from_millis(
                                (actual_delay.as_millis() as f64 * jitter_factor) as u64
                            );
                        }
                        
                        thread::sleep(actual_delay);
                        
                        // Increase delay for next attempt
                        delay = std::cmp::min(
                            Duration::from_millis(
                                (delay.as_millis() as f64 * config.backoff_multiplier) as u64
                            ),
                            config.max_delay
                        );
                    }
                }
            }
        }
    }
    
    fn retry_with_context(self, context: &mut RecoveryContext) -> Result<T> {
        loop {
            context.increment_attempt();
            
            match self() {
                Ok(result) => return Ok(result),
                Err(error) => {
                    context.set_last_error(error.clone());
                    
                    if !context.has_attempts_left() {
                        return Err(error);
                    }
                    
                    // Simple delay between attempts
                    thread::sleep(Duration::from_millis(100 * context.attempt as u64));
                }
            }
        }
    }
}

/// Recovery manager for handling different types of errors
pub struct RecoveryManager {
    strategies: std::collections::HashMap<String, RecoveryStrategy>,
}

impl RecoveryManager {
    pub fn new() -> Self {
        let mut manager = Self {
            strategies: std::collections::HashMap::new(),
        };
        
        // Set up default recovery strategies
        manager.set_default_strategies();
        manager
    }
    
    fn set_default_strategies(&mut self) {
        // Network errors - retry with exponential backoff
        self.strategies.insert(
            "NetworkError".to_string(),
            RecoveryStrategy::Retry(RetryConfig::new(3).with_delay(Duration::from_secs(1)))
        );
        
        // File not found - ask user for input
        self.strategies.insert(
            "FileNotFound".to_string(),
            RecoveryStrategy::UserInput
        );
        
        // Permission denied - ask user for input
        self.strategies.insert(
            "PermissionDenied".to_string(),
            RecoveryStrategy::UserInput
        );
        
        // Disk full - ask user for input
        self.strategies.insert(
            "DiskFull".to_string(),
            RecoveryStrategy::UserInput
        );
        
        // Device not found - retry with longer delays
        self.strategies.insert(
            "DeviceNotFound".to_string(),
            RecoveryStrategy::Retry(RetryConfig::new(5).with_delay(Duration::from_secs(2)))
        );
        
        // System command failed - retry once
        self.strategies.insert(
            "SystemCommandFailed".to_string(),
            RecoveryStrategy::Retry(RetryConfig::new(2).with_delay(Duration::from_millis(500)))
        );
        
        // Process timeout - retry with longer timeout
        self.strategies.insert(
            "ProcessTimeout".to_string(),
            RecoveryStrategy::Retry(RetryConfig::new(2).with_delay(Duration::from_secs(1)))
        );
    }
    
    pub fn set_strategy(&mut self, error_type: &str, strategy: RecoveryStrategy) {
        self.strategies.insert(error_type.to_string(), strategy);
    }
    
    pub fn get_strategy(&self, error: &BuilderError) -> Option<&RecoveryStrategy> {
        let error_type = match error {
            BuilderError::NetworkError(_) => "NetworkError",
            BuilderError::FileNotFound(_) => "FileNotFound",
            BuilderError::PermissionDenied(_) => "PermissionDenied",
            BuilderError::DiskFull(_) => "DiskFull",
            BuilderError::DeviceNotFound(_) => "DeviceNotFound",
            BuilderError::SystemCommandFailed(_, _) => "SystemCommandFailed",
            BuilderError::ProcessTimeout(_) => "ProcessTimeout",
            BuilderError::DownloadFailed(_) => "NetworkError",
            BuilderError::DependencyMissing(_) => "UserInput",
            _ => return None,
        };
        
        self.strategies.get(error_type)
    }
    
    pub fn should_retry(&self, error: &BuilderError) -> bool {
        matches!(self.get_strategy(error), Some(RecoveryStrategy::Retry(_)))
    }
    
    pub fn should_ask_user(&self, error: &BuilderError) -> bool {
        matches!(self.get_strategy(error), Some(RecoveryStrategy::UserInput))
    }
    
    pub fn get_retry_config(&self, error: &BuilderError) -> Option<&RetryConfig> {
        match self.get_strategy(error) {
            Some(RecoveryStrategy::Retry(config)) => Some(config),
            _ => None,
        }
    }
}

impl Default for RecoveryManager {
    fn default() -> Self {
        Self::new()
    }
}

/// Automatic recovery wrapper for operations
pub struct AutoRecovery {
    manager: RecoveryManager,
    context: RecoveryContext,
}

impl AutoRecovery {
    pub fn new(operation: &str, component: &str) -> Self {
        Self {
            manager: RecoveryManager::new(),
            context: RecoveryContext::new(operation, component, 3),
        }
    }
    
    pub fn with_manager(mut self, manager: RecoveryManager) -> Self {
        self.manager = manager;
        self
    }
    
    pub fn with_max_attempts(mut self, max_attempts: u32) -> Self {
        self.context.max_attempts = max_attempts;
        self
    }
    
    pub fn execute<T, F>(mut self, operation: F) -> Result<T>
    where
        F: Fn() -> Result<T>,
    {
        loop {
            match operation() {
                Ok(result) => return Ok(result),
                Err(error) => {
                    self.context.set_last_error(error.clone());
                    
                    if !self.context.has_attempts_left() {
                        return Err(error);
                    }
                    
                    // Check if we should retry based on error type
                    if !self.manager.should_retry(&error) {
                        return Err(error);
                    }
                    
                    // Get retry configuration
                    if let Some(config) = self.manager.get_retry_config(&error) {
                        let delay = std::cmp::min(
                            Duration::from_millis(
                                (config.base_delay.as_millis() as f64 * 
                                 config.backoff_multiplier.powi(self.context.attempt as i32)) as u64
                            ),
                            config.max_delay
                        );
                        
                        thread::sleep(delay);
                    }
                    
                    self.context.increment_attempt();
                }
            }
        }
    }
}

/// Recovery utilities for common operations
pub mod utils {
    use super::*;
    use std::fs;
    use std::path::Path;
    
    /// Retry file operations with automatic recovery
    pub fn retry_file_operation<T, F>(operation: F, path: &Path) -> Result<T>
    where
        F: Fn() -> Result<T>,
    {
        let config = RetryConfig::new(3)
            .with_delay(Duration::from_millis(100))
            .with_max_delay(Duration::from_secs(2));
        
        operation.retry_with_config(config)
    }
    
    /// Retry network operations with automatic recovery
    pub fn retry_network_operation<T, F>(operation: F, url: &str) -> Result<T>
    where
        F: Fn() -> Result<T>,
    {
        let config = RetryConfig::new(3)
            .with_delay(Duration::from_secs(1))
            .with_max_delay(Duration::from_secs(30))
            .with_backoff_multiplier(2.0)
            .with_jitter(true);
        
        operation.retry_with_config(config)
    }
    
    /// Retry system commands with automatic recovery
    pub fn retry_system_command<T, F>(operation: F, command: &str) -> Result<T>
    where
        F: Fn() -> Result<T>,
    {
        let config = RetryConfig::new(2)
            .with_delay(Duration::from_millis(500))
            .with_max_delay(Duration::from_secs(5));
        
        operation.retry_with_config(config)
    }
    
    /// Ensure directory exists with retry
    pub fn ensure_directory_with_retry(path: &Path) -> Result<()> {
        retry_file_operation(|| {
            fs::create_dir_all(path)
                .map_err(BuilderError::from)
        }, path)
    }
    
    /// Read file with retry
    pub fn read_file_with_retry(path: &Path) -> Result<String> {
        retry_file_operation(|| {
            fs::read_to_string(path)
                .map_err(BuilderError::from)
        }, path)
    }
    
    /// Write file with retry
    pub fn write_file_with_retry(path: &Path, contents: &str) -> Result<()> {
        retry_file_operation(|| {
            fs::write(path, contents)
                .map_err(BuilderError::from)
        }, path)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicU32, Ordering};
    use std::sync::Arc;
    
    #[test]
    fn test_retry_config_default() {
        let config = RetryConfig::default();
        assert_eq!(config.max_attempts, 3);
        assert_eq!(config.base_delay, Duration::from_millis(100));
        assert!(config.jitter);
    }
    
    #[test]
    fn test_retry_config_builder() {
        let config = RetryConfig::new(5)
            .with_delay(Duration::from_secs(1))
            .with_max_delay(Duration::from_secs(10))
            .with_backoff_multiplier(1.5)
            .with_jitter(false);
        
        assert_eq!(config.max_attempts, 5);
        assert_eq!(config.base_delay, Duration::from_secs(1));
        assert_eq!(config.max_delay, Duration::from_secs(10));
        assert_eq!(config.backoff_multiplier, 1.5);
        assert!(!config.jitter);
    }
    
    #[test]
    fn test_recovery_context() {
        let mut context = RecoveryContext::new("test_operation", "TestComponent", 3);
        
        assert_eq!(context.attempt, 0);
        assert!(context.has_attempts_left());
        assert_eq!(context.get_progress(), 0.0);
        
        context.increment_attempt();
        assert_eq!(context.attempt, 1);
        assert!(context.has_attempts_left());
        
        context.increment_attempt();
        context.increment_attempt();
        assert_eq!(context.attempt, 3);
        assert!(!context.has_attempts_left());
        assert_eq!(context.get_progress(), 1.0);
    }
    
    #[test]
    fn test_retryable_success() {
        let counter = Arc::new(AtomicU32::new(0));
        let counter_clone = Arc::clone(&counter);
        
        let operation = || {
            counter_clone.fetch_add(1, Ordering::SeqCst);
            Ok(42)
        };
        
        let result = operation.retry_with_config(RetryConfig::new(3));
        assert!(result.is_ok());
        assert_eq!(result.unwrap(), 42);
        assert_eq!(counter.load(Ordering::SeqCst), 1);
    }
    
    #[test]
    fn test_retryable_failure_then_success() {
        let counter = Arc::new(AtomicU32::new(0));
        let counter_clone = Arc::clone(&counter);
        
        let operation = || {
            let count = counter_clone.fetch_add(1, Ordering::SeqCst);
            if count < 2 {
                Err(BuilderError::Custom("temporary failure".to_string()))
            } else {
                Ok(42)
            }
        };
        
        let result = operation.retry_with_config(RetryConfig::new(3));
        assert!(result.is_ok());
        assert_eq!(result.unwrap(), 42);
        assert_eq!(counter.load(Ordering::SeqCst), 3);
    }
    
    #[test]
    fn test_retryable_always_fails() {
        let counter = Arc::new(AtomicU32::new(0));
        let counter_clone = Arc::clone(&counter);
        
        let operation = || {
            counter_clone.fetch_add(1, Ordering::SeqCst);
            Err(BuilderError::Custom("always fails".to_string()))
        };
        
        let result = operation.retry_with_config(RetryConfig::new(3));
        assert!(result.is_err());
        assert_eq!(counter.load(Ordering::SeqCst), 3);
    }
    
    #[test]
    fn test_recovery_manager() {
        let manager = RecoveryManager::new();
        
        let network_error = BuilderError::NetworkError("connection failed".to_string());
        assert!(manager.should_retry(&network_error));
        
        let file_not_found = BuilderError::FileNotFound(std::path::PathBuf::from("test.txt"));
        assert!(manager.should_ask_user(&file_not_found));
        
        let custom_error = BuilderError::Custom("unknown error".to_string());
        assert!(!manager.should_retry(&custom_error));
        assert!(!manager.should_ask_user(&custom_error));
    }
}