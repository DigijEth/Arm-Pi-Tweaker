use serde::{Deserialize, Serialize};
use std::fs;
use std::path::Path;
use chrono::{DateTime, Utc};
use crate::error::{BuilderError, Result};
use crate::utils;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UpdateConfig {
    pub kernel_sources: Vec<KernelSourceConfig>,
    pub distro_releases: Vec<DistroReleaseConfig>,
    pub update_schedule: UpdateScheduleConfig,
    pub notifications: NotificationConfig,
    pub last_updated: DateTime<Utc>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct KernelSourceConfig {
    pub name: String,
    pub repo_url: String,
    pub branch: String,
    pub current_version: String,
    pub latest_version: String,
    pub last_checked: DateTime<Utc>,
    pub enabled: bool,
    pub auto_update: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DistroReleaseConfig {
    pub name: String,
    pub version: String,
    pub codename: String,
    pub current_version: String,
    pub latest_version: String,
    pub release_date: String,
    pub support_end: String,
    pub last_checked: DateTime<Utc>,
    pub enabled: bool,
    pub track_lts_only: bool,
    pub security_updates: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UpdateScheduleConfig {
    pub frequency: String, // "hourly", "daily", "weekly", "monthly", "manual"
    pub check_time: String, // "06:00" for daily checks
    pub last_check: DateTime<Utc>,
    pub next_check: DateTime<Utc>,
    pub enabled: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NotificationConfig {
    pub notify_available: bool,
    pub notify_critical: bool,
    pub notify_eol: bool,
    pub notification_method: String, // "popup", "email", "log"
    pub email_address: Option<String>,
}

impl Default for UpdateConfig {
    fn default() -> Self {
        UpdateConfig {
            kernel_sources: vec![
                KernelSourceConfig {
                    name: "Linus's Linux Repository".to_string(),
                    repo_url: "https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git".to_string(),
                    branch: "master".to_string(),
                    current_version: "6.6.8".to_string(),
                    latest_version: "6.7.1".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    auto_update: false,
                },
                KernelSourceConfig {
                    name: "Joshua-Riek's Ubuntu Kernel".to_string(),
                    repo_url: "https://github.com/Joshua-Riek/linux-rockchip".to_string(),
                    branch: "rk3588".to_string(),
                    current_version: "6.1.75".to_string(),
                    latest_version: "6.1.78".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    auto_update: false,
                },
                KernelSourceConfig {
                    name: "Armbian Kernel".to_string(),
                    repo_url: "https://github.com/armbian/linux-rockchip64".to_string(),
                    branch: "rk3588-6.1.y".to_string(),
                    current_version: "6.1.63".to_string(),
                    latest_version: "6.1.63".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    auto_update: false,
                },
                KernelSourceConfig {
                    name: "Mainline Stable".to_string(),
                    repo_url: "https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git".to_string(),
                    branch: "linux-6.6.y".to_string(),
                    current_version: "6.6.8".to_string(),
                    latest_version: "6.6.10".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    auto_update: false,
                },
            ],
            distro_releases: vec![
                DistroReleaseConfig {
                    name: "Ubuntu".to_string(),
                    version: "22.04".to_string(),
                    codename: "Jammy Jellyfish".to_string(),
                    current_version: "22.04.3".to_string(),
                    latest_version: "22.04.4".to_string(),
                    release_date: "2022-04-21".to_string(),
                    support_end: "2027-04-21".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    track_lts_only: true,
                    security_updates: true,
                },
                DistroReleaseConfig {
                    name: "Ubuntu".to_string(),
                    version: "24.04".to_string(),
                    codename: "Noble Numbat".to_string(),
                    current_version: "24.04.0".to_string(),
                    latest_version: "24.04.1".to_string(),
                    release_date: "2024-04-25".to_string(),
                    support_end: "2029-04-25".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    track_lts_only: true,
                    security_updates: true,
                },
                DistroReleaseConfig {
                    name: "Debian".to_string(),
                    version: "12".to_string(),
                    codename: "Bookworm".to_string(),
                    current_version: "12.4".to_string(),
                    latest_version: "12.4".to_string(),
                    release_date: "2023-06-10".to_string(),
                    support_end: "2028-06-10".to_string(),
                    last_checked: Utc::now(),
                    enabled: true,
                    track_lts_only: false,
                    security_updates: true,
                },
                DistroReleaseConfig {
                    name: "Debian".to_string(),
                    version: "13".to_string(),
                    codename: "Trixie".to_string(),
                    current_version: "13.0".to_string(),
                    latest_version: "13.0".to_string(),
                    release_date: "2024-12-01".to_string(),
                    support_end: "2029-12-01".to_string(),
                    last_checked: Utc::now(),
                    enabled: false,
                    track_lts_only: false,
                    security_updates: true,
                },
            ],
            update_schedule: UpdateScheduleConfig {
                frequency: "daily".to_string(),
                check_time: "06:00".to_string(),
                last_check: Utc::now(),
                next_check: Utc::now(),
                enabled: true,
            },
            notifications: NotificationConfig {
                notify_available: true,
                notify_critical: true,
                notify_eol: true,
                notification_method: "popup".to_string(),
                email_address: None,
            },
            last_updated: Utc::now(),
        }
    }
}

impl UpdateConfig {
    pub fn load_from_file<P: AsRef<Path>>(path: P) -> Result<Self> {
        let path = path.as_ref();
        
        // Validate file exists
        utils::validate_file_exists(&path.to_path_buf())?;
        
        let content = fs::read_to_string(path)
            .map_err(BuilderError::from)?;
        
        let config: UpdateConfig = toml::from_str(&content)
            .map_err(BuilderError::from)?;
        
        Ok(config)
    }

    pub fn save_to_file<P: AsRef<Path>>(&self, path: P) -> Result<()> {
        let path = path.as_ref();
        
        // Ensure parent directory exists
        if let Some(parent) = path.parent() {
            utils::ensure_dir_exists(&parent.to_path_buf())?;
        }
        
        let content = toml::to_string_pretty(self)
            .map_err(BuilderError::from)?;
        
        fs::write(path, content)
            .map_err(BuilderError::from)?;
        
        Ok(())
    }

    pub fn get_config_path() -> Result<String> {
        let config_dir = utils::get_config_dir()?
            .join("orange-pi-builder");
        
        utils::ensure_dir_exists(&config_dir)?;
        
        Ok(config_dir.join("update_config.toml").to_string_lossy().to_string())
    }

    pub fn load_or_create_default() -> Result<Self> {
        let config_path = Self::get_config_path()?;
        
        match Self::load_from_file(&config_path) {
            Ok(config) => Ok(config),
            Err(_) => {
                let default_config = Self::default();
                default_config.save_to_file(&config_path)?;
                Ok(default_config)
            }
        }
    }

    pub fn save(&self) -> Result<()> {
        let config_path = Self::get_config_path()?;
        self.save_to_file(config_path)
    }

    pub fn update_kernel_source(&mut self, name: &str, new_version: &str) -> bool {
        if let Some(source) = self.kernel_sources.iter_mut().find(|s| s.name == name) {
            source.current_version = new_version.to_string();
            source.last_checked = Utc::now();
            self.last_updated = Utc::now();
            true
        } else {
            false
        }
    }

    pub fn update_distro_release(&mut self, name: &str, version: &str, new_version: &str) -> bool {
        if let Some(release) = self.distro_releases.iter_mut().find(|r| r.name == name && r.version == version) {
            release.current_version = new_version.to_string();
            release.last_checked = Utc::now();
            self.last_updated = Utc::now();
            true
        } else {
            false
        }
    }

    pub fn get_outdated_kernel_sources(&self) -> Vec<&KernelSourceConfig> {
        self.kernel_sources.iter()
            .filter(|source| source.enabled && source.current_version != source.latest_version)
            .collect()
    }

    pub fn get_outdated_distro_releases(&self) -> Vec<&DistroReleaseConfig> {
        self.distro_releases.iter()
            .filter(|release| release.enabled && release.current_version != release.latest_version)
            .collect()
    }

    pub fn get_enabled_kernel_sources(&self) -> Vec<&KernelSourceConfig> {
        self.kernel_sources.iter()
            .filter(|source| source.enabled)
            .collect()
    }

    pub fn get_enabled_distro_releases(&self) -> Vec<&DistroReleaseConfig> {
        self.distro_releases.iter()
            .filter(|release| release.enabled)
            .collect()
    }

    pub fn update_schedule(&mut self, frequency: &str, check_time: &str) {
        self.update_schedule.frequency = frequency.to_string();
        self.update_schedule.check_time = check_time.to_string();
        self.last_updated = Utc::now();
    }

    pub fn update_notifications(&mut self, notify_available: bool, notify_critical: bool, notify_eol: bool) {
        self.notifications.notify_available = notify_available;
        self.notifications.notify_critical = notify_critical;
        self.notifications.notify_eol = notify_eol;
        self.last_updated = Utc::now();
    }

    pub fn should_check_updates(&self) -> bool {
        if !self.update_schedule.enabled {
            return false;
        }

        let now = Utc::now();
        match self.update_schedule.frequency.as_str() {
            "hourly" => (now - self.update_schedule.last_check).num_hours() >= 1,
            "daily" => (now - self.update_schedule.last_check).num_days() >= 1,
            "weekly" => (now - self.update_schedule.last_check).num_weeks() >= 1,
            "monthly" => (now - self.update_schedule.last_check).num_days() >= 30,
            _ => false,
        }
    }

    pub fn mark_check_completed(&mut self) {
        self.update_schedule.last_check = Utc::now();
        self.last_updated = Utc::now();
    }

    pub fn get_stats(&self) -> UpdateStats {
        let enabled_kernel_sources = self.get_enabled_kernel_sources().len();
        let outdated_kernel_sources = self.get_outdated_kernel_sources().len();
        let enabled_distro_releases = self.get_enabled_distro_releases().len();
        let outdated_distro_releases = self.get_outdated_distro_releases().len();

        UpdateStats {
            enabled_kernel_sources,
            outdated_kernel_sources,
            enabled_distro_releases,
            outdated_distro_releases,
            total_updates_available: outdated_kernel_sources + outdated_distro_releases,
            last_check: self.update_schedule.last_check,
            next_scheduled_check: self.update_schedule.next_check,
        }
    }
}

#[derive(Debug, Clone)]
pub struct UpdateStats {
    pub enabled_kernel_sources: usize,
    pub outdated_kernel_sources: usize,
    pub enabled_distro_releases: usize,
    pub outdated_distro_releases: usize,
    pub total_updates_available: usize,
    pub last_check: DateTime<Utc>,
    pub next_scheduled_check: DateTime<Utc>,
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs::File;
    use std::io::Write;
    use tempfile::tempdir;

    #[test]
    fn test_default_config() {
        let config = UpdateConfig::default();
        assert_eq!(config.kernel_sources.len(), 4);
        assert_eq!(config.distro_releases.len(), 4);
        assert!(config.update_schedule.enabled);
        assert!(config.notifications.notify_critical);
    }

    #[test]
    fn test_save_and_load_config() {
        let temp_dir = tempdir().unwrap();
        let config_path = temp_dir.path().join("test_config.toml");
        
        let config = UpdateConfig::default();
        config.save_to_file(&config_path).unwrap();
        
        let loaded_config = UpdateConfig::load_from_file(&config_path).unwrap();
        assert_eq!(config.kernel_sources.len(), loaded_config.kernel_sources.len());
        assert_eq!(config.distro_releases.len(), loaded_config.distro_releases.len());
    }

    #[test]
    fn test_update_kernel_source() {
        let mut config = UpdateConfig::default();
        let result = config.update_kernel_source("Linus's Linux Repository", "6.7.2");
        assert!(result);
        
        let source = config.kernel_sources.iter().find(|s| s.name == "Linus's Linux Repository").unwrap();
        assert_eq!(source.current_version, "6.7.2");
    }

    #[test]
    fn test_get_outdated_sources() {
        let config = UpdateConfig::default();
        let outdated = config.get_outdated_kernel_sources();
        assert!(outdated.len() > 0);
    }

    #[test]
    fn test_should_check_updates() {
        let mut config = UpdateConfig::default();
        config.update_schedule.last_check = Utc::now() - chrono::Duration::days(2);
        config.update_schedule.frequency = "daily".to_string();
        
        assert!(config.should_check_updates());
        
        config.update_schedule.last_check = Utc::now();
        assert!(!config.should_check_updates());
    }

    #[test]
    fn test_get_stats() {
        let config = UpdateConfig::default();
        let stats = config.get_stats();
        
        assert_eq!(stats.enabled_kernel_sources, 4);
        assert_eq!(stats.enabled_distro_releases, 3); // One Debian release is disabled by default
        assert!(stats.total_updates_available > 0);
    }
}