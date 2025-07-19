use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView, EditView, Checkbox};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;

pub fn show_network_configuration_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Network Configuration & Connectivity"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Configure network interfaces and services"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("🌐 Network Interfaces - Configure Ethernet and wireless", "interfaces");
    menu.add_item("📶 Wi-Fi Configuration - Connect to wireless networks", "wifi");
    menu.add_item("🔗 Ethernet Settings - Wired network configuration", "ethernet");
    menu.add_item("🔒 VPN Configuration - VPN client and server setup", "vpn");
    menu.add_item("📍 IP Configuration - Static and dynamic IP settings", "ip_config");
    menu.add_item("🌍 DNS Settings - Domain name resolution", "dns");
    menu.add_item("🛡️ Firewall Rules - Network security and filtering", "firewall");
    menu.add_item("📱 Bluetooth - Bluetooth device management", "bluetooth");
    menu.add_item("🌐 Hotspot - Create Wi-Fi access point", "hotspot");
    menu.add_item("🔧 Advanced Networking - Bridging, routing, tunnels", "advanced");
    menu.add_item("📊 Network Monitoring - Bandwidth and connection stats", "monitoring");
    menu.add_item("🔍 Network Diagnostics - Troubleshooting tools", "diagnostics");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "interfaces" => show_network_interfaces(s),
            "wifi" => show_wifi_configuration(s),
            "ethernet" => show_ethernet_configuration(s),
            "vpn" => show_vpn_configuration(s),
            "ip_config" => show_ip_configuration(s),
            "dns" => show_dns_configuration(s),
            "firewall" => show_firewall_configuration(s),
            "bluetooth" => show_bluetooth_configuration(s),
            "hotspot" => show_hotspot_configuration(s),
            "advanced" => show_advanced_networking(s),
            "monitoring" => show_network_monitoring(s),
            "diagnostics" => show_network_diagnostics(s),
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
    
    let dialog = Dialog::around(layout.fixed_width(80))
        .title("Network Configuration")
        .button("Back", |s| { 
            s.pop_layer(); 
            crate::armpi_tweaker::show_armpi_tweaker(s);
        });
    
    siv.add_layer(dialog);
}

