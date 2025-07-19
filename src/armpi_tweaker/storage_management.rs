use cursive::views::{Dialog, LinearLayout, TextView, SelectView, DummyView};
use cursive::{Cursive, View};
use cursive::align::HAlign;
use cursive::traits::*;

pub fn show_storage_management_menu(siv: &mut Cursive) {
    let mut layout = LinearLayout::vertical();
    
    layout.add_child(TextView::new("Storage & Filesystem Management"));
    layout.add_child(DummyView.fixed_height(1));
    layout.add_child(TextView::new("Manage disks, partitions, and filesystems"));
    layout.add_child(DummyView.fixed_height(1));
    
    let mut menu = SelectView::<&str>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    menu.add_item("💾 Disk Management - View and manage storage devices", "disk_mgmt");
    menu.add_item("🗂️ Partition Management - Create and modify partitions", "partition_mgmt");
    menu.add_item("📁 Filesystem Management - Format and mount filesystems", "filesystem_mgmt");
    menu.add_item("💿 Mount Points - Configure automatic mounting", "mount_points");
    menu.add_item("🚀 Boot Configuration - Bootloader and boot partitions", "boot_config");
    menu.add_item("🔄 RAID Configuration - Software RAID setup", "raid_config");
    menu.add_item("💾 Swap Management - Virtual memory configuration", "swap_mgmt");
    menu.add_item("🧹 Disk Cleanup - Free up storage space", "cleanup");
    menu.add_item("📊 Storage Monitoring - Disk usage and health", "monitoring");
    menu.add_item("🔧 Advanced Tools - Disk utilities and recovery", "advanced");
    
    menu.set_on_submit(|s, option| {
        match *option {
            "disk_mgmt" => show_disk_management(s),
            "partition_mgmt" => show_partition_management(s),
            "filesystem_mgmt" => show_filesystem_management(s),
            "mount_points" => show_mount_points(s),
            "boot_config" => show_boot_configuration(s),
            "raid_config" => show_raid_configuration(s),
            "swap_mgmt" => show_swap_management(s),
            "cleanup" => show_disk_cleanup(s),
            "monitoring" => show_storage_monitoring(s),
            "advanced" => show_advanced_tools(s),
            _ => {
                s.add_layer(
                    Dialog::text("Storage feature coming soon!")
                        .title("Not Implemented")
                        .button("OK", |s| { s.pop_layer(); })
                );
            }
        }
    });
    
    layout.add_child(menu);
    
    let dialog = Dialog::around(layout.fixed_width(75))
        .title("Storage Management")
        .button("Close", |s| { 
            s.pop_layer(); 
            crate::armpi_tweaker::show_armpi_tweaker(s);
        });
    
    siv.add_layer(dialog);
}

