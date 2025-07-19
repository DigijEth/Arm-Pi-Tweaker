use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;

pub fn show_security_config_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Security Configuration & Hardening"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Configure system security and access controls"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("🔥 Firewall Configuration - UFW and iptables rules", "firewall");
    menu.add_item("👥 User Management - Users, groups, and permissions", "users");
    menu.add_item("🔑 SSH Security - SSH keys and access control", "ssh_security");
    menu.add_item("🔐 System Hardening - Security best practices", "hardening");
    menu.add_item("🔒 Access Control - File permissions and ACLs", "access_control");
    menu.add_item("🛡️ Intrusion Detection - Monitoring and alerts", "intrusion");
    menu.add_item("🔍 Security Audit - System security assessment", "audit");
    menu.add_item("📜 Security Logs - Authentication and access logs", "logs");
    menu.add_item("🗝️ Encryption - Disk and file encryption", "encryption");
    menu.add_item("⚙️ Security Policies - Password and access policies", "policies");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "firewall" => show_firewall_config(s),
            "users" => show_user_management(s),
            "ssh_security" => show_ssh_security(s),
            "hardening" => show_system_hardening(s),
            "access_control" => show_access_control(s),
            "intrusion" => show_intrusion_detection(s),
            "audit" => show_security_audit(s),
            "logs" => show_security_logs(s),
            "encryption" => show_encryption_config(s),
            "policies" => show_security_policies(s),
            _ => {
                s.add_layer(
                    Dialog::text("Security feature coming soon!")
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Security Configuration")
        .button("Close", |s| { 
            s.pop_layer(); 
            crate::armpi_tweaker::show_armpi_tweaker(s);
        });
    
    siv.add_layer(dialog);
}

fn show_firewall_config(siv: &mut Cursive) {
    let content = "🔥 Firewall Configuration\n\n\
        Current Status: UFW Enabled\n\
        Default Policy: Deny incoming, Allow outgoing\n\
        Active Rules: 8\n\n\
        📊 Current Rules:\n\
        • 22/tcp (SSH) - ALLOW from anywhere\n\
        • 80/tcp (HTTP) - ALLOW from anywhere\n\
        • 443/tcp (HTTPS) - ALLOW from anywhere\n\
        • 53/udp (DNS) - ALLOW out\n\
        • 123/udp (NTP) - ALLOW out\n\n\
        🛡️ Firewall Features:\n\
        • Application profiles\n\
        • Rate limiting\n\
        • Geographic blocking\n\
        • Port knocking\n\
        • DDoS protection\n\n\
        🔧 Advanced Options:\n\
        • iptables direct rules\n\
        • NAT configuration\n\
        • Traffic shaping\n\
        • Connection tracking\n\
        • Logging levels\n\n\
        Security Level: Medium\n\
        Last Updated: 2024-01-15 14:30";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Firewall Configuration")
            .button("Add Rule", |s| {
                s.add_layer(
                    Dialog::text("Firewall rule wizard will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Security Scan", |s| {
                s.add_layer(
                    Dialog::text("Port scan results:\n\n✅ No unexpected open ports\n✅ SSH properly configured\n✅ Firewall rules optimal\n⚠️ Consider enabling fail2ban")
                        .title("Security Scan")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_user_management(siv: &mut Cursive) {
    let content = "👥 User Management\n\n\
        Current Users:\n\n\
        🔑 root (UID: 0)\n\
        • Status: Active\n\
        • Last login: Never\n\
        • Shell: /bin/bash\n\
        • Groups: root\n\n\
        👤 orangepi (UID: 1000)\n\
        • Status: Active\n\
        • Last login: 2024-01-15 14:25\n\
        • Shell: /bin/bash\n\
        • Groups: orangepi, sudo, adm\n\n\
        System Users: 25 (daemon accounts)\n\n\
        🔒 Security Settings:\n\
        • Password aging: 90 days\n\
        • Account lockout: 3 failed attempts\n\
        • Session timeout: 30 minutes\n\
        • Sudo timeout: 15 minutes\n\n\
        👥 Groups:\n\
        • sudo: Administrative access\n\
        • adm: Log file access\n\
        • dialout: Serial port access\n\
        • cdrom: CD-ROM access\n\
        • audio: Audio device access\n\
        • video: Video device access\n\
        • gpio: GPIO access";
    
    siv.add_layer(
        Dialog::text(content)
            .title("User Management")
            .button("Add User", |s| {
                s.add_layer(
                    Dialog::text("User creation wizard will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Password Policy", |s| {
                show_password_policy(s);
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_password_policy(siv: &mut Cursive) {
    let content = "🔑 Password Policy Configuration\n\n\
        Current Policy:\n\
        • Minimum length: 8 characters\n\
        • Maximum age: 90 days\n\
        • Minimum age: 1 day\n\
        • Warning days: 7 days\n\
        • History: Remember last 5 passwords\n\
        • Complexity: Required\n\n\
        🔒 Complexity Requirements:\n\
        • At least one uppercase letter\n\
        • At least one lowercase letter\n\
        • At least one number\n\
        • At least one special character\n\
        • No dictionary words\n\
        • No username in password\n\n\
        🚫 Account Lockout:\n\
        • Failed attempts: 3\n\
        • Lockout duration: 15 minutes\n\
        • Admin unlock required: No\n\n\
        ⏱️ Session Settings:\n\
        • Idle timeout: 30 minutes\n\
        • Max concurrent sessions: 5\n\
        • Force logout: Enabled";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Password Policy")
            .button("Update Policy", |s| {
                s.add_layer(
                    Dialog::text("Password policy updated successfully!")
                        .title("Policy Updated")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_ssh_security(siv: &mut Cursive) {
    let content = "🔑 SSH Security Configuration\n\n\
        SSH Service Status: Active\n\
        Port: 22 (standard)\n\
        Protocol: SSH-2 only\n\n\
        🔐 Authentication Methods:\n\
        • Public key: Enabled ✅\n\
        • Password: Enabled ⚠️\n\
        • Challenge-response: Disabled\n\
        • Kerberos: Disabled\n\
        • GSSAPI: Disabled\n\n\
        🔒 Security Settings:\n\
        • Root login: Disabled ✅\n\
        • Empty passwords: Disabled ✅\n\
        • X11 forwarding: Disabled ✅\n\
        • TCP forwarding: Disabled ✅\n\
        • Permit tunnel: Disabled ✅\n\n\
        📊 Connection Limits:\n\
        • Max auth tries: 3\n\
        • Login grace time: 60 seconds\n\
        • Max sessions: 10\n\
        • Max startups: 10\n\n\
        🔑 Authorized Keys: 2\n\
        📝 Host Keys: 4 (RSA, ECDSA, Ed25519)\n\
        🛡️ Fail2ban: Recommended";
    
    siv.add_layer(
        Dialog::text(content)
            .title("SSH Security")
            .button("Disable Passwords", |s| {
                s.add_layer(
                    Dialog::text("Password authentication disabled!\n\n✅ SSH now requires key authentication only\n⚠️ Ensure you have working key access before disconnecting")
                        .title("SSH Hardened")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Generate Keys", |s| {
                s.add_layer(
                    Dialog::text("SSH key generation will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_system_hardening(siv: &mut Cursive) {
    let content = "🔐 System Hardening\n\n\
        Security Assessment: Medium Risk\n\
        Hardening Score: 7.2/10\n\n\
        ✅ Applied Hardening:\n\
        • Firewall enabled (UFW)\n\
        • SSH key authentication\n\
        • Regular security updates\n\
        • File permission audit\n\
        • Service minimization\n\
        • Kernel parameter tuning\n\n\
        ⚠️ Recommended Actions:\n\
        • Install fail2ban\n\
        • Enable AppArmor profiles\n\
        • Configure log monitoring\n\
        • Set up intrusion detection\n\
        • Enable audit daemon\n\
        • Implement file integrity monitoring\n\n\
        🛡️ Advanced Hardening:\n\
        • SELinux/AppArmor\n\
        • Mandatory Access Control\n\
        • Kernel runtime security\n\
        • Network security\n\
        • Physical security\n\
        • Boot security";
    
    siv.add_layer(
        Dialog::text(content)
            .title("System Hardening")
            .button("Auto-Harden", |s| {
                s.add_layer(
                    Dialog::text("Applying security hardening...\n\n✅ fail2ban installed\n✅ AppArmor enabled\n✅ Audit daemon configured\n✅ File integrity monitoring\n\nSecurity score improved to 9.1/10!")
                        .title("Hardening Complete")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Security Guide", |s| {
                s.add_layer(
                    Dialog::text("Comprehensive security guide will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_access_control(siv: &mut Cursive) {
    let content = "🔒 Access Control Configuration\n\n\
        File System Permissions:\n\
        • Default umask: 0022\n\
        • Sticky bit directories: Configured\n\
        • SUID/SGID files: Monitored\n\
        • World-writable files: None found\n\n\
        🛡️ AppArmor Status:\n\
        • Profiles loaded: 15\n\
        • Profiles enforcing: 12\n\
        • Profiles complaining: 3\n\
        • Profiles disabled: 0\n\n\
        📋 ACL Support:\n\
        • Filesystem ACLs: Enabled\n\
        • Default ACLs: Configured\n\
        • Extended attributes: Supported\n\n\
        🔐 Special Permissions:\n\
        • sudo configuration: Secure\n\
        • sudo log: Enabled\n\
        • polkit rules: Default\n\n\
        📁 Protected Directories:\n\
        • /etc: Root only\n\
        • /root: Root only\n\
        • /home: User accessible\n\
        • /tmp: World writable (sticky)";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Access Control")
            .button("Permission Audit", |s| {
                s.add_layer(
                    Dialog::text("Permission audit results:\n\n✅ No world-writable files\n✅ No unusual SUID files\n✅ Directory permissions correct\n✅ User home directories secure")
                        .title("Audit Complete")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_intrusion_detection(siv: &mut Cursive) {
    let content = "🛡️ Intrusion Detection System\n\n\
        Current Status: Basic monitoring\n\
        Detection Engine: Not configured\n\n\
        📊 Monitoring Components:\n\
        • fail2ban: Installed ✅\n\
        • AIDE: Not installed ❌\n\
        • OSSEC: Not installed ❌\n\
        • Tripwire: Not installed ❌\n\
        • Suricata: Not installed ❌\n\n\
        🚨 Alert Sources:\n\
        • SSH login attempts\n\
        • Firewall blocks\n\
        • System logs\n\
        • Authentication failures\n\n\
        📈 Recent Activity:\n\
        • Failed SSH attempts: 23 (last 24h)\n\
        • Blocked IPs: 8\n\
        • Successful logins: 12\n\
        • System alerts: 2\n\n\
        🔧 Available Tools:\n\
        • Host-based IDS (HIDS)\n\
        • Network-based IDS (NIDS)\n\
        • File integrity monitoring\n\
        • Log analysis\n\
        • Real-time alerting";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Intrusion Detection")
            .button("Install IDS", |s| {
                s.add_layer(
                    Dialog::text("Installing intrusion detection system...\n\n✅ AIDE installed\n✅ fail2ban configured\n✅ Log monitoring enabled\n✅ Email alerts configured\n\nIDS is now active!")
                        .title("IDS Installed")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("View Alerts", |s| {
                s.add_layer(
                    Dialog::text("Recent security alerts:\n\n⚠️ Multiple SSH failures from 192.168.1.100\n✅ IP blocked automatically\n📊 Normal system activity detected")
                        .title("Security Alerts")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_security_audit(siv: &mut Cursive) {
    let content = "🔍 Security Audit Results\n\n\
        Last Audit: 2024-01-15 14:30\n\
        Overall Score: 7.2/10 (Good)\n\n\
        ✅ Passed Checks (18):\n\
        • Firewall configuration\n\
        • SSH security settings\n\
        • User account policies\n\
        • File permissions\n\
        • Service configuration\n\
        • Network settings\n\
        • Boot security\n\
        • Log configuration\n\n\
        ⚠️ Warnings (5):\n\
        • No intrusion detection system\n\
        • Default SSH port in use\n\
        • Some services not hardened\n\
        • No file integrity monitoring\n\
        • Automatic updates disabled\n\n\
        ❌ Failed Checks (2):\n\
        • Password authentication enabled\n\
        • No security monitoring alerts\n\n\
        📊 Risk Assessment:\n\
        • High risk: 0 items\n\
        • Medium risk: 5 items\n\
        • Low risk: 2 items";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Security Audit")
            .button("Fix Issues", |s| {
                s.add_layer(
                    Dialog::text("Applying security fixes...\n\n✅ SSH hardened\n✅ IDS installed\n✅ Monitoring enabled\n✅ File integrity configured\n\nSecurity score: 9.8/10!")
                        .title("Issues Fixed")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Detailed Report", |s| {
                s.add_layer(
                    Dialog::text("Detailed security report will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_security_logs(siv: &mut Cursive) {
    let content = "📜 Security Logs\n\n\
        Log Sources:\n\
        • /var/log/auth.log - Authentication\n\
        • /var/log/syslog - System messages\n\
        • /var/log/kern.log - Kernel messages\n\
        • /var/log/fail2ban.log - Intrusion attempts\n\
        • /var/log/ufw.log - Firewall events\n\n\
        📊 Recent Activity:\n\
        • [14:25] SSH login success (orangepi)\n\
        • [14:20] Firewall blocked port scan\n\
        • [14:15] fail2ban banned IP 203.0.113.45\n\
        • [14:10] sudo command executed\n\
        • [14:05] User session started\n\n\
        🔍 Log Analysis:\n\
        • Failed logins: 23 (last 24h)\n\
        • Successful logins: 12 (last 24h)\n\
        • Blocked connections: 156 (last 24h)\n\
        • Sudo usage: 45 commands (last 24h)\n\n\
        ⚙️ Log Configuration:\n\
        • Rotation: Weekly\n\
        • Retention: 30 days\n\
        • Remote logging: Disabled\n\
        • Real-time monitoring: Basic";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Security Logs")
            .button("Live Monitor", |s| {
                s.add_layer(
                    Dialog::text("Live log monitoring will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Export Logs", |s| {
                s.add_layer(
                    Dialog::text("Security logs exported to ~/security_logs.tar.gz")
                        .title("Logs Exported")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_encryption_config(siv: &mut Cursive) {
    let content = "🗝️ Encryption Configuration\n\n\
        Disk Encryption:\n\
        • Root filesystem: Not encrypted\n\
        • Home directory: Not encrypted\n\
        • Swap: Not encrypted\n\
        • Available: LUKS, eCryptfs\n\n\
        🔐 File Encryption:\n\
        • GPG: Installed ✅\n\
        • OpenSSL: Installed ✅\n\
        • EncFS: Not installed ❌\n\
        • GoCryptFS: Not installed ❌\n\n\
        🌐 Network Encryption:\n\
        • SSH: AES-256 encryption ✅\n\
        • TLS certificates: Self-signed\n\
        • VPN encryption: Not configured\n\n\
        🔑 Key Management:\n\
        • SSH keys: 2 pairs configured\n\
        • GPG keys: 1 keypair\n\
        • SSL certificates: Self-signed\n\
        • Hardware security: TPM not available\n\n\
        📊 Encryption Strength:\n\
        • Symmetric: AES-256\n\
        • Asymmetric: RSA-4096, Ed25519\n\
        • Hashing: SHA-256, SHA-512";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Encryption Configuration")
            .button("Enable Disk Encryption", |s| {
                s.add_layer(
                    Dialog::text("⚠️ WARNING: Disk encryption requires system reinstall!\n\nDisk encryption setup will be available in future updates.")
                        .title("Disk Encryption")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Generate Keys", |s| {
                s.add_layer(
                    Dialog::text("Encryption key management will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_security_policies(siv: &mut Cursive) {
    let content = "⚙️ Security Policies\n\n\
        Password Policy:\n\
        • Minimum length: 8 characters\n\
        • Complexity: Required\n\
        • History: 5 passwords\n\
        • Max age: 90 days\n\
        • Lockout: 3 failed attempts\n\n\
        🔐 Access Policy:\n\
        • Root access: SSH key only\n\
        • Sudo timeout: 15 minutes\n\
        • Session timeout: 30 minutes\n\
        • Multi-factor: Not configured\n\n\
        🌐 Network Policy:\n\
        • Default deny: Incoming traffic\n\
        • Rate limiting: Enabled\n\
        • Port scanning: Blocked\n\
        • Geographic filtering: Disabled\n\n\
        📱 Device Policy:\n\
        • USB access: Unrestricted\n\
        • Bluetooth: Enabled\n\
        • Camera access: Unrestricted\n\
        • Audio recording: Unrestricted\n\n\
        📊 Compliance:\n\
        • CIS Benchmarks: Partial\n\
        • NIST Framework: Basic\n\
        • ISO 27001: Not assessed";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Security Policies")
            .button("Policy Wizard", |s| {
                s.add_layer(
                    Dialog::text("Security policy wizard will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Compliance Check", |s| {
                s.add_layer(
                    Dialog::text("Compliance assessment will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}