use std::fs::{self, OpenOptions};
use std::io::Write;
use std::path::Path;
use chrono::Local;
use cursive::event::{Event, MouseEvent, Key, MouseButton};
use log::Level;

pub struct AppLogger {
    log_level: Level,
    log_mouse: bool,
    log_keyboard: bool,
    log_processes: bool,
}

impl AppLogger {
    pub fn new() -> Self {
        // Create logs directory if it doesn't exist
        if let Err(e) = fs::create_dir_all("logs") {
            eprintln!("Failed to create logs directory: {}", e);
        }
        
        AppLogger {
            log_level: Level::Warn, // Default to warning as requested
            log_mouse: true,
            log_keyboard: true,
            log_processes: true,
        }
    }
    
    pub fn set_log_level(&mut self, level: Level) {
        self.log_level = level;
    }
    
    pub fn set_mouse_logging(&mut self, enabled: bool) {
        self.log_mouse = enabled;
    }
    
    pub fn set_keyboard_logging(&mut self, enabled: bool) {
        self.log_keyboard = enabled;
    }
    
    pub fn set_process_logging(&mut self, enabled: bool) {
        self.log_processes = enabled;
    }
    
    pub fn log_event(&self, event: &Event) {
        let timestamp = Local::now();
        let log_entry = match event {
            Event::Mouse { event, position, offset } => {
                if self.log_mouse {
                    let mouse_detail = match event {
                        MouseEvent::Press(button) => format!("PRESS({:?})", button),
                        MouseEvent::Release(button) => format!("RELEASE({:?})", button),
                        MouseEvent::Hold(button) => format!("HOLD({:?})", button),
                        MouseEvent::WheelUp => "WHEEL_UP".to_string(),
                        MouseEvent::WheelDown => "WHEEL_DOWN".to_string(),
                    };
                    format!("[{}] MOUSE: {} at position ({},{}) offset ({},{})", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                           mouse_detail, position.x, position.y, offset.x, offset.y)
                } else {
                    return;
                }
            }
            Event::Key(key) => {
                if self.log_keyboard {
                    let key_detail = match key {
                        Key::Enter => "ENTER".to_string(),
                        Key::Tab => "TAB".to_string(),
                        Key::Backspace => "BACKSPACE".to_string(),
                        Key::Esc => "ESCAPE".to_string(),
                        Key::Left => "ARROW_LEFT".to_string(),
                        Key::Right => "ARROW_RIGHT".to_string(),
                        Key::Up => "ARROW_UP".to_string(),
                        Key::Down => "ARROW_DOWN".to_string(),
                        Key::F1 => "F1".to_string(),
                        Key::F2 => "F2".to_string(),
                        Key::F3 => "F3".to_string(),
                        Key::F4 => "F4".to_string(),
                        Key::F5 => "F5".to_string(),
                        Key::F6 => "F6".to_string(),
                        Key::F7 => "F7".to_string(),
                        Key::F8 => "F8".to_string(),
                        Key::F9 => "F9".to_string(),
                        Key::F10 => "F10".to_string(),
                        Key::F11 => "F11".to_string(),
                        Key::F12 => "F12".to_string(),
                        other => format!("{:?}", other),
                    };
                    format!("[{}] KEYBOARD: {}", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), key_detail)
                } else {
                    return;
                }
            }
            Event::Char(c) => {
                if self.log_keyboard {
                    format!("[{}] CHARACTER: '{}'", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), c)
                } else {
                    return;
                }
            }
            Event::Ctrl(c) => {
                if self.log_keyboard {
                    format!("[{}] CTRL+{:?}", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), c)
                } else {
                    return;
                }
            }
            Event::Alt(c) => {
                if self.log_keyboard {
                    format!("[{}] ALT+{:?}", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), c)
                } else {
                    return;
                }
            }
            Event::Shift(key) => {
                if self.log_keyboard {
                    format!("[{}] SHIFT+{:?}", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), key)
                } else {
                    return;
                }
            }
            Event::CtrlChar(c) => {
                if self.log_keyboard {
                    format!("[{}] CTRL_CHAR: '{}'", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), c)
                } else {
                    return;
                }
            }
            Event::AltChar(c) => {
                if self.log_keyboard {
                    format!("[{}] ALT_CHAR: '{}'", 
                           timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), c)
                } else {
                    return;
                }
            }
            Event::Refresh => {
                format!("[{}] REFRESH", 
                       timestamp.format("%Y-%m-%d %H:%M:%S%.3f"))
            }
            Event::WindowResize => {
                format!("[{}] WINDOW_RESIZE", 
                       timestamp.format("%Y-%m-%d %H:%M:%S%.3f"))
            }
            other => {
                format!("[{}] OTHER_EVENT: {:?}", 
                       timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), other)
            }
        };
        
        self.write_to_log("events.log", &log_entry);
    }
    
    pub fn log_process(&self, command: &str, args: &[&str]) {
        if !self.log_processes {
            return;
        }
        
        let timestamp = Local::now();
        let log_entry = format!("[{}] PROCESS: {} {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               command, 
                               args.join(" "));
        
        self.write_to_log("processes.log", &log_entry);
    }
    
    pub fn log_process_output(&self, command: &str, stdout: &str, stderr: &str, exit_code: Option<i32>) {
        if !self.log_processes {
            return;
        }
        
        let timestamp = Local::now();
        let log_entry = format!("[{}] PROCESS_OUTPUT: {} | Exit: {:?} | STDOUT: {} | STDERR: {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               command, 
                               exit_code,
                               stdout.trim(),
                               stderr.trim());
        
        self.write_to_log("processes.log", &log_entry);
    }
    
    pub fn log_error(&self, error: &str) {
        let timestamp = Local::now();
        let log_entry = format!("[{}] ERROR: {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               error);
        
        self.write_to_log("errors.log", &log_entry);
    }
    
    pub fn log_warning(&self, warning: &str) {
        if self.log_level > Level::Warn {
            return;
        }
        
        let timestamp = Local::now();
        let log_entry = format!("[{}] WARNING: {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               warning);
        
        self.write_to_log("main.log", &log_entry);
    }
    
    pub fn log_info(&self, info: &str) {
        if self.log_level > Level::Info {
            return;
        }
        
        let timestamp = Local::now();
        let log_entry = format!("[{}] INFO: {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               info);
        
        self.write_to_log("main.log", &log_entry);
    }
    
    pub fn log_debug(&self, debug: &str) {
        if self.log_level > Level::Debug {
            return;
        }
        
        let timestamp = Local::now();
        let log_entry = format!("[{}] DEBUG: {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               debug);
        
        self.write_to_log("main.log", &log_entry);
    }
    
    pub fn log_ui_action(&self, action: &str, details: &str) {
        let timestamp = Local::now();
        let log_entry = format!("[{}] UI_ACTION: {} | {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               action, details);
        
        self.write_to_log("ui_actions.log", &log_entry);
    }
    
    pub fn log_menu_selection(&self, menu: &str, item: &str) {
        let timestamp = Local::now();
        let log_entry = format!("[{}] MENU_SELECTION: {} -> {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               menu, item);
        
        self.write_to_log("ui_actions.log", &log_entry);
    }
    
    pub fn log_dialog_action(&self, dialog: &str, action: &str, details: &str) {
        let timestamp = Local::now();
        let log_entry = format!("[{}] DIALOG: {} | {} | {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               dialog, action, details);
        
        self.write_to_log("ui_actions.log", &log_entry);
    }
    
    pub fn log_build_step(&self, step: &str, status: &str, details: &str) {
        let timestamp = Local::now();
        let log_entry = format!("[{}] BUILD_STEP: {} | {} | {}", 
                               timestamp.format("%Y-%m-%d %H:%M:%S%.3f"), 
                               step, status, details);
        
        self.write_to_log("build.log", &log_entry);
    }
    
    fn should_log_event(&self, event: &Event) -> bool {
        match event {
            Event::Mouse { .. } => self.log_mouse,
            Event::Key(_) => self.log_keyboard,
            _ => true,
        }
    }
    
    fn write_to_log(&self, filename: &str, entry: &str) {
        let log_path = Path::new("logs").join(filename);
        
        if let Ok(mut file) = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&log_path) {
            if let Err(e) = writeln!(file, "{}", entry) {
                eprintln!("Failed to write to log file {}: {}", filename, e);
            }
        } else {
            eprintln!("Failed to open log file: {}", log_path.display());
        }
    }
    
    pub fn get_log_level(&self) -> Level {
        self.log_level
    }
    
    pub fn is_mouse_logging_enabled(&self) -> bool {
        self.log_mouse
    }
    
    pub fn is_keyboard_logging_enabled(&self) -> bool {
        self.log_keyboard
    }
    
    pub fn is_process_logging_enabled(&self) -> bool {
        self.log_processes
    }
}

// Global logger instance
use std::sync::{Arc, Mutex};
use std::sync::OnceLock;

static GLOBAL_LOGGER: OnceLock<Arc<Mutex<AppLogger>>> = OnceLock::new();

pub fn init_global_logger() {
    let logger = Arc::new(Mutex::new(AppLogger::new()));
    if GLOBAL_LOGGER.set(logger).is_err() {
        eprintln!("Global logger already initialized");
    }
}

pub fn get_global_logger() -> Option<Arc<Mutex<AppLogger>>> {
    GLOBAL_LOGGER.get().cloned()
}

pub fn log_event(event: &Event) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_event(event);
        }
    }
}

pub fn log_process(command: &str, args: &[&str]) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_process(command, args);
        }
    }
}

pub fn log_process_output(command: &str, stdout: &str, stderr: &str, exit_code: Option<i32>) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_process_output(command, stdout, stderr, exit_code);
        }
    }
}

pub fn log_error(error: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_error(error);
        }
    }
}

pub fn log_warning(warning: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_warning(warning);
        }
    }
}

pub fn log_info(info: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_info(info);
        }
    }
}

pub fn log_debug(debug: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_debug(debug);
        }
    }
}

pub fn log_ui_action(action: &str, details: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_ui_action(action, details);
        }
    }
}

pub fn log_menu_selection(menu: &str, item: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_menu_selection(menu, item);
        }
    }
}

pub fn log_dialog_action(dialog: &str, action: &str, details: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_dialog_action(dialog, action, details);
        }
    }
}

pub fn log_build_step(step: &str, status: &str, details: &str) {
    if let Some(logger) = get_global_logger() {
        if let Ok(logger) = logger.lock() {
            logger.log_build_step(step, status, details);
        }
    }
}