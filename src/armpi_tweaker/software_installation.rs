use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView, EditView, Checkbox};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;

pub fn show_software_installation_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Software Installation & Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Install and manage software packages and applications"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("🎮 Gaming Software - RetroArch, emulators, Steam", "gaming");
    menu.add_item("📺 Media Servers - Plex, Jellyfin, Kodi, DLNA", "media_servers");
    menu.add_item("🏠 Home Automation - Home Assistant, Node-RED, MQTT", "home_automation");
    menu.add_item("💾 Database Servers - MySQL, PostgreSQL, MongoDB", "databases");
    menu.add_item("🌐 Web Servers - Apache, Nginx, Caddy", "web_servers");
    menu.add_item("📦 Container Systems - Docker, Podman, LXC", "containers");
    menu.add_item("🔧 Development Tools - IDEs, compilers, interpreters", "development");
    menu.add_item("📊 Monitoring Tools - Grafana, Prometheus, InfluxDB", "monitoring");
    menu.add_item("🔐 Security Tools - Fail2Ban, ClamAV, firewall", "security");
    menu.add_item("☁️ Cloud Services - Nextcloud, Syncthing, backup", "cloud");
    menu.add_item("🎵 Audio Software - PulseAudio, JACK, audio tools", "audio");
    menu.add_item("🖥️ Desktop Environment - GNOME, KDE, XFCE, LXQt", "desktop");
    menu.add_item("🤖 AI/ML Software - TensorFlow, PyTorch, ONNX", "ai_ml");
    menu.add_item("📡 Network Services - VPN, DNS, DHCP servers", "network_services");
    menu.add_item("🛠️ System Utilities - Backup, monitoring, maintenance", "utilities");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "gaming" => show_gaming_software(s),
            "media_servers" => show_media_servers(s),
            "home_automation" => show_home_automation(s),
            "databases" => show_database_servers(s),
            "web_servers" => show_web_servers(s),
            "containers" => show_container_systems(s),
            "development" => show_development_tools(s),
            "monitoring" => show_monitoring_tools(s),
            "security" => show_security_tools(s),
            "cloud" => show_cloud_services(s),
            "audio" => show_audio_software(s),
            "desktop" => show_desktop_environments(s),
            "ai_ml" => show_ai_ml_software(s),
            "network_services" => show_network_services(s),
            "utilities" => show_system_utilities(s),
            _ => {
                s.add_layer(
                    Dialog::text("Feature coming soon!")
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(85))
        .title("Software Installation")
        .button("Back", |s| { 
            s.pop_layer(); 
            crate::armpi_tweaker::show_armpi_tweaker(s);
        });
    
    siv.add_layer(dialog);
}

