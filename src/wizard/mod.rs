use cursive::Cursive;
use cursive::views::{Dialog, TextView, LinearLayout, DummyView, Checkbox, SelectView, EditView, ProgressBar};
use cursive::traits::*;
use cursive::theme::{ColorStyle, BaseColor, Color};
use log::info;
use crate::devicetree::BuildType;
use crate::download::DownloadManager;
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;
use std::io::Write;

// Build configuration struct to hold all wizard selections
#[derive(Debug, Clone)]
pub struct WizardBuildConfig {
    pub distro: String,
    pub distro_version: String,
    pub kernel: String,
    pub kernel_version: String,
    pub desktop_env: String,
    pub gpu_driver: String,
    pub bootloader: String,
    pub build_type: BuildType,
    pub root_password: String,
    pub username: String,
    pub user_password: String,
    pub hostname: String,
    pub locale: String,
    pub timezone: String,
    pub packages: Vec<String>,
}

impl Default for WizardBuildConfig {
    fn default() -> Self {
        Self {
            distro: "ubuntu".to_string(),
            distro_version: "22.04".to_string(),
            kernel: "rockchip".to_string(),
            kernel_version: "6.1".to_string(),
            desktop_env: "lxqt".to_string(),
            gpu_driver: "valhall-g610".to_string(),
            bootloader: "uboot-mainline".to_string(),
            build_type: BuildType::DesktopServer,
            root_password: "orangepi".to_string(),
            username: "orangepi".to_string(),
            user_password: "orangepi".to_string(),
            hostname: "orangepi5plus".to_string(),
            locale: "en_US.UTF-8".to_string(),
            timezone: "UTC".to_string(),
            packages: vec![],
        }
    }
}

pub fn show_distro_wizard(siv: &mut Cursive) {
    info!("Opening Distro Wizard");
    crate::ui::logger::log_ui_action("WIZARD_START", "Distro Wizard opened");
    
    // Initialize build config
    let config = Arc::new(Mutex::new(WizardBuildConfig::default()));
    siv.set_user_data(config);
    
    // Show category selection first
    show_category_selection(siv);
}

// Helper function to update menu items with asterisk for selected item
fn update_select_view_with_asterisk<T: 'static + Clone>(view: &mut SelectView<T>, selected_idx: usize) {
    let items: Vec<(String, T)> = view.iter()
        .enumerate()
        .map(|(idx, item)| {
            let label = item.0.clone();
            let value = item.1.clone();
            if idx == selected_idx {
                (format!("* {}", label.trim_start_matches("* ")), value)
            } else {
                (label.trim_start_matches("* ").to_string(), value)
            }
        })
        .collect();
    
    view.clear();
    for (label, value) in items {
        view.add_item(label, value);
    }
    view.set_selection(selected_idx);
}