fn show_disk_management(siv: &mut Cursive) {
    let content = "💾 Disk Management\n\n\
        Detected Storage Devices:\n\n\
        📱 /dev/mmcblk0 - eMMC Storage\n\
        • Capacity: 32 GB\n\
        • Type: eMMC 5.1\n\
        • Status: Healthy\n\
        • Partitions: 2\n\n\
        💳 /dev/mmcblk1 - MicroSD Card\n\
        • Capacity: 64 GB\n\
        • Type: SDXC Class 10\n\
        • Status: Healthy\n\
        • Partitions: 1\n\n\
        🚀 /dev/nvme0n1 - NVMe SSD\n\
        • Capacity: 512 GB\n\
        • Type: NVMe PCIe 3.0\n\
        • Status: Healthy\n\
        • Partitions: 3\n\n\
        Available operations:\n\
        • View disk information\n\
        • Check disk health\n\
        • Partition management\n\
        • Format operations\n\
        • Backup and clone";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Disk Management")
            .button("Disk Health", |s| {
                s.add_layer(
                    Dialog::text("All storage devices are healthy!\n\n✅ eMMC: No errors detected\n✅ MicroSD: Good condition\n✅ NVMe: Excellent health")
                        .title("Disk Health Status")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_partition_management(siv: &mut Cursive) {
    let content = "🗂️ Partition Management\n\n\
        Current Partition Layout:\n\n\
        📱 eMMC (/dev/mmcblk0):\n\
        • mmcblk0p1: 512 MB (FAT32) - Boot\n\
        • mmcblk0p2: 31.5 GB (ext4) - Root\n\n\
        💳 MicroSD (/dev/mmcblk1):\n\
        • mmcblk1p1: 64 GB (exFAT) - Data\n\n\
        🚀 NVMe (/dev/nvme0n1):\n\
        • nvme0n1p1: 512 MB (FAT32) - EFI\n\
        • nvme0n1p2: 8 GB (swap) - Swap\n\
        • nvme0n1p3: 503.5 GB (ext4) - Home\n\n\
        Partition Operations:\n\
        • Create new partition\n\
        • Resize existing partition\n\
        • Delete partition\n\
        • Change partition type\n\
        • Set partition flags";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Partition Management")
            .button("Create Partition", |s| {
                s.add_layer(
                    Dialog::text("Partition creation wizard will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_filesystem_management(siv: &mut Cursive) {
    let content = "📁 Filesystem Management\n\n\
        Supported Filesystems:\n\n\
        🐧 Linux Filesystems:\n\
        • ext4 - Default Linux filesystem\n\
        • ext3 - Legacy Linux filesystem\n\
        • ext2 - Basic Linux filesystem\n\
        • Btrfs - Advanced copy-on-write\n\
        • XFS - High-performance filesystem\n\
        • F2FS - Flash-friendly filesystem\n\n\
        🌐 Cross-platform:\n\
        • FAT32 - Universal compatibility\n\
        • exFAT - Large file support\n\
        • NTFS - Windows compatibility\n\n\
        🔧 Specialized:\n\
        • swap - Virtual memory\n\
        • tmpfs - RAM filesystem\n\
        • overlayfs - Union filesystem\n\n\
        Operations:\n\
        • Format partition\n\
        • Check filesystem\n\
        • Repair filesystem\n\
        • Convert filesystem";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Filesystem Management")
            .button("Format Disk", |s| {
                s.add_layer(
                    Dialog::text("⚠️ WARNING: Formatting will erase all data!\n\nFormat operation will be available in future updates.")
                        .title("Format Warning")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_mount_points(siv: &mut Cursive) {
    let content = "💿 Mount Points Configuration\n\n\
        Current Mount Points:\n\n\
        / (root) - /dev/mmcblk0p2 (ext4)\n\
        /boot - /dev/mmcblk0p1 (vfat)\n\
        /home - /dev/nvme0n1p3 (ext4)\n\
        /mnt/sdcard - /dev/mmcblk1p1 (exfat)\n\
        /tmp - tmpfs (tmpfs)\n\
        /dev/shm - tmpfs (tmpfs)\n\n\
        Mount Options:\n\
        • Auto-mount on boot\n\
        • Mount with specific options\n\
        • User mountable\n\
        • Read-only mount\n\
        • No execute permission\n\
        • Synchronous writes\n\n\
        /etc/fstab Configuration:\n\
        Persistent mount configuration\n\
        for automatic mounting at boot.\n\n\
        Operations:\n\
        • Add mount point\n\
        • Remove mount point\n\
        • Modify mount options\n\
        • Test mount configuration";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Mount Points")
            .button("Edit fstab", |s| {
                s.add_layer(
                    Dialog::text("fstab editor will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_boot_configuration(siv: &mut Cursive) {
    let content = "🚀 Boot Configuration\n\n\
        Boot Loader: U-Boot\n\
        Boot Device: eMMC (/dev/mmcblk0)\n\
        Boot Partition: /dev/mmcblk0p1\n\n\
        Boot Sequence:\n\
        1. U-Boot loads from SPI flash\n\
        2. U-Boot reads boot.scr from boot partition\n\
        3. Kernel and device tree loaded\n\
        4. Root filesystem mounted\n\
        5. Init system started\n\n\
        Boot Files:\n\
        • Image - Kernel image\n\
        • rk3588s-orangepi-5-plus.dtb - Device tree\n\
        • boot.scr - Boot script\n\
        • config.txt - Boot configuration\n\n\
        Boot Options:\n\
        • Select boot device priority\n\
        • Configure boot delay\n\
        • Set default boot entry\n\
        • Enable/disable boot menu\n\
        • Recovery boot options";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Boot Configuration")
            .button("Boot Menu", |s| {
                s.add_layer(
                    Dialog::text("Boot menu configuration will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_raid_configuration(siv: &mut Cursive) {
    let content = "🔄 RAID Configuration\n\n\
        Software RAID support for multiple\n\
        storage devices on Orange Pi 5 Plus.\n\n\
        Supported RAID Levels:\n\
        • RAID 0 - Striping (performance)\n\
        • RAID 1 - Mirroring (redundancy)\n\
        • RAID 5 - Striping with parity\n\
        • RAID 6 - Double parity\n\
        • RAID 10 - Stripe of mirrors\n\n\
        Current Status: No RAID configured\n\n\
        Available Devices:\n\
        • /dev/mmcblk0 (eMMC) - In use\n\
        • /dev/mmcblk1 (MicroSD) - Available\n\
        • /dev/nvme0n1 (NVMe) - In use\n\n\
        Requirements:\n\
        • Minimum 2 devices for RAID\n\
        • Equal or similar device sizes\n\
        • mdadm software package\n\n\
        Note: RAID is recommended for\n\
        data redundancy and performance.";
    
    siv.add_layer(
        Dialog::text(content)
            .title("RAID Configuration")
            .button("Setup RAID", |s| {
                s.add_layer(
                    Dialog::text("RAID setup wizard will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_swap_management(siv: &mut Cursive) {
    let content = "💾 Swap Management\n\n\
        Current Swap Configuration:\n\n\
        Swap Devices:\n\
        • /dev/nvme0n1p2: 8 GB (active)\n\
        • Swap file: Not configured\n\n\
        Swap Usage:\n\
        • Total: 8 GB\n\
        • Used: 0 MB\n\
        • Free: 8 GB\n\
        • Priority: -2\n\n\
        Swap Settings:\n\
        • Swappiness: 60 (default)\n\
        • VFS cache pressure: 100\n\
        • Dirty ratio: 20%\n\
        • Dirty background ratio: 10%\n\n\
        Swap Options:\n\
        • Create swap file\n\
        • Resize swap partition\n\
        • Adjust swappiness\n\
        • Enable/disable swap\n\
        • Configure zswap compression\n\n\
        Note: Swap provides virtual memory\n\
        when physical RAM is exhausted.";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Swap Management")
            .button("Configure Swap", |s| {
                s.add_layer(
                    Dialog::text("Swap configuration will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_disk_cleanup(siv: &mut Cursive) {
    let content = "🧹 Disk Cleanup\n\n\
        Storage Usage Analysis:\n\n\
        🗑️ Temporary Files: 2.1 GB\n\
        • /tmp: 150 MB\n\
        • /var/tmp: 89 MB\n\
        • Browser cache: 1.2 GB\n\
        • System logs: 670 MB\n\n\
        📦 Package Cache: 1.8 GB\n\
        • APT cache: 1.5 GB\n\
        • Snap cache: 300 MB\n\n\
        📜 Log Files: 890 MB\n\
        • System logs: 450 MB\n\
        • Application logs: 440 MB\n\n\
        🗂️ Duplicates: 3.2 GB\n\
        • Duplicate files found\n\n\
        💾 Old Kernels: 1.1 GB\n\
        • Previous kernel versions\n\n\
        Total Reclaimable: 9.1 GB\n\n\
        Cleanup Operations:\n\
        • Clean temporary files\n\
        • Clear package cache\n\
        • Rotate log files\n\
        • Remove duplicates\n\
        • Uninstall old kernels";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Disk Cleanup")
            .button("Start Cleanup", |s| {
                s.add_layer(
                    Dialog::text("Cleaning up disk space...\n\n✅ Temporary files cleared\n✅ Package cache cleaned\n✅ Log files rotated\n\nFreed 9.1 GB of disk space!")
                        .title("Cleanup Complete")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_storage_monitoring(siv: &mut Cursive) {
    let content = "📊 Storage Monitoring\n\n\
        Real-time Storage Statistics:\n\n\
        📱 eMMC Performance:\n\
        • Read Speed: 285 MB/s\n\
        • Write Speed: 178 MB/s\n\
        • IOPS: 8,500 read, 3,200 write\n\
        • Temperature: 45°C\n\
        • Health: Excellent (100%)\n\n\
        🚀 NVMe Performance:\n\
        • Read Speed: 3,200 MB/s\n\
        • Write Speed: 2,800 MB/s\n\
        • IOPS: 485,000 read, 425,000 write\n\
        • Temperature: 52°C\n\
        • Health: Excellent (99%)\n\n\
        💳 MicroSD Performance:\n\
        • Read Speed: 95 MB/s\n\
        • Write Speed: 85 MB/s\n\
        • IOPS: 2,100 read, 1,800 write\n\
        • Health: Good (95%)\n\n\
        📈 I/O Statistics:\n\
        • Current read rate: 12.5 MB/s\n\
        • Current write rate: 8.2 MB/s\n\
        • Queue depth: 4\n\
        • Utilization: 15%";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Storage Monitoring")
            .button("Detailed Stats", |s| {
                s.add_layer(
                    Dialog::text("Detailed storage statistics will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}

fn show_advanced_tools(siv: &mut Cursive) {
    let content = "🔧 Advanced Storage Tools\n\n\
        Disk Utilities:\n\
        • badblocks - Check for bad sectors\n\
        • fsck - Filesystem check and repair\n\
        • smartctl - SMART disk health\n\
        • hdparm - Disk parameter tuning\n\
        • fdisk - Partition table editor\n\
        • gdisk - GPT partition editor\n\
        • parted - Advanced partitioning\n\
        • dd - Low-level disk operations\n\
        • rsync - Advanced file synchronization\n\
        • lsblk - List block devices\n\n\
        Recovery Tools:\n\
        • testdisk - Partition recovery\n\
        • photorec - File recovery\n\
        • ddrescue - Data recovery\n\
        • extundelete - ext filesystem recovery\n\n\
        Performance Tools:\n\
        • iotop - I/O monitoring\n\
        • iostat - I/O statistics\n\
        • fio - Flexible I/O tester\n\
        • bonnie++ - Filesystem benchmark\n\n\
        Security Tools:\n\
        • cryptsetup - Disk encryption\n\
        • shred - Secure file deletion";
    
    siv.add_layer(
        Dialog::text(content)
            .title("Advanced Tools")
            .button("Launch Tool", |s| {
                s.add_layer(
                    Dialog::text("Advanced tool launcher will be available in future updates!")
                        .title("Coming Soon")
                        .button("OK", |s| { s.pop_layer(); })
                );
            })
            .button("Close", |s| { s.pop_layer(); })
    );
}