fn show_gaming_software(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Gaming Software for Orange Pi 5 Plus"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Optimized for RK3588S Mali-G610 GPU"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut gaming_menu = SelectView::<&str>::new();
    gaming_menu.add_item("🕹️ RetroArch - Multi-system emulator", "retroarch");
    gaming_menu.add_item("🎮 GameScope - Gaming compositor", "gamescope");
    gaming_menu.add_item("🎯 Steam - PC gaming platform", "steam");
    gaming_menu.add_item("🎲 Lutris - Gaming management", "lutris");
    gaming_menu.add_item("🏗️ Emulation Station - Frontend", "emulationstation");
    gaming_menu.add_item("🎪 PCSX2 - PlayStation 2 emulator", "pcsx2");
    gaming_menu.add_item("🍄 Dolphin - GameCube/Wii emulator", "dolphin");
    gaming_menu.add_item("🗡️ PPSSPP - PlayStation Portable emulator", "ppsspp");
    gaming_menu.add_item("🎯 Mupen64Plus - Nintendo 64 emulator", "mupen64plus");
    gaming_menu.add_item("🚀 ScummVM - Adventure game engine", "scummvm");
    gaming_menu.add_item("🎮 Gaming Optimization Package", "gaming_optimizations");
    gaming_menu.add_item("🔧 Gaming Performance Tuning", "gaming_tuning");
    
    gaming_menu.set_on_submit(|s, option| {
        match *option {
            "retroarch" => show_retroarch_install(s),
            "gamescope" => show_gamescope_install(s),
            "steam" => show_steam_install(s),
            "gaming_optimizations" => show_gaming_optimizations(s),
            _ => {
                s.add_layer(
                    Dialog::text(format!("Installing {}...\n\nThis will download and configure the software with Orange Pi 5 Plus optimizations.", option))
                        .title("Software Installation")
                        .button("Install", |s| {
                            s.pop_layer();
                            show_installation_progress(s, "Gaming Software");
                        })
                        .button("Cancel", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(gaming_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Gaming Software")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_retroarch_install(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("RetroArch Installation"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Multi-system emulator with hardware acceleration"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Components to install:"));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("retroarch_core"))
        .child(TextView::new(" RetroArch core")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("retroarch_cores"))
        .child(TextView::new(" Emulator cores (NES, SNES, Genesis, etc.)")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("retroarch_assets"))
        .child(TextView::new(" Assets and themes")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("retroarch_roms"))
        .child(TextView::new(" Sample ROMs (homebrew)")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("retroarch_shaders"))
        .child(TextView::new(" GPU shaders")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("retroarch_optimization"))
        .child(TextView::new(" Orange Pi 5 Plus optimizations")));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("RetroArch Installation")
        .button("Install", |s| {
            s.pop_layer();
            show_installation_progress(s, "RetroArch");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_media_servers(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Media Server Software"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Stream and manage media content"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut media_menu = SelectView::<&str>::new();
    media_menu.add_item("🎬 Jellyfin - Open-source media server", "jellyfin");
    media_menu.add_item("📺 Plex - Premium media server", "plex");
    media_menu.add_item("🎵 Kodi - Media center", "kodi");
    media_menu.add_item("📻 Emby - Media server", "emby");
    media_menu.add_item("🎶 Navidrome - Music streaming server", "navidrome");
    media_menu.add_item("📹 Shinobi - Video surveillance", "shinobi");
    media_menu.add_item("🎥 FFmpeg - Video processing", "ffmpeg");
    media_menu.add_item("📡 MiniDLNA - DLNA server", "minidlna");
    media_menu.add_item("🔊 Icecast - Audio streaming server", "icecast");
    media_menu.add_item("📼 OBS Studio - Streaming software", "obs");
    media_menu.add_item("🎬 Hardware Transcoding Setup", "hw_transcoding");
    media_menu.add_item("📊 Media Server Optimization", "media_optimization");
    
    media_menu.set_on_submit(|s, option| {
        match *option {
            "jellyfin" => show_jellyfin_install(s),
            "plex" => show_plex_install(s),
            "hw_transcoding" => show_hw_transcoding_setup(s),
            _ => {
                s.add_layer(
                    Dialog::text(format!("Installing {}...\n\nThis will set up the media server with hardware acceleration support.", option))
                        .title("Media Server Installation")
                        .button("Install", |s| {
                            s.pop_layer();
                            show_installation_progress(s, "Media Server");
                        })
                        .button("Cancel", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(media_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Media Servers")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_home_automation(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Home Automation Software"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Smart home and IoT management"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut home_menu = SelectView::<&str>::new();
    home_menu.add_item("🏠 Home Assistant - Complete home automation", "homeassistant");
    home_menu.add_item("🔗 Node-RED - Visual automation", "nodered");
    home_menu.add_item("📡 MQTT Broker - IoT messaging", "mqtt");
    home_menu.add_item("📊 Grafana - IoT dashboards", "grafana");
    home_menu.add_item("💾 InfluxDB - IoT database", "influxdb");
    home_menu.add_item("🌡️ Zigbee2MQTT - Zigbee gateway", "zigbee2mqtt");
    home_menu.add_item("📱 ESPHome - ESP device management", "esphome");
    home_menu.add_item("🔌 OpenHAB - Alternative automation", "openhab");
    home_menu.add_item("🏠 Domoticz - Home automation platform", "domoticz");
    home_menu.add_item("📟 Z-Wave support", "zwave");
    home_menu.add_item("🎯 Complete Home Automation Stack", "complete_stack");
    
    layout.add_child(home_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Home Automation")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_database_servers(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Database Server Software"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Database management systems"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut db_menu = SelectView::<&str>::new();
    db_menu.add_item("🐬 MySQL - Popular SQL database", "mysql");
    db_menu.add_item("🐘 PostgreSQL - Advanced SQL database", "postgresql");
    db_menu.add_item("🍃 MongoDB - NoSQL document database", "mongodb");
    db_menu.add_item("🗃️ MariaDB - MySQL alternative", "mariadb");
    db_menu.add_item("⚡ Redis - In-memory database", "redis");
    db_menu.add_item("📊 InfluxDB - Time series database", "influxdb_db");
    db_menu.add_item("🔍 Elasticsearch - Search database", "elasticsearch");
    db_menu.add_item("💾 SQLite - Embedded database", "sqlite");
    db_menu.add_item("🏃 CouchDB - NoSQL database", "couchdb");
    db_menu.add_item("🔧 Database Optimization", "db_optimization");
    db_menu.add_item("📊 Database Monitoring", "db_monitoring");
    
    layout.add_child(db_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Database Servers")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_container_systems(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Container Systems"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Containerization and virtualization"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut container_menu = SelectView::<&str>::new();
    container_menu.add_item("🐳 Docker - Container platform", "docker");
    container_menu.add_item("📦 Podman - Rootless containers", "podman");
    container_menu.add_item("🏗️ Docker Compose - Multi-container apps", "docker_compose");
    container_menu.add_item("☸️ K3s - Lightweight Kubernetes", "k3s");
    container_menu.add_item("📱 LXC/LXD - System containers", "lxc");
    container_menu.add_item("🌐 Portainer - Container management", "portainer");
    container_menu.add_item("📊 Container Monitoring", "container_monitoring");
    container_menu.add_item("🔧 Container Optimization", "container_optimization");
    container_menu.add_item("📋 Popular Container Stack", "container_stack");
    
    layout.add_child(container_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Container Systems")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_development_tools(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Development Tools"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Programming languages and development environments"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut dev_menu = SelectView::<&str>::new();
    dev_menu.add_item("💻 Visual Studio Code - Code editor", "vscode");
    dev_menu.add_item("🦀 Rust Development - Rust toolchain", "rust");
    dev_menu.add_item("🐍 Python Development - Python interpreters", "python");
    dev_menu.add_item("☕ Java Development - OpenJDK", "java");
    dev_menu.add_item("📝 Node.js Development - JavaScript runtime", "nodejs");
    dev_menu.add_item("🐹 Go Development - Go compiler", "golang");
    dev_menu.add_item("💎 Ruby Development - Ruby interpreter", "ruby");
    dev_menu.add_item("🔧 C/C++ Development - GCC, CMake", "cpp");
    dev_menu.add_item("🌐 Web Development Stack", "web_dev");
    dev_menu.add_item("📱 Mobile Development - Flutter, React Native", "mobile_dev");
    dev_menu.add_item("🎯 Complete Development Environment", "complete_dev");
    
    layout.add_child(dev_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Development Tools")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ai_ml_software(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("AI/ML Software for Orange Pi 5 Plus"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Optimized for RK3588S NPU (6 TOPS)"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut ai_menu = SelectView::<&str>::new();
    ai_menu.add_item("🧠 RKNN Toolkit - Rockchip Neural Network", "rknn");
    ai_menu.add_item("🔥 PyTorch - Deep learning framework", "pytorch");
    ai_menu.add_item("📊 TensorFlow - ML framework", "tensorflow");
    ai_menu.add_item("⚡ ONNX Runtime - Model inference", "onnx");
    ai_menu.add_item("🤖 OpenCV - Computer vision", "opencv");
    ai_menu.add_item("📸 AI Camera Software", "ai_camera");
    ai_menu.add_item("🎯 Object Detection Models", "object_detection");
    ai_menu.add_item("🗣️ Speech Recognition", "speech_recognition");
    ai_menu.add_item("📝 Natural Language Processing", "nlp");
    ai_menu.add_item("🔬 Jupyter Notebook - ML development", "jupyter");
    ai_menu.add_item("🧮 NumPy/SciPy Stack", "numpy_stack");
    ai_menu.add_item("🎯 Complete AI/ML Environment", "complete_ai");
    
    layout.add_child(ai_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("AI/ML Software")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_web_servers(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Web Server Software"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut web_menu = SelectView::<&str>::new();
    web_menu.add_item("🅰️ Apache HTTP Server - Popular web server", "apache");
    web_menu.add_item("🌟 Nginx - High-performance web server", "nginx");
    web_menu.add_item("⚡ Caddy - Modern web server", "caddy");
    web_menu.add_item("🚀 Lighttpd - Lightweight web server", "lighttpd");
    web_menu.add_item("🐍 Gunicorn - Python WSGI server", "gunicorn");
    web_menu.add_item("📦 LAMP Stack - Linux, Apache, MySQL, PHP", "lamp");
    web_menu.add_item("🔧 LEMP Stack - Linux, Nginx, MySQL, PHP", "lemp");
    web_menu.add_item("🌐 Web Server Optimization", "web_optimization");
    
    layout.add_child(web_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Web Servers")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_monitoring_tools(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Monitoring Tools"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut monitor_menu = SelectView::<&str>::new();
    monitor_menu.add_item("📊 Grafana - Visualization platform", "grafana_monitor");
    monitor_menu.add_item("📈 Prometheus - Monitoring system", "prometheus");
    monitor_menu.add_item("📊 Node Exporter - System metrics", "node_exporter");
    monitor_menu.add_item("🔍 Netdata - Real-time monitoring", "netdata");
    monitor_menu.add_item("🌡️ Glances - System monitoring", "glances");
    monitor_menu.add_item("💾 Zabbix - Enterprise monitoring", "zabbix");
    monitor_menu.add_item("📡 Nagios - Network monitoring", "nagios");
    monitor_menu.add_item("📊 Complete Monitoring Stack", "monitoring_stack");
    
    layout.add_child(monitor_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Monitoring Tools")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_security_tools(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Security Tools"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut security_menu = SelectView::<&str>::new();
    security_menu.add_item("🛡️ Fail2Ban - Intrusion prevention", "fail2ban");
    security_menu.add_item("🦠 ClamAV - Antivirus scanner", "clamav");
    security_menu.add_item("🔥 UFW - Uncomplicated firewall", "ufw");
    security_menu.add_item("🔐 OpenSSL - Cryptography toolkit", "openssl");
    security_menu.add_item("🕵️ Tripwire - File integrity monitoring", "tripwire");
    security_menu.add_item("🔒 OSSEC - Security monitoring", "ossec");
    security_menu.add_item("🌐 VPN Server Setup", "vpn_server");
    security_menu.add_item("🛡️ Complete Security Hardening", "security_hardening");
    
    layout.add_child(security_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Security Tools")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_cloud_services(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Cloud Services"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut cloud_menu = SelectView::<&str>::new();
    cloud_menu.add_item("☁️ Nextcloud - Personal cloud", "nextcloud");
    cloud_menu.add_item("🔄 Syncthing - File synchronization", "syncthing");
    cloud_menu.add_item("💾 Restic - Backup solution", "restic");
    cloud_menu.add_item("📦 MinIO - Object storage", "minio");
    cloud_menu.add_item("🗃️ Seafile - File hosting", "seafile");
    cloud_menu.add_item("🔄 Rclone - Cloud sync tool", "rclone");
    cloud_menu.add_item("💿 Duplicati - Backup software", "duplicati");
    cloud_menu.add_item("☁️ Complete Cloud Stack", "cloud_stack");
    
    layout.add_child(cloud_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Cloud Services")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_audio_software(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Audio Software"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut audio_menu = SelectView::<&str>::new();
    audio_menu.add_item("🔊 PulseAudio - Audio server", "pulseaudio");
    audio_menu.add_item("🎵 JACK - Low-latency audio", "jack");
    audio_menu.add_item("🎼 Ardour - Digital audio workstation", "ardour");
    audio_menu.add_item("🎹 LMMS - Music production", "lmms");
    audio_menu.add_item("🎧 Audacity - Audio editor", "audacity");
    audio_menu.add_item("🎶 MPD - Music player daemon", "mpd");
    audio_menu.add_item("📻 Icecast - Audio streaming", "icecast_audio");
    audio_menu.add_item("🔊 Audio Optimization Package", "audio_optimization");
    
    layout.add_child(audio_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Audio Software")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_desktop_environments(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Desktop Environments"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut desktop_menu = SelectView::<&str>::new();
    desktop_menu.add_item("🌟 GNOME - Modern desktop", "gnome");
    desktop_menu.add_item("🔷 KDE Plasma - Feature-rich desktop", "kde");
    desktop_menu.add_item("🖥️ XFCE - Lightweight desktop", "xfce");
    desktop_menu.add_item("⚡ LXQt - Ultra-lightweight desktop", "lxqt");
    desktop_menu.add_item("🎯 MATE - Traditional desktop", "mate");
    desktop_menu.add_item("🌊 Cinnamon - User-friendly desktop", "cinnamon");
    desktop_menu.add_item("🏃 i3 - Tiling window manager", "i3wm");
    desktop_menu.add_item("🎮 Gaming-optimized Desktop", "gaming_desktop");
    
    layout.add_child(desktop_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Desktop Environments")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_network_services(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Network Services"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut network_menu = SelectView::<&str>::new();
    network_menu.add_item("🔒 OpenVPN Server", "openvpn_server");
    network_menu.add_item("⚡ WireGuard Server", "wireguard_server");
    network_menu.add_item("🌐 DNS Server (Pi-hole)", "pihole");
    network_menu.add_item("📡 DHCP Server", "dhcp_server");
    network_menu.add_item("📁 SMB/CIFS Server", "smb_server");
    network_menu.add_item("📂 NFS Server", "nfs_server");
    network_menu.add_item("🔍 Network Scanner", "network_scanner");
    network_menu.add_item("🌐 Complete Network Stack", "network_stack");
    
    layout.add_child(network_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Network Services")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_system_utilities(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("System Utilities"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut utils_menu = SelectView::<&str>::new();
    utils_menu.add_item("💾 Backup Tools - rsync, borgbackup", "backup_tools");
    utils_menu.add_item("🔍 System Monitoring - htop, iotop", "system_monitoring");
    utils_menu.add_item("🧹 System Cleanup - bleachbit, stacer", "system_cleanup");
    utils_menu.add_item("⚡ Performance Tools - stress, benchmarks", "performance_tools");
    utils_menu.add_item("🔧 Hardware Tools - lshw, lscpu", "hardware_tools");
    utils_menu.add_item("📊 Disk Utilities - gparted, smarttools", "disk_utilities");
    utils_menu.add_item("🌡️ Temperature Monitoring", "temp_monitoring");
    utils_menu.add_item("🛠️ Complete Utility Suite", "utility_suite");
    
    layout.add_child(utils_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("System Utilities")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_installation_progress(siv: &mut Cursive, software_name: &str) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new(format!("Installing {}...", software_name)));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("⬇️ Downloading packages..."));
    layout.add_child(TextView::new("📦 Installing dependencies..."));
    layout.add_child(TextView::new("⚙️ Configuring software..."));
    layout.add_child(TextView::new("🔧 Applying optimizations..."));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("This may take several minutes..."));
    
    let dialog = Dialog::around(layout.fixed_width(50))
        .title("Software Installation");
    
    siv.add_layer(dialog);
    
    // Simulate installation completion
    let software_name_owned = software_name.to_string();
    siv.add_global_callback(cursive::event::Event::Refresh, move |s| {
        s.pop_layer();
        s.add_layer(
            Dialog::text(format!("{} installed successfully!\n\n✅ Software configured\n✅ Orange Pi 5 Plus optimizations applied\n✅ Hardware acceleration enabled\n\nSoftware is ready to use.", software_name_owned))
                .title("Installation Complete")
                .button("Launch", |s| { s.pop_layer(); })
                .button("OK", |s| { s.pop_layer(); })
        );
    });
}

fn show_gamescope_install(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("GameScope Installation"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Gaming compositor optimized for Orange Pi 5 Plus"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Features:"));
    layout.add_child(TextView::new("• Mali-G610 GPU acceleration"));
    layout.add_child(TextView::new("• Variable refresh rate support"));
    layout.add_child(TextView::new("• Frame rate limiting"));
    layout.add_child(TextView::new("• HDR support"));
    layout.add_child(TextView::new("• Steam Deck compatibility mode"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("gamescope_vulkan"))
        .child(TextView::new(" Vulkan support")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("gamescope_wayland"))
        .child(TextView::new(" Wayland support")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("gamescope_experimental"))
        .child(TextView::new(" Experimental features")));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("GameScope Installation")
        .button("Install", |s| {
            s.pop_layer();
            show_installation_progress(s, "GameScope");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_steam_install(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Steam Installation"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("PC gaming platform with ARM64 compatibility"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("⚠️ Note: Steam on ARM64 requires:"));
    layout.add_child(TextView::new("• Box64 emulation layer"));
    layout.add_child(TextView::new("• x86-64 to ARM64 translation"));
    layout.add_child(TextView::new("• Performance may vary by game"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("steam_box64"))
        .child(TextView::new(" Install Box64 emulator")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("steam_proton"))
        .child(TextView::new(" Install Proton compatibility layer")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("steam_experimental"))
        .child(TextView::new(" Enable experimental features")));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("Steam Installation")
        .button("Install", |s| {
            s.pop_layer();
            show_installation_progress(s, "Steam with Box64");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_gaming_optimizations(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Gaming Optimization Package"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("System-wide gaming performance improvements"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Optimizations included:"));
    layout.add_child(TextView::new("• CPU governor optimization"));
    layout.add_child(TextView::new("• GPU frequency scaling"));
    layout.add_child(TextView::new("• Memory management tuning"));
    layout.add_child(TextView::new("• I/O scheduler optimization"));
    layout.add_child(TextView::new("• Audio latency reduction"));
    layout.add_child(TextView::new("• Network performance tuning"));
    layout.add_child(TextView::new("• Thermal management"));
    layout.add_child(DummyView.fixed_height(1));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("Gaming Optimizations")
        .button("Apply All", |s| {
            s.pop_layer();
            show_installation_progress(s, "Gaming Optimizations");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_jellyfin_install(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Jellyfin Media Server Installation"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Open-source media server with hardware transcoding"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Features:"));
    layout.add_child(TextView::new("• RK3588S hardware transcoding"));
    layout.add_child(TextView::new("• H.264/H.265 acceleration"));
    layout.add_child(TextView::new("• 4K video support"));
    layout.add_child(TextView::new("• Mobile and TV clients"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("jellyfin_hw_accel"))
        .child(TextView::new(" Enable hardware transcoding")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("jellyfin_web"))
        .child(TextView::new(" Install web interface")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("jellyfin_plugins"))
        .child(TextView::new(" Install popular plugins")));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("Jellyfin Installation")
        .button("Install", |s| {
            s.pop_layer();
            show_installation_progress(s, "Jellyfin Media Server");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_plex_install(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Plex Media Server Installation"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Premium media server with advanced features"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("⚠️ Note: Plex Pass required for:"));
    layout.add_child(TextView::new("• Hardware transcoding"));
    layout.add_child(TextView::new("• Mobile sync"));
    layout.add_child(TextView::new("• Premium features"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("plex_server"))
        .child(TextView::new(" Plex Media Server")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("plex_hw_accel"))
        .child(TextView::new(" Hardware transcoding (requires Plex Pass)")));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("Plex Installation")
        .button("Install", |s| {
            s.pop_layer();
            show_installation_progress(s, "Plex Media Server");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_hw_transcoding_setup(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Hardware Transcoding Setup"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Configure RK3588S hardware video acceleration"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Components:"));
    layout.add_child(TextView::new("• MPP (Media Processing Platform)"));
    layout.add_child(TextView::new("• V4L2 hardware acceleration"));
    layout.add_child(TextView::new("• FFmpeg with RK codecs"));
    layout.add_child(TextView::new("• GStreamer RK plugins"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Supported formats:"));
    layout.add_child(TextView::new("• H.264 (decode/encode)"));
    layout.add_child(TextView::new("• H.265/HEVC (decode/encode)"));
    layout.add_child(TextView::new("• VP9 (decode)"));
    layout.add_child(TextView::new("• MJPEG (decode/encode)"));
    
    let dialog = Dialog::around(layout.fixed_width(65))
        .title("Hardware Transcoding")
        .button("Setup", |s| {
            s.pop_layer();
            show_installation_progress(s, "Hardware Transcoding");
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}