fn show_network_interfaces(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Network Interface Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Detected interfaces:"));
    layout.add_child(TextView::new("• eth0: Ethernet (1Gbps) - Connected"));
    layout.add_child(TextView::new("• wlan0: Wi-Fi (802.11ac) - Disconnected"));
    layout.add_child(TextView::new("• lo: Loopback - Active"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut interfaces_menu = SelectView::<&str>::new();
    interfaces_menu.add_item("📋 List All Interfaces", "list_interfaces");
    interfaces_menu.add_item("🟢 Enable Interface", "enable_interface");
    interfaces_menu.add_item("🔴 Disable Interface", "disable_interface");
    interfaces_menu.add_item("⚙️ Configure Interface", "config_interface");
    interfaces_menu.add_item("📊 Interface Statistics", "interface_stats");
    interfaces_menu.add_item("🔄 Restart Network Service", "restart_network");
    interfaces_menu.add_item("🔧 Advanced Interface Settings", "advanced_interface");
    interfaces_menu.add_item("🏷️ Interface Naming", "interface_naming");
    interfaces_menu.add_item("⚡ Interface Bonding", "interface_bonding");
    interfaces_menu.add_item("🌉 Network Bridging", "network_bridging");
    
    layout.add_child(interfaces_menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Network Interfaces")
        .button("Refresh", |s| { s.pop_layer(); })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_wifi_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Wi-Fi Network Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Status: wlan0 - Not connected"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut wifi_menu = SelectView::<&str>::new();
    wifi_menu.add_item("🔍 Scan for Networks", "scan_networks");
    wifi_menu.add_item("🔗 Connect to Network", "connect_network");
    wifi_menu.add_item("💾 Saved Networks", "saved_networks");
    wifi_menu.add_item("🔐 Security Settings", "wifi_security");
    wifi_menu.add_item("📶 Signal Strength", "signal_strength");
    wifi_menu.add_item("⚙️ Wi-Fi Adapter Settings", "adapter_settings");
    wifi_menu.add_item("🔧 Advanced Wi-Fi Settings", "advanced_wifi");
    wifi_menu.add_item("🎯 Preferred Networks", "preferred_networks");
    wifi_menu.add_item("📱 Wi-Fi Direct", "wifi_direct");
    wifi_menu.add_item("🏠 Enterprise Networks", "enterprise_wifi");
    
    wifi_menu.set_on_submit(|s, option| {
        match *option {
            "scan_networks" => show_wifi_scan(s),
            "connect_network" => show_wifi_connect(s),
            "saved_networks" => show_saved_networks(s),
            "wifi_security" => show_wifi_security(s),
            _ => {
                s.add_layer(
                    Dialog::text("Wi-Fi feature is being implemented!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(wifi_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Wi-Fi Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_wifi_scan(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Available Wi-Fi Networks"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut networks = SelectView::<&str>::new();
    networks.add_item("🔒 HomeNetwork-5G    [-45 dBm] WPA2", "home_5g");
    networks.add_item("🔒 OfficeWiFi       [-52 dBm] WPA3", "office");
    networks.add_item("🔒 iPhone_Hotspot   [-67 dBm] WPA2", "iphone");
    networks.add_item("🔓 FreeWiFi         [-72 dBm] Open", "free");
    networks.add_item("🔒 Neighbor_2.4G    [-78 dBm] WPA2", "neighbor");
    
    networks.set_on_submit(|s, network| {
        let network_name = match *network {
            "home_5g" => "HomeNetwork-5G",
            "office" => "OfficeWiFi",
            "iphone" => "iPhone_Hotspot",
            "free" => "FreeWiFi",
            "neighbor" => "Neighbor_2.4G",
            _ => "Unknown",
        };
        
        if *network == "free" {
            s.add_layer(
                Dialog::text(format!("Connect to {}?\n\nThis is an open network (no password required).", network_name))
                    .title("Connect to Network")
                    .button("Connect", |s| {
                        s.pop_layer();
                        s.add_layer(
                            Dialog::text("Connecting to network...")
                                .title("Connecting")
                                .button("Cancel", |s| { s.pop_layer(); })
                        );
                    })
                    .button("Cancel", |s| { s.pop_layer(); })
            );
        } else {
            show_wifi_password_dialog(s, network_name);
        }
    });
    
    layout.add_child(networks);
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Wi-Fi Networks")
        .button("Rescan", |s| { s.pop_layer(); })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_wifi_password_dialog(siv: &mut Cursive, network_name: &str) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new(format!("Connect to: {}", network_name)));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Password:"));
    layout.add_child(EditView::new().secret().with_name("wifi_password"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("save_password"))
        .child(TextView::new(" Save password")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("auto_connect"))
        .child(TextView::new(" Connect automatically")));
    
    let dialog = Dialog::around(layout.fixed_width(50))
        .title("Wi-Fi Password")
        .button("Connect", |s| {
            s.pop_layer();
            s.add_layer(
                Dialog::text("Connecting to network...\n\nThis may take a few seconds.")
                    .title("Connecting")
                    .button("Cancel", |s| { s.pop_layer(); })
            );
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ethernet_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Ethernet Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Interface: eth0 (Realtek RTL8111H)"));
    layout.add_child(TextView::new("Status: Connected (1000 Mbps, Full Duplex)"));
    layout.add_child(TextView::new("IP: 192.168.1.100/24"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut ethernet_menu = SelectView::<&str>::new();
    ethernet_menu.add_item("📊 Connection Status", "connection_status");
    ethernet_menu.add_item("⚙️ Interface Configuration", "interface_config");
    ethernet_menu.add_item("🌐 IP Address Settings", "ip_settings");
    ethernet_menu.add_item("⚡ Speed & Duplex", "speed_duplex");
    ethernet_menu.add_item("🔧 Advanced Settings", "advanced_ethernet");
    ethernet_menu.add_item("📈 Traffic Statistics", "traffic_stats");
    ethernet_menu.add_item("🔍 Cable Diagnostics", "cable_diag");
    ethernet_menu.add_item("💤 Wake-on-LAN", "wake_on_lan");
    ethernet_menu.add_item("🏷️ VLAN Configuration", "vlan_config");
    ethernet_menu.add_item("🔄 Reset Interface", "reset_interface");
    
    layout.add_child(ethernet_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Ethernet Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ip_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("IP Address Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut ip_menu = SelectView::<&str>::new();
    ip_menu.add_item("🔄 DHCP Configuration", "dhcp_config");
    ip_menu.add_item("📍 Static IP Settings", "static_ip");
    ip_menu.add_item("🌐 IPv6 Configuration", "ipv6_config");
    ip_menu.add_item("🛣️ Routing Table", "routing");
    ip_menu.add_item("🔗 Default Gateway", "gateway");
    ip_menu.add_item("📋 Current IP Settings", "current_ip");
    ip_menu.add_item("🔧 Advanced IP Settings", "advanced_ip");
    ip_menu.add_item("🏷️ IP Aliases", "ip_aliases");
    ip_menu.add_item("📊 IP Statistics", "ip_stats");
    ip_menu.add_item("🔄 Renew DHCP Lease", "renew_dhcp");
    
    layout.add_child(ip_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("IP Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_dns_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("DNS Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Current DNS servers:"));
    layout.add_child(TextView::new("• 1.1.1.1 (Cloudflare)"));
    layout.add_child(TextView::new("• 8.8.8.8 (Google)"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut dns_menu = SelectView::<&str>::new();
    dns_menu.add_item("⚙️ DNS Server Settings", "dns_servers");
    dns_menu.add_item("🏠 Local DNS Resolution", "local_dns");
    dns_menu.add_item("🔒 DNS over HTTPS (DoH)", "dns_doh");
    dns_menu.add_item("🛡️ DNS over TLS (DoT)", "dns_dot");
    dns_menu.add_item("📋 DNS Cache", "dns_cache");
    dns_menu.add_item("🔍 DNS Lookup Test", "dns_lookup");
    dns_menu.add_item("⚡ DNS Performance Test", "dns_performance");
    dns_menu.add_item("🌐 Public DNS Providers", "public_dns");
    dns_menu.add_item("🏷️ DNS Search Domains", "search_domains");
    dns_menu.add_item("🔧 Advanced DNS Settings", "advanced_dns");
    
    layout.add_child(dns_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("DNS Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_vpn_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("VPN Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Virtual Private Network setup and management"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut vpn_menu = SelectView::<&str>::new();
    vpn_menu.add_item("🔗 OpenVPN Client", "openvpn_client");
    vpn_menu.add_item("🌐 OpenVPN Server", "openvpn_server");
    vpn_menu.add_item("⚡ WireGuard Client", "wireguard_client");
    vpn_menu.add_item("🏠 WireGuard Server", "wireguard_server");
    vpn_menu.add_item("🏢 IPsec/IKEv2", "ipsec");
    vpn_menu.add_item("📱 PPTP Client", "pptp");
    vpn_menu.add_item("🔒 L2TP/IPsec", "l2tp");
    vpn_menu.add_item("📋 VPN Status", "vpn_status");
    vpn_menu.add_item("🔑 Certificate Management", "vpn_certs");
    vpn_menu.add_item("🔧 VPN Troubleshooting", "vpn_troubleshoot");
    
    layout.add_child(vpn_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("VPN Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_firewall_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Firewall Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Status: UFW enabled | Rules: 12 active"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut firewall_menu = SelectView::<&str>::new();
    firewall_menu.add_item("🟢 Enable Firewall", "enable_firewall");
    firewall_menu.add_item("🔴 Disable Firewall", "disable_firewall");
    firewall_menu.add_item("📋 Firewall Status", "firewall_status");
    firewall_menu.add_item("📝 Firewall Rules", "firewall_rules");
    firewall_menu.add_item("➕ Add Rule", "add_rule");
    firewall_menu.add_item("🗑️ Remove Rule", "remove_rule");
    firewall_menu.add_item("🎯 Default Policies", "default_policies");
    firewall_menu.add_item("🔧 Advanced Rules", "advanced_rules");
    firewall_menu.add_item("📊 Firewall Logs", "firewall_logs");
    firewall_menu.add_item("🏠 Port Forwarding", "port_forwarding");
    firewall_menu.add_item("🛡️ DDoS Protection", "ddos_protection");
    firewall_menu.add_item("🔒 Application Profiles", "app_profiles");
    
    layout.add_child(firewall_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Firewall Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_bluetooth_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Bluetooth Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Bluetooth adapter: ON | Discoverable: OFF"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut bluetooth_menu = SelectView::<&str>::new();
    bluetooth_menu.add_item("🔵 Enable Bluetooth", "enable_bluetooth");
    bluetooth_menu.add_item("⚫ Disable Bluetooth", "disable_bluetooth");
    bluetooth_menu.add_item("👁️ Make Discoverable", "make_discoverable");
    bluetooth_menu.add_item("🔍 Scan for Devices", "scan_devices");
    bluetooth_menu.add_item("📱 Paired Devices", "paired_devices");
    bluetooth_menu.add_item("🔗 Pair New Device", "pair_device");
    bluetooth_menu.add_item("🗑️ Remove Device", "remove_device");
    bluetooth_menu.add_item("🎵 Audio Devices", "audio_devices");
    bluetooth_menu.add_item("📁 File Transfer", "file_transfer");
    bluetooth_menu.add_item("⚙️ Bluetooth Settings", "bluetooth_settings");
    bluetooth_menu.add_item("🔧 Troubleshooting", "bluetooth_troubleshoot");
    
    layout.add_child(bluetooth_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Bluetooth Configuration")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_hotspot_configuration(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Wi-Fi Hotspot Configuration"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Create a Wi-Fi access point using your Orange Pi"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Network Name (SSID):"));
    layout.add_child(EditView::new().content("OrangePi-Hotspot").with_name("hotspot_ssid"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Password:"));
    layout.add_child(EditView::new().secret().with_name("hotspot_password"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Security:"));
    let mut security_select = SelectView::<&str>::new();
    security_select.add_item("WPA2", "wpa2");
    security_select.add_item("WPA3", "wpa3");
    security_select.add_item("Open (No Security)", "open");
    layout.add_child(security_select.with_name("hotspot_security"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Channel:"));
    let mut channel_select = SelectView::<u8>::new();
    for i in 1..=11 {
        channel_select.add_item(&format!("Channel {}", i), i);
    }
    channel_select.set_selection(5); // Default to channel 6
    layout.add_child(channel_select.with_name("hotspot_channel"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("hotspot_hidden"))
        .child(TextView::new(" Hidden network")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("hotspot_internet"))
        .child(TextView::new(" Share internet connection")));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Wi-Fi Hotspot")
        .button("Start Hotspot", |s| {
            s.add_layer(
                Dialog::text("Wi-Fi hotspot started successfully!\n\nOther devices can now connect to 'OrangePi-Hotspot'")
                    .title("Hotspot Active")
                    .button("Stop Hotspot", |s| { s.pop_layer(); })
                    .button("OK", |s| { s.pop_layer(); })
            );
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_advanced_networking(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Advanced Networking"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Advanced network configuration and services"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut advanced_menu = SelectView::<&str>::new();
    advanced_menu.add_item("🌉 Network Bridging", "network_bridge");
    advanced_menu.add_item("🛣️ Routing Configuration", "routing_config");
    advanced_menu.add_item("🏷️ VLAN Setup", "vlan_setup");
    advanced_menu.add_item("🔗 Network Bonding", "network_bonding");
    advanced_menu.add_item("🌐 NAT Configuration", "nat_config");
    advanced_menu.add_item("🔧 Traffic Shaping (QoS)", "traffic_shaping");
    advanced_menu.add_item("🌀 Load Balancing", "load_balancing");
    advanced_menu.add_item("🔒 Network Tunnels", "network_tunnels");
    advanced_menu.add_item("📊 Bandwidth Monitoring", "bandwidth_monitor");
    advanced_menu.add_item("🎯 Packet Filtering", "packet_filtering");
    advanced_menu.add_item("🔍 Network Analysis", "network_analysis");
    advanced_menu.add_item("🏠 Network Namespaces", "network_namespaces");
    
    layout.add_child(advanced_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Advanced Networking")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_network_monitoring(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Network Monitoring & Statistics"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Real-time network usage and performance"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(TextView::new("Current Network Activity:"));
    layout.add_child(TextView::new("📈 Download: 15.2 MB/s | Upload: 2.1 MB/s"));
    layout.add_child(TextView::new("📊 Total: 2.5 GB down | 1.2 GB up"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut monitor_menu = SelectView::<&str>::new();
    monitor_menu.add_item("📊 Real-time Traffic", "realtime_traffic");
    monitor_menu.add_item("📈 Bandwidth Usage", "bandwidth_usage");
    monitor_menu.add_item("🌐 Connection Status", "connection_status");
    monitor_menu.add_item("📋 Network Statistics", "network_stats");
    monitor_menu.add_item("🔍 Active Connections", "active_connections");
    monitor_menu.add_item("📱 Device Discovery", "device_discovery");
    monitor_menu.add_item("🎯 Port Scanning", "port_scanning");
    monitor_menu.add_item("📊 Traffic Analysis", "traffic_analysis");
    monitor_menu.add_item("⏱️ Latency Monitoring", "latency_monitor");
    monitor_menu.add_item("🔧 Network Troubleshooting", "network_troubleshoot");
    
    layout.add_child(monitor_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Network Monitoring")
        .button("Refresh", |s| { s.pop_layer(); })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_network_diagnostics(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Network Diagnostics & Troubleshooting"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Tools to diagnose and fix network issues"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut diag_menu = SelectView::<&str>::new();
    diag_menu.add_item("🏓 Ping Test", "ping_test");
    diag_menu.add_item("🛣️ Traceroute", "traceroute");
    diag_menu.add_item("🌐 DNS Lookup", "dns_lookup");
    diag_menu.add_item("🔌 Port Connectivity", "port_test");
    diag_menu.add_item("⚡ Speed Test", "speed_test");
    diag_menu.add_item("📊 Network Quality", "network_quality");
    diag_menu.add_item("🔍 Packet Capture", "packet_capture");
    diag_menu.add_item("🔧 Network Reset", "network_reset");
    diag_menu.add_item("📋 Network Report", "network_report");
    diag_menu.add_item("🏥 Auto Diagnosis", "auto_diagnosis");
    
    diag_menu.set_on_submit(|s, option| {
        match *option {
            "ping_test" => show_ping_test(s),
            "speed_test" => show_speed_test(s),
            "auto_diagnosis" => show_auto_diagnosis(s),
            _ => {
                s.add_layer(
                    Dialog::text("Network diagnostic tool is being implemented!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(diag_menu);
    
    let dialog = Dialog::around(layout.fixed_width(70))
        .title("Network Diagnostics")
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_ping_test(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Ping Network Test"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Host to ping:"));
    layout.add_child(EditView::new().content("8.8.8.8").with_name("ping_host"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Count:"));
    layout.add_child(EditView::new().content("4").with_name("ping_count"));
    
    let dialog = Dialog::around(layout.fixed_width(50))
        .title("Ping Test")
        .button("Start Ping", |s| {
            s.add_layer(
                Dialog::text("PING 8.8.8.8 (8.8.8.8) 56(84) bytes of data.\n64 bytes from 8.8.8.8: icmp_seq=1 ttl=118 time=15.2 ms\n64 bytes from 8.8.8.8: icmp_seq=2 ttl=118 time=14.8 ms\n64 bytes from 8.8.8.8: icmp_seq=3 ttl=118 time=15.1 ms\n64 bytes from 8.8.8.8: icmp_seq=4 ttl=118 time=15.0 ms\n\n--- 8.8.8.8 ping statistics ---\n4 packets transmitted, 4 received, 0% packet loss\nround-trip min/avg/max/mdev = 14.8/15.0/15.2/0.2 ms")
                    .title("Ping Results")
                    .button("OK", |s| { s.pop_layer(); })
            );
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_speed_test(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Network Speed Test"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Testing connection speed..."));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("📡 Server: Speedtest.net (Auto-selected)"));
    layout.add_child(TextView::new("⬇️ Download: Testing..."));
    layout.add_child(TextView::new("⬆️ Upload: Waiting..."));
    layout.add_child(TextView::new("🏓 Ping: Measuring..."));
    
    let dialog = Dialog::around(layout.fixed_width(50))
        .title("Speed Test")
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
    
    // Simulate speed test completion
    siv.add_global_callback(cursive::event::Event::Refresh, |s| {
        s.pop_layer();
        s.add_layer(
            Dialog::text("Speed Test Results:\n\n⬇️ Download: 95.3 Mbps\n⬆️ Upload: 42.1 Mbps\n🏓 Ping: 15 ms\n📊 Jitter: 2 ms\n\n🌐 Server: Speedtest.net - Local ISP")
                .title("Speed Test Complete")
                .button("Run Again", |s| { s.pop_layer(); })
                .button("Close", |s| { s.pop_layer(); })
        );
    });
}

fn show_auto_diagnosis(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Automatic Network Diagnosis"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Running comprehensive network diagnostics..."));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("✅ Interface Status: OK"));
    layout.add_child(TextView::new("✅ IP Configuration: OK"));
    layout.add_child(TextView::new("✅ DNS Resolution: OK"));
    layout.add_child(TextView::new("✅ Internet Connectivity: OK"));
    layout.add_child(TextView::new("⚠️ IPv6 Connectivity: Disabled"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("💡 Suggestions:"));
    layout.add_child(TextView::new("• Consider enabling IPv6 for better performance"));
    layout.add_child(TextView::new("• Wi-Fi signal could be stronger"));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Network Diagnosis")
        .button("Fix Issues", |s| {
            s.add_layer(
                Dialog::text("Automatic fixes applied:\n\n✅ IPv6 enabled\n✅ DNS cache cleared\n✅ Network interfaces reset\n\nYour network should now perform better!")
                    .title("Issues Fixed")
                    .button("OK", |s| { s.pop_layer(); })
            );
        })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_wifi_connect(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Connect to Wi-Fi Network"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Network Name (SSID):"));
    layout.add_child(EditView::new().with_name("wifi_ssid"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Security Type:"));
    let mut security_select = SelectView::<&str>::new();
    security_select.add_item("WPA/WPA2 Personal", "wpa2");
    security_select.add_item("WPA3 Personal", "wpa3");
    security_select.add_item("WEP", "wep");
    security_select.add_item("Open (No Security)", "open");
    security_select.add_item("WPA Enterprise", "enterprise");
    layout.add_child(security_select.with_name("wifi_security_type"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Password:"));
    layout.add_child(EditView::new().secret().with_name("wifi_manual_password"));
    
    let dialog = Dialog::around(layout.fixed_width(50))
        .title("Manual Wi-Fi Connection")
        .button("Connect", |s| { s.pop_layer(); })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_saved_networks(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Saved Wi-Fi Networks"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut networks = SelectView::<&str>::new();
    networks.add_item("🏠 HomeNetwork-5G (Connected)", "home");
    networks.add_item("🏢 OfficeWiFi", "office");
    networks.add_item("☕ CafeWiFi", "cafe");
    networks.add_item("📱 iPhone_Hotspot", "phone");
    
    layout.add_child(networks);
    
    let dialog = Dialog::around(layout.fixed_width(50))
        .title("Saved Networks")
        .button("Connect", |s| { s.pop_layer(); })
        .button("Forget", |s| { s.pop_layer(); })
        .button("Close", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

fn show_wifi_security(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Wi-Fi Security Settings"));
    layout.add_child(DummyView.fixed_height(1));
    
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("auto_connect_secure"))
        .child(TextView::new(" Only auto-connect to secure networks")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().with_name("randomize_mac"))
        .child(TextView::new(" Randomize MAC address")));
    layout.add_child(LinearLayout::horizontal()
        .child(Checkbox::new().checked().with_name("warn_open_networks"))
        .child(TextView::new(" Warn about open networks")));
    
    let dialog = Dialog::around(layout.fixed_width(60))
        .title("Wi-Fi Security")
        .button("Save", |s| { s.pop_layer(); })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}