fn show_category_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Select the type of build you want to create:"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut category_select = SelectView::<&str>::new()
        .item("  🖥️  Desktop/Server - Traditional Linux distributions (Coming Soon)", "desktop-server")
        .item("* 🎮 Emulation - Gaming and emulation focused builds", "emulation")
        .item("  🎬 Media Center - Kodi and media streaming builds (Coming Soon)", "media-center");
    
    category_select.set_on_select(|s, category| {
        update_category_description(s, category);
        // Update asterisks when selection changes
        s.call_on_name("category_select", |view: &mut SelectView<&str>| {
            let selected_idx = view.selected_id().unwrap_or(0);
            let items = vec![
                ("🖥️  Desktop/Server - Traditional Linux distributions", "desktop-server"),
                ("🎮 Emulation - Gaming and emulation focused builds", "emulation"),
                ("🎬 Media Center - Kodi and media streaming builds", "media-center"),
            ];
            view.clear();
            for (idx, (label, value)) in items.iter().enumerate() {
                if idx == selected_idx {
                    view.add_item(format!("* {}", label), value);
                } else {
                    view.add_item(format!("  {}", label), value);
                }
            }
            view.set_selection(selected_idx);
        });
    });
    
    // Set initial selection
    category_select.set_selection(0);
    
    layout.add_child(category_select.with_name("category_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("category_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Build Category Selection")
        .button("Next", |s| {
            let selected_category = s.call_on_name("category_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(category) = selected_category {
                // Update build config
                if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                    let mut cfg = config.lock().unwrap();
                    cfg.build_type = match category {
                        "desktop-server" => BuildType::DesktopServer,
                        "emulation" => BuildType::GameScopePi,
                        "media-center" => BuildType::KodiMediaCenter,
                        _ => BuildType::DesktopServer,
                    };
                }
                
                match category {
                    "desktop-server" | "media-center" => {
                        s.add_layer(
                            Dialog::text("This option is coming soon!\n\nPlease select the Emulation option for GameScope builds.")
                                .title("Coming Soon")
                                .button("OK", |s| { s.pop_layer(); })
                        );
                    }
                    "emulation" => {
                        s.pop_layer();
                        show_emulation_selection(s);
                    }
                    _ => {}
                }
            }
        })
        .button("Cancel", |s| {
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
    update_category_description(siv, &"desktop-server");
}

fn update_category_description(siv: &mut Cursive, category: &&str) {
    let description = match *category {
        "desktop-server" => "Build traditional Linux distributions with desktop environments or server configurations.\nSupports Ubuntu, Debian, and other distributions with various desktop environments.",
        "emulation" => "Gaming-focused builds with emulation support.\nIncludes GameScope-Pi for Steam Deck-like experience and RetroArch for retro gaming.",
        "media-center" => "Media-focused builds for home entertainment.\nIncludes Kodi with hardware acceleration and media server options.",
        _ => "",
    };
    
    siv.call_on_name("category_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_base_system_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Select base system and kernel:"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Distribution selection
    layout.add_child(TextView::new("Distribution:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let mut base_select = SelectView::<(&str, &str)>::new()
        .item("* Ubuntu 22.04 LTS (Recommended)", ("ubuntu", "22.04"))
        .item("  Ubuntu 24.04 LTS", ("ubuntu", "24.04"))
        .item("  Ubuntu 25.04 (Experimental)", ("ubuntu", "25.04"))
        .item("  Debian 11 (Bullseye)", ("debian", "11"))
        .item("  Debian 12 (Bookworm)", ("debian", "12"))
        .item("  Debian 13 (Trixie) (Experimental)", ("debian", "13"));
    
    base_select.set_on_select(|s, item| {
        update_base_system_description(s, &item.0, &item.1);
        
        // Update asterisks
        s.call_on_name("base_system_select", |view: &mut SelectView<(&str, &str)>| {
            let selected_idx = view.selected_id().unwrap_or(0);
            let items = vec![
                ("Ubuntu 22.04 LTS (Recommended)", ("ubuntu", "22.04")),
                ("Ubuntu 24.04 LTS", ("ubuntu", "24.04")),
                ("Ubuntu 25.04 (Experimental)", ("ubuntu", "25.04")),
                ("Debian 11 (Bullseye)", ("debian", "11")),
                ("Debian 12 (Bookworm)", ("debian", "12")),
                ("Debian 13 (Trixie) (Experimental)", ("debian", "13")),
            ];
            view.clear();
            for (idx, (label, value)) in items.iter().enumerate() {
                if idx == selected_idx {
                    view.add_item(format!("* {}", label), value.clone());
                } else {
                    view.add_item(format!("  {}", label), value.clone());
                }
            }
            view.set_selection(selected_idx);
        });
    });
    
    base_select.set_selection(0);
    layout.add_child(base_select.with_name("base_system_select"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Kernel selection
    layout.add_child(TextView::new("Kernel:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let mut kernel_select = SelectView::<(&str, &str)>::new()
        .item("* Rockchip 6.1 (Recommended)", ("rockchip", "6.1"))
        .item("  Rockchip 5.1 (Stable)", ("rockchip", "5.1"));
    
    kernel_select.set_on_select(|s, item| {
        update_kernel_description_combined(s, &item.0, &item.1);
    });
    
    kernel_select.set_selection(0);
    layout.add_child(kernel_select.with_name("kernel_select"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Combined description area
    layout.add_child(TextView::new("").with_name("system_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Base System Selection")
        .button("Next", |s| {
            let selected_base = s.call_on_name("base_system_select", |view: &mut SelectView<(&str, &str)>| {
                view.selection()
            }).flatten();
            
            let selected_kernel = s.call_on_name("kernel_select", |view: &mut SelectView<(&str, &str)>| {
                view.selection()
            }).flatten();
            
            if let (Some(base), Some(kernel)) = (selected_base, selected_kernel) {
                // Update build config
                if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                    let mut cfg = config.lock().unwrap();
                    cfg.distro = base.0.to_string();
                    cfg.distro_version = base.1.to_string();
                    cfg.kernel = kernel.0.to_string();
                    cfg.kernel_version = kernel.1.to_string();
                }
                
                // Start downloads
                let download_manager = DownloadManager::new();
                if let Ok(manager) = download_manager {
                    // Download distribution files
                    if let Err(e) = manager.download_distro_files(base.0, base.1) {
                        crate::ui::logger::log_error(&format!("Failed to download {} {}: {}", base.0, base.1, e));
                    } else {
                        crate::ui::logger::log_info(&format!("Started downloading {} {} files", base.0, base.1));
                    }
                    
                    // Download kernel files
                    if let Err(e) = manager.download_kernel(kernel.1) {
                        crate::ui::logger::log_error(&format!("Failed to download kernel {}: {}", kernel.1, e));
                    } else {
                        crate::ui::logger::log_info(&format!("Started downloading kernel {} files", kernel.1));
                    }
                }
                
                s.pop_layer();
                show_desktop_environment_selection_new(s);
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_category_selection(s);
        });
    
    siv.add_layer(dialog);
    
    // Set initial descriptions
    update_base_system_description(siv, &"ubuntu", &"22.04");
    update_kernel_description_combined(siv, &"rockchip", &"6.1");
}

fn update_base_system_description(siv: &mut Cursive, distro: &&str, version: &&str) {
    let description = match (*distro, *version) {
        ("ubuntu", "22.04") => "Ubuntu 22.04 LTS - Stable, well-tested, excellent hardware support. Best choice for most users.",
        ("ubuntu", "24.04") => "Ubuntu 24.04 LTS - Latest LTS with modern packages and improved performance.",
        ("ubuntu", "25.04") => "Ubuntu 25.04 - Cutting edge features, may have stability issues. For testing only.",
        ("debian", "11") => "Debian 11 - Rock solid stability, conservative package versions, excellent for servers.",
        ("debian", "12") => "Debian 12 - Current stable release with good balance of stability and modern features.",
        ("debian", "13") => "Debian 13 - Testing branch with newer packages but potential instability.",
        _ => "Unknown distribution option.",
    };
    
    siv.call_on_name("system_description", |view: &mut TextView| {
        let current = view.get_content();
        let kernel_desc = if current.source().contains("\n\nKernel:") {
            current.source().split("\n\nKernel:").nth(1).unwrap_or("")
        } else {
            ""
        };
        view.set_content(format!("Distribution: {}{}", description, 
            if !kernel_desc.is_empty() { format!("\n\nKernel:{}", kernel_desc) } else { String::new() }));
    });
}

fn update_kernel_description_combined(siv: &mut Cursive, kernel: &&str, version: &&str) {
    let description = match (*kernel, *version) {
        ("rockchip", "6.1") => "Rockchip 6.1 kernel (develop-6.1 branch) with full RK3588S support and latest Rockchip optimizations. Recommended for GameScope builds.",
        ("rockchip", "5.1") => "Rockchip 5.1 kernel (develop-5.10-rt53 branch) with proven stability and hardware support. Ideal for stable gaming systems.",
        _ => "Unknown kernel option",
    };
    
    siv.call_on_name("system_description", |view: &mut TextView| {
        let current = view.get_content();
        let distro_desc = if current.source().contains("\n\nKernel:") {
            current.source().split("\n\nKernel:").next().unwrap_or("")
        } else {
            current.source()
        };
        view.set_content(format!("{}\n\nKernel: {}", distro_desc, description));
    });
}

fn show_desktop_environment_selection_new(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Select desktop environment or server configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Note: Mali G610 GPU requires Wayland support for desktop builds").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut de_select = SelectView::<&str>::new()
        .item("* LXQt - Lightweight, fast, low resource usage (Recommended)", "lxqt")
        .item("  GNOME - Modern, polished, full-featured", "gnome")
        .item("  KDE Plasma - Customizable, feature-rich", "kde")
        .item("  XFCE - Traditional, stable, lightweight", "xfce")
        .item("  Cinnamon - Familiar, Windows-like interface", "cinnamon")
        .item("  MATE - Classic GNOME 2 experience", "mate")
        .item("  Budgie - Modern, clean, macOS-inspired", "budgie")
        .item("  Server Minimal - Command line only, essential packages", "server-minimal")
        .item("  Server Standard - Command line with common server tools", "server")
        .item("  Server Full - Command line with development tools", "server-full");
    
    de_select.set_on_select(|s, de| {
        update_de_description_new(s, de);
        
        // Update asterisks
        s.call_on_name("desktop_env_select", |view: &mut SelectView<&str>| {
            let selected_idx = view.selected_id().unwrap_or(0);
            let items = vec![
                ("LXQt - Lightweight, fast, low resource usage (Recommended)", "lxqt"),
                ("GNOME - Modern, polished, full-featured", "gnome"),
                ("KDE Plasma - Customizable, feature-rich", "kde"),
                ("XFCE - Traditional, stable, lightweight", "xfce"),
                ("Cinnamon - Familiar, Windows-like interface", "cinnamon"),
                ("MATE - Classic GNOME 2 experience", "mate"),
                ("Budgie - Modern, clean, macOS-inspired", "budgie"),
                ("Server Minimal - Command line only, essential packages", "server-minimal"),
                ("Server Standard - Command line with common server tools", "server"),
                ("Server Full - Command line with development tools", "server-full"),
            ];
            view.clear();
            for (idx, (label, value)) in items.iter().enumerate() {
                if idx == selected_idx {
                    view.add_item(format!("* {}", label), value);
                } else {
                    view.add_item(format!("  {}", label), value);
                }
            }
            view.set_selection(selected_idx);
        });
    });
    
    de_select.set_selection(0);
    layout.add_child(de_select.with_name("desktop_env_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("desktop_env_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Desktop Environment / Server Configuration")
        .button("Next", |s| {
            let selected_de = s.call_on_name("desktop_env_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(de) = selected_de {
                // Update build config
                if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                    let mut cfg = config.lock().unwrap();
                    cfg.desktop_env = de.to_string();
                }
                
                // Download desktop environment files
                let download_manager = DownloadManager::new();
                if let Ok(manager) = download_manager {
                    if let Err(e) = manager.download_desktop_environment(de) {
                        crate::ui::logger::log_error(&format!("Failed to download {} desktop: {}", de, e));
                    } else {
                        crate::ui::logger::log_info(&format!("Started downloading {} desktop files", de));
                    }
                }
                
                s.pop_layer();
                if de.starts_with("server") {
                    show_server_packages_selection(s);
                } else {
                    show_gpu_driver_selection(s);
                }
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_base_system_selection(s);
        });
    
    siv.add_layer(dialog);
    update_de_description_new(siv, &"lxqt");
}

fn update_de_description_new(siv: &mut Cursive, de: &&str) {
    let description = match *de {
        "lxqt" => "LXQt - Best for Orange Pi 5 Plus. Low memory usage (~400MB), fast startup, Wayland ready.\nRecommended for gaming and media center builds.",
        "gnome" => "GNOME - Modern interface with touch support. Higher memory usage (~800MB).\nFull Wayland support with excellent Mali GPU integration.",
        "kde" => "KDE Plasma - Highly customizable with many features. Moderate memory usage (~600MB).\nGood Wayland support, excellent for power users.",
        "xfce" => "XFCE - Traditional desktop, very stable. Low memory usage (~350MB).\nX11 only, may have limited GPU acceleration.",
        "cinnamon" => "Cinnamon - Familiar interface similar to Windows. Moderate memory usage (~500MB).\nGood for users transitioning from Windows.",
        "mate" => "MATE - Continuation of GNOME 2. Low memory usage (~400MB).\nStable and traditional, good for older hardware.",
        "budgie" => "Budgie - Modern and clean interface. Moderate memory usage (~450MB).\nGood balance of features and performance.",
        "server-minimal" => "Minimal installation with only essential packages. ~1GB disk usage.\nNo GUI, SSH server, basic networking tools.",
        "server" => "Standard server installation. ~2-3GB disk usage.\nIncludes common server tools, development headers, Python/Perl.",
        "server-full" => "Full server installation. ~4-5GB disk usage.\nIncludes development tools, compilers, libraries, Docker support.",
        _ => "Unknown desktop environment.",
    };
    
    siv.call_on_name("desktop_env_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_server_packages_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Select additional server packages:"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Package categories with checkboxes
    let packages = vec![
        ("docker", "Docker CE - Container runtime"),
        ("kubernetes", "Kubernetes - Container orchestration"),
        ("nginx", "Nginx - High-performance web server"),
        ("apache2", "Apache - Traditional web server"),
        ("mysql", "MySQL - Relational database"),
        ("postgresql", "PostgreSQL - Advanced relational database"),
        ("mongodb", "MongoDB - NoSQL database"),
        ("redis", "Redis - In-memory data store"),
        ("nodejs", "Node.js - JavaScript runtime"),
        ("python3-full", "Python 3 - Full development environment"),
        ("golang", "Go - Programming language"),
        ("rust", "Rust - Systems programming language"),
        ("prometheus", "Prometheus - Monitoring system"),
        ("grafana", "Grafana - Metrics visualization"),
        ("jenkins", "Jenkins - CI/CD automation"),
        ("gitlab", "GitLab - Git repository manager"),
    ];
    
    for (pkg, desc) in packages {
        let checkbox = Checkbox::new().with_name(format!("pkg_{}", pkg));
        layout.add_child(LinearLayout::horizontal()
            .child(checkbox)
            .child(TextView::new(format!(" {} - {}", pkg, desc))));
    }
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Server Package Selection")
        .button("Next", |s| {
            // Collect selected packages
            let mut selected_packages = vec![];
            let package_names = vec![
                "docker", "kubernetes", "nginx", "apache2", "mysql", "postgresql",
                "mongodb", "redis", "nodejs", "python3-full", "golang", "rust",
                "prometheus", "grafana", "jenkins", "gitlab"
            ];
            
            for pkg in package_names {
                let is_selected = s.call_on_name(&format!("pkg_{}", pkg), |view: &mut Checkbox| {
                    view.is_checked()
                }).unwrap_or(false);
                
                if is_selected {
                    selected_packages.push(pkg.to_string());
                }
            }
            
            // Update build config
            if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                let mut cfg = config.lock().unwrap();
                cfg.packages = selected_packages;
            }
            
            s.pop_layer();
            show_system_configuration(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_desktop_environment_selection_new(s);
        });
    
    siv.add_layer(dialog);
}

fn show_gpu_driver_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Select GPU driver for Mali G610:"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut gpu_select = SelectView::<&str>::new()
        .item("* Valhall G610 (g13p0) - Latest proprietary driver", "valhall-g610")
        .item("  Panfrost - Open source Mesa driver", "panfrost")
        .item("  Panfork - Community Mesa fork with CSF", "panfork")
        .item("  Bifrost - Legacy compatibility mode", "bifrost");
    
    gpu_select.set_on_select(|s, gpu| {
        update_gpu_driver_description(s, gpu);
    });
    
    gpu_select.set_selection(0);
    layout.add_child(gpu_select.with_name("gpu_driver_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("gpu_driver_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("GPU Driver Selection")
        .button("Next", |s| {
            let selected_gpu = s.call_on_name("gpu_driver_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(gpu) = selected_gpu {
                // Update build config
                if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                    let mut cfg = config.lock().unwrap();
                    cfg.gpu_driver = gpu.to_string();
                }
                
                // Download GPU driver
                let download_manager = DownloadManager::new();
                if let Ok(manager) = download_manager {
                    if let Err(e) = manager.download_gpu_drivers(gpu) {
                        crate::ui::logger::log_error(&format!("Failed to download {} driver: {}", gpu, e));
                    } else {
                        crate::ui::logger::log_info(&format!("Started downloading {} driver files", gpu));
                    }
                }
                
                s.pop_layer();
                show_system_configuration(s);
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_desktop_environment_selection_new(s);
        });
    
    siv.add_layer(dialog);
    update_gpu_driver_description(siv, &"valhall-g610");
}

fn update_gpu_driver_description(siv: &mut Cursive, gpu: &&str) {
    let description = match *gpu {
        "valhall-g610" => "ARM proprietary driver with best performance.\nSupports OpenGL ES 3.2, Vulkan 1.2, OpenCL 2.2.\nRecommended for gaming and compute workloads.",
        "panfrost" => "Open source Mesa driver with good compatibility.\nSupports OpenGL ES 3.1, experimental Vulkan.\nBest for desktop use and open source enthusiasts.",
        "panfork" => "Community fork with CSF support for RK3588.\nImproved performance over Panfrost.\nGood for gaming on Linux.",
        "bifrost" => "Legacy mode for compatibility.\nMay have reduced features but stable.\nUse if other drivers have issues.",
        _ => "Unknown GPU driver option.",
    };
    
    siv.call_on_name("gpu_driver_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_system_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Hostname
    layout.add_child(TextView::new("Hostname:"));
    let hostname_input = EditView::new()
        .content("orangepi5plus")
        .with_name("hostname")
        .fixed_width(30);
    layout.add_child(hostname_input);
    layout.add_child(DummyView.fixed_height(1));
    
    // Root password
    layout.add_child(TextView::new("Root password:"));
    let root_password = EditView::new()
        .secret()
        .content("orangepi")
        .with_name("root_password")
        .fixed_width(30);
    layout.add_child(root_password);
    layout.add_child(DummyView.fixed_height(1));
    
    // Username
    layout.add_child(TextView::new("Username:"));
    let username_input = EditView::new()
        .content("orangepi")
        .with_name("username")
        .fixed_width(30);
    layout.add_child(username_input);
    
    // User password
    layout.add_child(TextView::new("User password:"));
    let user_password = EditView::new()
        .secret()
        .content("orangepi")
        .with_name("user_password")
        .fixed_width(30);
    layout.add_child(user_password);
    layout.add_child(DummyView.fixed_height(1));
    
    // Locale
    layout.add_child(TextView::new("Locale:"));
    let locale_select = SelectView::<&str>::new()
        .item("en_US.UTF-8 (English - United States)", "en_US.UTF-8")
        .item("en_GB.UTF-8 (English - United Kingdom)", "en_GB.UTF-8")
        .item("de_DE.UTF-8 (German - Germany)", "de_DE.UTF-8")
        .item("fr_FR.UTF-8 (French - France)", "fr_FR.UTF-8")
        .item("es_ES.UTF-8 (Spanish - Spain)", "es_ES.UTF-8")
        .item("zh_CN.UTF-8 (Chinese - China)", "zh_CN.UTF-8")
        .item("ja_JP.UTF-8 (Japanese - Japan)", "ja_JP.UTF-8")
        .with_name("locale");
    layout.add_child(locale_select);
    layout.add_child(DummyView.fixed_height(1));
    
    // Timezone
    layout.add_child(TextView::new("Timezone:"));
    let timezone_select = SelectView::<&str>::new()
        .item("UTC", "UTC")
        .item("America/New_York", "America/New_York")
        .item("America/Chicago", "America/Chicago")
        .item("America/Denver", "America/Denver")
        .item("America/Los_Angeles", "America/Los_Angeles")
        .item("Europe/London", "Europe/London")
        .item("Europe/Berlin", "Europe/Berlin")
        .item("Europe/Paris", "Europe/Paris")
        .item("Asia/Shanghai", "Asia/Shanghai")
        .item("Asia/Tokyo", "Asia/Tokyo")
        .with_name("timezone");
    layout.add_child(timezone_select);
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("System Configuration")
        .button("Next", |s| {
            // Collect all configuration
            let hostname = s.call_on_name("hostname", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_else(|| "orangepi5plus".to_string());
            
            let root_password = s.call_on_name("root_password", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_else(|| "orangepi".to_string());
            
            let username = s.call_on_name("username", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_else(|| "orangepi".to_string());
            
            let user_password = s.call_on_name("user_password", |view: &mut EditView| {
                view.get_content().to_string()
            }).unwrap_or_else(|| "orangepi".to_string());
            
            let locale = s.call_on_name("locale", |view: &mut SelectView<&str>| {
                view.selection().map(|s| *s).unwrap_or("en_US.UTF-8")
            }).unwrap_or("en_US.UTF-8");
            
            let timezone = s.call_on_name("timezone", |view: &mut SelectView<&str>| {
                view.selection().map(|s| *s).unwrap_or("UTC")
            }).unwrap_or("UTC");
            
            // Update build config
            if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                let mut cfg = config.lock().unwrap();
                cfg.hostname = hostname;
                cfg.root_password = root_password;
                cfg.username = username;
                cfg.user_password = user_password;
                cfg.locale = locale.to_string();
                cfg.timezone = timezone.to_string();
            }
            
            s.pop_layer();
            show_bootloader_selection(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            // Go back to appropriate screen
            if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                let is_server = config.lock().unwrap().desktop_env.starts_with("server");
                if is_server {
                    show_server_packages_selection(s);
                } else {
                    show_gpu_driver_selection(s);
                }
            }
        });
    
    siv.add_layer(dialog);
}

fn show_bootloader_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Select bootloader:"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut bootloader_select = SelectView::<&str>::new()
        .item("* U-Boot Mainline (Recommended)", "uboot-mainline")
        .item("  U-Boot Rockchip BSP", "uboot-rockchip")
        .item("  U-Boot Armbian", "uboot-armbian")
        .item("  EDK2 UEFI (Experimental)", "edk2-uefi")
        .item("  Tow-Boot (SPI Flash)", "tow-boot");
    
    bootloader_select.set_on_select(|s, bootloader| {
        update_bootloader_description(s, bootloader);
    });
    
    bootloader_select.set_selection(0);
    layout.add_child(bootloader_select.with_name("bootloader_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("bootloader_description"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Bootloader Selection")
        .button("Next", |s| {
            let selected_bootloader = s.call_on_name("bootloader_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(bootloader) = selected_bootloader {
                // Update build config
                if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                    let mut cfg = config.lock().unwrap();
                    cfg.bootloader = bootloader.to_string();
                }
                
                // Download bootloader files
                let download_manager = DownloadManager::new();
                if let Ok(manager) = download_manager {
                    let bootloader_version = match bootloader {
                        "uboot-mainline" => "2024.01",
                        "uboot-rockchip" => "2023.10-rk3588",
                        "uboot-armbian" => "2023.10-armbian",
                        "edk2-uefi" => "latest",
                        "tow-boot" => "2023.07",
                        _ => "latest",
                    };
                    
                    if let Err(e) = manager.download_uboot(bootloader_version) {
                        crate::ui::logger::log_error(&format!("Failed to download {} bootloader: {}", bootloader, e));
                    } else {
                        crate::ui::logger::log_info(&format!("Started downloading {} bootloader files", bootloader));
                    }
                }
                
                s.pop_layer();
                show_build_summary_final(s);
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_system_configuration(s);
        });
    
    siv.add_layer(dialog);
    update_bootloader_description(siv, &"uboot-mainline");
}

fn update_bootloader_description(siv: &mut Cursive, bootloader: &&str) {
    let description = match *bootloader {
        "uboot-mainline" => "Latest mainline U-Boot with active development.\nBest compatibility with standard Linux distributions.\nRecommended for most users.",
        "uboot-rockchip" => "Rockchip's BSP U-Boot with platform-specific optimizations.\nMay have better hardware support for some features.",
        "uboot-armbian" => "Armbian's patched U-Boot with community improvements.\nGood balance of features and stability.",
        "edk2-uefi" => "UEFI firmware implementation.\nProvides standard UEFI boot interface.\nExperimental on RK3588.",
        "tow-boot" => "U-Boot distribution for SPI flash.\nAllows booting from NVMe/USB without SD card.\nRequires SPI flash programming.",
        _ => "Unknown bootloader option.",
    };
    
    siv.call_on_name("bootloader_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_build_summary_final(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Build Configuration Summary:").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Get config and display summary
    if let Some(config) = siv.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
        let cfg = config.lock().unwrap();
        
        layout.add_child(TextView::new(format!("Distribution: {} {}", cfg.distro.to_uppercase(), cfg.distro_version)));
        layout.add_child(TextView::new(format!("Kernel: {} {}", cfg.kernel, cfg.kernel_version)));
        layout.add_child(TextView::new(format!("Desktop: {}", cfg.desktop_env)));
        if !cfg.desktop_env.starts_with("server") {
            layout.add_child(TextView::new(format!("GPU Driver: {}", cfg.gpu_driver)));
        }
        layout.add_child(TextView::new(format!("Bootloader: {}", cfg.bootloader)));
        layout.add_child(TextView::new(format!("Hostname: {}", cfg.hostname)));
        layout.add_child(TextView::new(format!("Username: {}", cfg.username)));
        layout.add_child(TextView::new(format!("Locale: {}", cfg.locale)));
        layout.add_child(TextView::new(format!("Timezone: {}", cfg.timezone)));
        
        if !cfg.packages.is_empty() {
            layout.add_child(DummyView.fixed_height(1));
            layout.add_child(TextView::new(format!("Additional packages: {}", cfg.packages.join(", "))));
        }
    }
    
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Image size: ~8GB"));
    layout.add_child(TextView::new("Build time: 30-60 minutes"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Build target selection
    layout.add_child(TextView::new("Build target:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let target_select = SelectView::<&str>::new()
        .item("Image file (.img)", "image")
        .item("Write directly to NVMe/eMMC (DANGEROUS)", "nvme")
        .with_name("build_target");
    layout.add_child(target_select);
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Ready to Build")
        .button("Start Build", |s| {
            let target = s.call_on_name("build_target", |view: &mut SelectView<&str>| {
                view.selection().map(|s| *s).unwrap_or("image")
            }).unwrap_or("image");
            
            if target == "nvme" {
                s.add_layer(
                    Dialog::text("⚠️ WARNING: Direct NVMe/eMMC Write\n\nThis will ERASE all data on the target device!\n\nAre you absolutely sure you want to continue?")
                        .title("Dangerous Operation")
                        .button("Cancel", |s| { s.pop_layer(); })
                        .button("Yes, write to device", |s| {
                            s.pop_layer();
                            s.pop_layer();
                            start_build_process(s, true);
                        })
                );
            } else {
                s.pop_layer();
                start_build_process(s, false);
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_bootloader_selection(s);
        })
        .button("Cancel", |s| {
            s.pop_layer();
            crate::ui::setup_main_menu(s);
        });
    
    siv.add_layer(dialog);
}

fn start_build_process(siv: &mut Cursive, write_to_device: bool) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Building Orange Pi 5 Plus Distribution").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Progress bar
    let mut progress = ProgressBar::new();
    progress.set_max(100);
    progress.set_value(0);
    layout.add_child(progress.with_name("build_progress").fixed_width(50));
    layout.add_child(DummyView.fixed_height(1));
    
    // Status text
    layout.add_child(TextView::new("Phase: Initializing...").with_name("build_phase"));
    layout.add_child(TextView::new("Status: Starting build process...").with_name("build_status"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Build log
    layout.add_child(TextView::new("Build Log:").style(ColorStyle::from(Color::Light(BaseColor::Yellow))));
    layout.add_child(TextView::new("").with_name("build_log").fixed_height(10));
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("Build in Progress")
        .button("Cancel", |s| {
            // TODO: Actually cancel the build
            s.add_layer(
                Dialog::text("Are you sure you want to cancel the build?")
                    .button("No", |s| { s.pop_layer(); })
                    .button("Yes", |s| {
                        s.pop_layer();
                        s.pop_layer();
                        crate::ui::setup_main_menu(s);
                    })
            );
        });
    
    siv.add_layer(dialog);
    
    // Get build config
    let config = if let Some(cfg) = siv.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
        cfg.clone()
    } else {
        return;
    };
    
    // Start actual build in background thread
    let cb_sink = siv.cb_sink().clone();
    thread::spawn(move || {
        execute_actual_build(config, write_to_device, cb_sink);
    });
}

fn execute_actual_build(config: Arc<Mutex<WizardBuildConfig>>, write_to_device: bool, cb_sink: cursive::CbSink) {
    let cfg = config.lock().unwrap().clone();
    
    // Check if this is a GameScope build
    if cfg.desktop_env == "lxqt-gamescope" || cfg.desktop_env == "gamescope-retroarch" {
        execute_gamescope_rust_build(cfg, write_to_device, cb_sink);
        return;
    }
    
    // TODO: Add support for other build types with Rust builders
    update_build_ui(&cb_sink, 0, "Error", "Build type not yet supported in Rust", "Only GameScope builds are currently supported");
    if let Some(logger) = crate::error::logging::get_global_logger() {
        let _ = logger.log_error(&crate::error::BuilderError::BuildFailed("Unsupported build type".to_string()), "WIZARD", "unsupported_build");
    }
}

fn execute_gamescope_rust_build(cfg: WizardBuildConfig, write_to_device: bool, cb_sink: cursive::CbSink) {
    use crate::builder::gamescope_builder::{GameScopeBuilder, GameScopeConfig, KernelChoice, DesktopChoice};
    use std::path::PathBuf;
    
    update_build_ui(&cb_sink, 10, "Starting", "Initializing GameScope builder...", "Setting up Rust builder");
    
    // Convert wizard config to GameScope config
    let kernel_choice = if cfg.kernel_version.contains("6.1") {
        KernelChoice::Rockchip61
    } else {
        KernelChoice::Rockchip51
    };
    
    let desktop_choice = match cfg.desktop_env.as_str() {
        "lxqt-gamescope" => DesktopChoice::LXQtWithGameScope,
        "gamescope-retroarch" => DesktopChoice::GameScopeRetroArch,
        _ => DesktopChoice::LXQtWithGameScope,
    };
    
    let output_path = if write_to_device {
        "/dev/nvme0n1".to_string()
    } else {
        let desktop_suffix = match desktop_choice {
            DesktopChoice::LXQtWithGameScope => "lxqt",
            DesktopChoice::GameScopeRetroArch => "retroarch",
        };
        format!("orangepi5plus-gamescope-{}-{}.img", desktop_suffix, cfg.distro_version)
    };
    
    let build_config = super::builder::BuildConfig {
        distro: "debian".to_string(),
        distro_version: "12".to_string(),
        kernel_version: match kernel_choice {
            KernelChoice::Rockchip51 => "5.1".to_string(),
            KernelChoice::Rockchip61 => "6.1".to_string(),
        },
        desktop_environment: Some(match desktop_choice {
            DesktopChoice::LXQtWithGameScope => "lxqt-gamescope".to_string(),
            DesktopChoice::GameScopeRetroArch => "gamescope-retroarch".to_string(),
        }),
        gpu_driver: Some("mali".to_string()),
        bootloader: "u-boot".to_string(),
        hostname: cfg.hostname,
        username: cfg.username,
        password: cfg.user_password,
        root_password: cfg.root_password,
        locale: cfg.locale,
        timezone: cfg.timezone,
        packages: vec![],
        image_size_gb: 8,
        output_path: output_path.clone(),
    };
    
    // Create and run the GameScope builder
    match GameScopeBuilder::new(build_config) {
        Ok(builder) => {
            update_build_ui(&cb_sink, 20, "Building", "Starting GameScope build process...", "Running Rust builder");
            
            // Run the build in a separate thread
            let cb_sink_clone = cb_sink.clone();
            thread::spawn(move || {
                match builder.build() {
                    Ok(()) => {
                        update_build_ui(&cb_sink_clone, 100, "Complete", "GameScope build completed successfully!", "Build finished");
                        
                        // Show completion dialog
                        cb_sink_clone.send(Box::new(move |s| {
                            s.pop_layer();
                            s.add_layer(
                                Dialog::text(format!(
                                    "GameScope build completed successfully!\n\n\
                                     Your Orange Pi 5 Plus GameScope image is ready.\n\
                                     Output: {}\n\n\
                                     The image includes:\n\
                                     • Debian 12.9 (Bookworm) base system\n\
                                     • GameScope compositor for Steam Deck-like gaming\n\
                                     • Optimized Mali G610 GPU drivers\n\
                                     • Gaming performance optimizations\n\
                                     • Custom kernel for Orange Pi 5 Plus",
                                    if write_to_device { "Written to device" } else { &output_path }
                                ))
                                .title("Build Complete")
                                .button("OK", |s| {
                                    s.quit();
                                })
                            );
                        })).unwrap();
                    },
                    Err(e) => {
                        update_build_ui(&cb_sink_clone, 0, "Error", "GameScope build failed", &format!("Error: {}", e));
                        if let Some(logger) = crate::error::logging::get_global_logger() {
                            let _ = logger.log_error(&e, "GAMESCOPE_BUILDER", "build_execution");
                        }
                        
                        // Show error dialog
                        cb_sink_clone.send(Box::new(move |s| {
                            s.pop_layer();
                            s.add_layer(
                                Dialog::text(format!("GameScope build failed:\n\n{}", e))
                                .title("Build Error")
                                .button("OK", |s| {
                                    s.quit();
                                })
                            );
                        })).unwrap();
                    }
                }
            });
        },
        Err(e) => {
            update_build_ui(&cb_sink, 0, "Error", "Failed to initialize GameScope builder", &format!("Error: {}", e));
            if let Some(logger) = crate::error::logging::get_global_logger() {
                let _ = logger.log_error(&e, "GAMESCOPE_BUILDER", "initialization");
            }
        }
    }
}


fn update_build_ui(cb_sink: &cursive::CbSink, progress: usize, phase: &str, status: &str, log_entry: &str) {
    let phase = phase.to_string();
    let status = status.to_string();
    let log_entry = log_entry.to_string();
    
    cb_sink.send(Box::new(move |s| {
        s.call_on_name("build_progress", |view: &mut ProgressBar| {
            view.set_value(progress);
        });
        
        s.call_on_name("build_phase", |view: &mut TextView| {
            view.set_content(format!("Phase: {}", phase));
        });
        
        s.call_on_name("build_status", |view: &mut TextView| {
            view.set_content(format!("Status: {}", status));
        });
        
        s.call_on_name("build_log", |view: &mut TextView| {
            let current = view.get_content();
            let new_content = format!("{}{}\n", current.source(), log_entry);
            let lines: Vec<&str> = new_content.lines().collect();
            let start = if lines.len() > 10 { lines.len() - 10 } else { 0 };
            let display = lines[start..].join("\n");
            view.set_content(display);
        });
    })).unwrap();
}

// Emulation selection functions
fn show_emulation_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🎮 Emulation Build Selection").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut emulation_select = SelectView::<&str>::new()
        .item("* Setec's GameScope-Pi v1 (Recommended for Gaming)", "gamescope-pi")
        .item("  Setec's OpenScope-Pi v1 (Coming Soon)", "openscope-pi")
        .item("  RetroArch - Multi-system emulator (Coming Soon)", "retroarch")
        .item("  Lakka - Lightweight retro gaming (Coming Soon)", "lakka")
        .item("  EmulationStation - Frontend for emulators (Coming Soon)", "emulationstation")
        .item("  Batocera - All-in-one retro gaming (Coming Soon)", "batocera")
        .item("  RetroPie - Classic emulation suite (Coming Soon)", "retropie");
    
    emulation_select.set_on_select(|s, emulation| {
        update_emulation_description(s, emulation);
    });
    
    emulation_select.set_selection(0);
    
    layout.add_child(emulation_select.with_name("emulation_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("emulation_description"));
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("Emulation Build Selection")
        .button("Next", |s| {
            let selected = s.call_on_name("emulation_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(emulation) = selected {
                if emulation != "gamescope-pi" {
                    s.add_layer(
                        Dialog::text("This option is coming soon!\n\nPlease select GameScope-Pi for now.")
                            .title("Coming Soon")
                            .button("OK", |s| { s.pop_layer(); })
                    );
                    return;
                }
                
                // Show desktop environment selection for GameScope
                s.pop_layer();
                show_gamescope_desktop_selection(s);
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_category_selection(s);
        });
    
    siv.add_layer(dialog);
    update_emulation_description(siv, &"gamescope-pi");
}

fn show_gamescope_desktop_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🎮 GameScope Desktop Environment Selection").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Choose your GameScope configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut desktop_select = SelectView::<&str>::new()
        .item("* LXQt + GameScope - Full desktop with Valve's GameScope compositor", "lxqt-gamescope")
        .item("  GameScope + RetroArch - Boot directly into RetroArch with GameScope", "gamescope-retroarch");
    
    desktop_select.set_on_select(|s, desktop| {
        let description = match *desktop {
            "lxqt-gamescope" => "Full LXQt desktop environment with Valve's GameScope compositor.\nProvides a complete desktop experience with gaming optimizations.\nYou can launch GameScope sessions from within LXQt.",
            "gamescope-retroarch" => "Minimal system that boots directly into RetroArch using GameScope.\nSteam Deck-like experience for retro gaming.\nNo desktop environment - pure gaming console mode.",
            _ => "",
        };
        
        s.call_on_name("desktop_description", |view: &mut TextView| {
            view.set_content(description);
        });
    });
    
    desktop_select.set_selection(0);
    
    // Trigger initial description before moving the select view
    let initial_description = "Full LXQt desktop environment with Valve's GameScope compositor.\nProvides a complete desktop experience with gaming optimizations.\nYou can launch GameScope sessions from within LXQt.";
    
    layout.add_child(desktop_select.with_name("desktop_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new(initial_description).with_name("desktop_description"));
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("GameScope Desktop Configuration")
        .button("Next", |s| {
            let selected_desktop = s.call_on_name("desktop_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten().unwrap_or("lxqt-gamescope");
            
            // Update build config
            if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                let mut cfg = config.lock().unwrap();
                cfg.build_type = BuildType::GameScopePi;
                cfg.distro = "debian".to_string();
                cfg.distro_version = "12.11".to_string();
                cfg.kernel = "rockchip".to_string();
                cfg.kernel_version = "6.1".to_string();
                cfg.desktop_env = selected_desktop.to_string();
                cfg.gpu_driver = "mali-g610-valhall-g13p0".to_string();
                cfg.packages = vec!["gamescope", "steam", "mangohud", "gamemode"].iter().map(|s| s.to_string()).collect();
            }
            
            s.pop_layer();
            show_gamescope_prebuild_menu(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_emulation_selection(s);
        });
    
    siv.add_layer(dialog);
}

fn show_gamescope_prebuild_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    // Title
    layout.add_child(TextView::new("🎮 GameScope-Pi Build Configuration").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Build configuration display
    layout.add_child(TextView::new("Build Configuration:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    // Configuration details
    let desktop_text = if let Some(config) = siv.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
        let cfg = config.lock().unwrap();
        match cfg.desktop_env.as_str() {
            "lxqt-gamescope" => "🖥️ Desktop: LXQt with GameScope",
            "gamescope-retroarch" => "🖥️ Desktop: GameScope + RetroArch (Console Mode)",
            _ => "🖥️ Desktop: LXQt with GameScope",
        }
    } else {
        "🖥️ Desktop: LXQt with GameScope"
    };
    
    let config_text = LinearLayout::vertical()
        .child(TextView::new("📦 Distribution: Debian 12.11"))
        .child(TextView::new("🐧 Kernel: Rockchip 6.1"))
        .child(TextView::new(desktop_text))
        .child(TextView::new("🎮 GPU: Mali-G610 Valhall drivers (g13p0)"))
        .child(TextView::new("💾 Storage: 16GB minimum recommended"));
    
    layout.add_child(config_text);
    layout.add_child(DummyView.fixed_height(1));
    
    // Description box
    let description = "This will build a custom Debian image with:\n\
                      • Optimized Rockchip 6.1 kernel\n\
                      • Mali-G610 Valhall drivers (g13p0)\n\
                      • Gamescope compositor with Mali optimizations\n\
                      • Automatic 1000MHz GPU frequency\n\
                      • 512MB CMA allocation\n\
                      • Performance governors enabled\n\
                      • Direct boot to gaming interface";
    
    layout.add_child(TextView::new("Description:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(TextView::new(description));
    layout.add_child(DummyView.fixed_height(1));
    
    // Output selection
    layout.add_child(TextView::new("Output Type:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let mut output_select = SelectView::<&str>::new()
        .item("Create IMG File", "img")
        .item("Write to eMMC/NVMe", "device");
    
    output_select.set_selection(0);
    layout.add_child(output_select.with_name("output_type"));
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("GameScope-Pi Build Configuration")
        .button("Start Build", |s| {
            let output_type = s.call_on_name("output_type", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten().unwrap_or("img");
            
            // Update configuration with output type
            if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                let mut cfg = config.lock().unwrap();
                cfg.hostname = "gamescope-pi".to_string();
                cfg.username = "gamer".to_string();
                cfg.user_password = "gamescope".to_string();
                cfg.root_password = "gamescope".to_string();
                cfg.locale = "en_US.UTF-8".to_string();
                cfg.timezone = "UTC".to_string();
            }
            
            s.pop_layer();
            
            // Start the build process with Python script
            start_gamescope_build(s, output_type);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_emulation_selection(s);
        });
    
    siv.add_layer(dialog);
}

fn start_gamescope_build(siv: &mut Cursive, output_type: &str) {
    
    // Create build progress dialog
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🎮 Building GameScope-Pi Image").style(ColorStyle::from(Color::Light(BaseColor::Green))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut progress = ProgressBar::new();
    progress.set_max(100);
    progress.set_value(0);
    
    layout.add_child(progress.with_name("build_progress"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Phase: Initializing...").with_name("build_phase"));
    layout.add_child(TextView::new("Status: Starting build process...").with_name("build_status"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Build log
    layout.add_child(TextView::new("Build Log:"));
    let mut log_view = LinearLayout::vertical();
    log_view.add_child(TextView::new(""));
    let log_scroll = log_view.scrollable().with_name("build_log").fixed_height(10);
    layout.add_child(log_scroll);
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("GameScope-Pi Build Progress");
    
    siv.add_layer(dialog);
    
    // Get cursive callback sink
    let cb_sink = siv.cb_sink().clone();
    
    // Get configuration
    let config = if let Some(config) = siv.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
        config.clone()
    } else {
        return;
    };
    
    let output_type = output_type.to_string();
    
    // Run build in background thread
    thread::spawn(move || {
        // Create config file for Python script
        let build_config = format!(r#"{{
    "distro": "debian",
    "version": "12.9",
    "codename": "bookworm",
    "arch": "arm64",
    "kernel_version": "6.1",
    "gpu_driver": "mali-g610-g13p0",
    "desktop": "lxqt",
    "hostname": "gamescope-pi",
    "username": "gamer",
    "password": "gamescope",
    "root_password": "gamescope",
    "locale": "en_US.UTF-8",
    "timezone": "UTC",
    "image_size": "16G",
    "output_format": "{}"
}}"#, output_type);
        
        // Write config to temp file
        let config_path = "build/gamescope_config.json";
        std::fs::create_dir_all("build").ok();
        if let Ok(mut file) = std::fs::File::create(config_path) {
            let _ = write!(file, "{}", build_config);
        }
        
        // Update UI
        update_build_ui(&cb_sink, 10, "Preparation", "Starting GameScope builder...", "Preparing build environment");
        thread::sleep(Duration::from_secs(1));
        
        // Use Rust GameScope builder
        use crate::builder::gamescope_builder::{GameScopeBuilder, GameScopeConfig, KernelChoice, DesktopChoice};
        
        let build_config = super::builder::BuildConfig {
            distro: "debian".to_string(),
            distro_version: "12".to_string(),
            kernel_version: "6.1".to_string(),
            desktop_environment: Some("lxqt-gamescope".to_string()),
            gpu_driver: Some("mali".to_string()),
            bootloader: "u-boot".to_string(),
            hostname: "gamescope-pi".to_string(),
            username: "gamer".to_string(),
            password: "gamescope".to_string(),
            root_password: "gamescope".to_string(),
            locale: "en_US.UTF-8".to_string(),
            timezone: "UTC".to_string(),
            packages: vec![],
            image_size_gb: 8,
            output_path: if output_type == "device" { "/dev/mmcblk0".to_string() } else { "output/gamescope.img".to_string() },
        };
        
        let builder_result = GameScopeBuilder::new(build_config).and_then(|builder| {
            update_build_ui(&cb_sink, 50, "Building", "Executing GameScope build...", "Running build process");
            builder.build()
        });
            
        match builder_result {
            Ok(()) => {
                update_build_ui(&cb_sink, 100, "Complete", "GameScope build completed successfully!", "Build finished");
                if let Some(logger) = crate::error::logging::get_global_logger() {
                    let _ = logger.log_info("GameScope build completed successfully", "GAMESCOPE_BUILDER", "build_success");
                }
                
                // Show completion dialog
                cb_sink.send(Box::new(move |s| {
                    s.pop_layer();
                    s.add_layer(
                        Dialog::text(format!(
                            "GameScope-Pi build completed successfully!\n\n\
                            The image has been created in the output directory.\n\n\
                            You can now write it to an SD card or eMMC."
                        ))
                        .title("Build Complete")
                        .button("OK", |s| {
                            s.pop_layer();
                            crate::ui::setup_main_menu(s);
                        })
                    );
                })).unwrap();
            },
            Err(e) => {
                update_build_ui(&cb_sink, 0, "Error", "GameScope build failed", &format!("Error: {}", e));
                if let Some(logger) = crate::error::logging::get_global_logger() {
                    let _ = logger.log_error(&e, "GAMESCOPE_BUILDER", "build_failed");
                }
                
                // Show error dialog
                cb_sink.send(Box::new(move |s| {
                    s.pop_layer();
                    s.add_layer(
                        Dialog::text(format!("GameScope build failed:\n\n{}\n\nPlease check the logs for details.", e))
                            .title("Build Failed")
                            .button("OK", |s| {
                                s.pop_layer();
                                crate::ui::setup_main_menu(s);
                            })
                    );
                })).unwrap();
            }
        }
    });
}

fn update_emulation_description(siv: &mut Cursive, emulation: &&str) {
    let description = match *emulation {
        "gamescope-pi" => "Setec's GameScope-Pi v1 - Steam Deck-like experience for Orange Pi.\nIncludes GameScope compositor, Steam integration, and optimized gaming performance.",
        "openscope-pi" => "Setec's OpenScope-Pi v1 - Open source gaming distribution.\nCommunity-driven with Panfork GPU drivers and extensive emulator support.",
        "retroarch" => "RetroArch - Powerful multi-system emulator with advanced features.\nSupports shaders, netplay, achievements, and more.",
        "lakka" => "Lakka - Lightweight Linux distribution for retro gaming.\nBased on LibreELEC with minimal overhead.",
        "emulationstation" => "EmulationStation - Clean frontend for organizing and launching games.\nHighly customizable with theme support.",
        "batocera" => "Batocera - All-in-one retro gaming distribution.\nAutomatic configuration for controllers and systems.",
        "retropie" => "RetroPie - Classic emulation suite originally for Raspberry Pi.\nExtensive documentation and community support.",
        _ => "Unknown emulation option.",
    };
    
    siv.call_on_name("emulation_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_gaming_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Gaming Build Configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    
    // GPU overclocking
    layout.add_child(TextView::new("GPU Configuration:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let gpu_oc = Checkbox::new()
        .checked()
        .with_name("gpu_overclock");
    layout.add_child(LinearLayout::horizontal()
        .child(gpu_oc)
        .child(TextView::new(" Enable GPU overclocking (1.2GHz)")));
    
    // CPU governor
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("CPU Governor:"));
    let cpu_select = SelectView::<&str>::new()
        .item("Performance (Maximum speed)", "performance")
        .item("Ondemand (Balanced)", "ondemand")
        .item("Schedutil (Efficient)", "schedutil")
        .with_name("cpu_governor");
    layout.add_child(cpu_select);
    
    // Storage optimization
    layout.add_child(DummyView.fixed_height(1));
    let storage_opt = Checkbox::new()
        .checked()
        .with_name("storage_optimize");
    layout.add_child(LinearLayout::horizontal()
        .child(storage_opt)
        .child(TextView::new(" Optimize storage for gaming (F2FS)")));
    
    // Low latency mode
    let low_latency = Checkbox::new()
        .checked()
        .with_name("low_latency");
    layout.add_child(LinearLayout::horizontal()
        .child(low_latency)
        .child(TextView::new(" Enable low-latency kernel mode")));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Gaming Optimization")
        .button("Next", |s| {
            // Collect settings and update config
            s.pop_layer();
            show_system_configuration(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_emulation_selection(s);
        });
    
    siv.add_layer(dialog);
}

fn show_emulation_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Emulation Configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Emulator selection
    layout.add_child(TextView::new("Select emulators to include:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    layout.add_child(DummyView.fixed_height(1));
    
    let emulators = vec![
        ("snes", "SNES - Super Nintendo"),
        ("nes", "NES - Nintendo"),
        ("genesis", "Genesis/Mega Drive"),
        ("gba", "GBA - Game Boy Advance"),
        ("nds", "NDS - Nintendo DS"),
        ("psx", "PSX - PlayStation 1"),
        ("ps2", "PS2 - PlayStation 2"),
        ("psp", "PSP - PlayStation Portable"),
        ("n64", "N64 - Nintendo 64"),
        ("gamecube", "GameCube/Wii"),
        ("dreamcast", "Dreamcast"),
        ("arcade", "Arcade (MAME)"),
    ];
    
    for (emu, desc) in emulators {
        let checkbox = Checkbox::new()
            .checked()
            .with_name(format!("emu_{}", emu));
        layout.add_child(LinearLayout::horizontal()
            .child(checkbox)
            .child(TextView::new(format!(" {}", desc))));
    }
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Emulator Selection")
        .button("Next", |s| {
            s.pop_layer();
            show_system_configuration(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_emulation_selection(s);
        });
    
    siv.add_layer(dialog);
}

// Media center selection functions
fn show_media_center_selection(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("🎬 Media Center Build Selection").style(ColorStyle::from(Color::Light(BaseColor::Cyan))));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut media_select = SelectView::<&str>::new()
        .item("* Kodi Media Center (Recommended)", "kodi")
        .item("  Kodi + Netflix/Prime plugins", "kodi-streaming")
        .item("  Plex Media Server", "plex")
        .item("  Jellyfin Media Server", "jellyfin")
        .item("  LibreELEC (Kodi appliance)", "libreelec");
    
    media_select.set_on_select(|s, media| {
        update_media_description(s, media);
    });
    
    media_select.set_selection(0);
    
    layout.add_child(media_select.with_name("media_select"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("").with_name("media_description"));
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("Media Center Build Selection")
        .button("Configure", |s| {
            let selected = s.call_on_name("media_select", |view: &mut SelectView<&str>| {
                view.selection().map(|sel| *sel)
            }).flatten();
            
            if let Some(media) = selected {
                // Update build config
                if let Some(config) = s.user_data::<Arc<Mutex<WizardBuildConfig>>>() {
                    let mut cfg = config.lock().unwrap();
                    cfg.build_type = BuildType::KodiMediaCenter;
                    cfg.desktop_env = media.to_string();
                }
                
                s.pop_layer();
                show_media_configuration(s);
            }
        })
        .button("Back", |s| {
            s.pop_layer();
            show_category_selection(s);
        });
    
    siv.add_layer(dialog);
    update_media_description(siv, &"kodi");
}

fn update_media_description(siv: &mut Cursive, media: &&str) {
    let description = match *media {
        "kodi" => "Kodi Media Center - Powerful media player with extensive codec support.\nHardware accelerated video playback for 4K content.",
        "kodi-streaming" => "Kodi with streaming service plugins pre-installed.\nIncludes Netflix, Amazon Prime, Disney+ support.",
        "plex" => "Plex Media Server - Stream your media library anywhere.\nTranscoding support for multiple devices.",
        "jellyfin" => "Jellyfin - Free and open source media server.\nNo subscription required, fully self-hosted.",
        "libreelec" => "LibreELEC - Just enough OS for Kodi.\nMinimal Linux that boots directly to Kodi.",
        _ => "Unknown media center option.",
    };
    
    siv.call_on_name("media_description", |view: &mut TextView| {
        view.set_content(description);
    });
}

fn show_media_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Media Center Configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    
    // Video settings
    layout.add_child(TextView::new("Video Settings:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let hdr = Checkbox::new()
        .checked()
        .with_name("enable_hdr");
    layout.add_child(LinearLayout::horizontal()
        .child(hdr)
        .child(TextView::new(" Enable HDR10 support")));
    
    let hevc = Checkbox::new()
        .checked()
        .with_name("enable_hevc");
    layout.add_child(LinearLayout::horizontal()
        .child(hevc)
        .child(TextView::new(" Enable HEVC/H.265 hardware decoding")));
    
    let av1 = Checkbox::new()
        .checked()
        .with_name("enable_av1");
    layout.add_child(LinearLayout::horizontal()
        .child(av1)
        .child(TextView::new(" Enable AV1 hardware decoding")));
    
    // Audio settings
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Audio Settings:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let passthrough = Checkbox::new()
        .checked()
        .with_name("audio_passthrough");
    layout.add_child(LinearLayout::horizontal()
        .child(passthrough)
        .child(TextView::new(" Enable audio passthrough (DTS, Dolby)")));
    
    // Network settings
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Network Shares:").style(ColorStyle::from(Color::Light(BaseColor::Blue))));
    let smb = Checkbox::new()
        .checked()
        .with_name("enable_smb");
    layout.add_child(LinearLayout::horizontal()
        .child(smb)
        .child(TextView::new(" Enable SMB/CIFS network shares")));
    
    let nfs = Checkbox::new()
        .with_name("enable_nfs");
    layout.add_child(LinearLayout::horizontal()
        .child(nfs)
        .child(TextView::new(" Enable NFS network shares")));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Media Center Options")
        .button("Next", |s| {
            s.pop_layer();
            show_media_storage_config(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_media_center_selection(s);
        });
    
    siv.add_layer(dialog);
}

fn show_media_storage_config(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Storage Configuration:"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Media Library Location:"));
    let storage_select = SelectView::<&str>::new()
        .item("Internal storage (/media)", "internal")
        .item("External USB drive", "usb")
        .item("Network share (NAS)", "network")
        .item("Multiple locations", "multiple")
        .with_name("storage_location");
    layout.add_child(storage_select);
    
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Database:"));
    let db_select = SelectView::<&str>::new()
        .item("Local SQLite (Default)", "sqlite")
        .item("MySQL/MariaDB (Shared library)", "mysql")
        .with_name("database_type");
    layout.add_child(db_select);
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Storage Configuration")
        .button("Next", |s| {
            s.pop_layer();
            show_system_configuration(s);
        })
        .button("Back", |s| {
            s.pop_layer();
            show_media_configuration(s);
        });
    
    siv.add_layer(dialog);
}

// Kernel info struct
#[derive(Debug, Clone)]
struct KernelInfo {
    name: String,
    repo_url: String,
    branch: String,
    download_path: String,
}

fn get_kernel_info(kernel: &str) -> KernelInfo {
    match kernel {
        "joshua-riek" => KernelInfo {
            name: "Joshua-Riek Ubuntu Kernel".to_string(),
            repo_url: "https://github.com/Joshua-Riek/linux-rockchip.git".to_string(),
            branch: "ubuntu-rockchip".to_string(),
            download_path: "/kernel/joshua-riek".to_string(),
        },
        "mainline" => KernelInfo {
            name: "Linux Mainline Kernel".to_string(),
            repo_url: "https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git".to_string(),
            branch: "master".to_string(),
            download_path: "/kernel/mainline".to_string(),
        },
        "armbian" => KernelInfo {
            name: "Armbian Kernel".to_string(),
            repo_url: "https://github.com/armbian/linux.git".to_string(),
            branch: "main".to_string(),
            download_path: "/kernel/armbian".to_string(),
        },
        _ => KernelInfo {
            name: "Unknown Kernel".to_string(),
            repo_url: "".to_string(),
            branch: "".to_string(),
            download_path: "/kernel/unknown".to_string(),
        },
    }
}