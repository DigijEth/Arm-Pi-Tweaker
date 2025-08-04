#include "imagebuilder.h"
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QJsonArray>
#include <QUuid>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>

// Constants
const QString ImageBuilder::BOARD_NAME = "Orange Pi 5 Plus";
const QString ImageBuilder::BOARD_MAKER = "Xulong";
const QString ImageBuilder::BOARD_SOC = "Rockchip RK3588";
const QString ImageBuilder::BOARD_CPU = "ARM Cortex A76 / A55";
const QString ImageBuilder::UBOOT_PACKAGE = "u-boot-radxa-rk3588";
const QString ImageBuilder::UBOOT_RULES_TARGET = "orangepi-5-plus-rk3588";
const QString ImageBuilder::KERNEL_FLAVOR = "rockchip";

const QString ImageBuilder::LIVECD_ROOTFS_URL = "https://github.com/Joshua-Riek/livecd-rootfs.git";
const QString ImageBuilder::LINUX_ROCKCHIP_URL = "https://github.com/Joshua-Riek/linux-rockchip.git";
const QString ImageBuilder::UBOOT_RADXA_URL = "https://github.com/radxa/u-boot.git";
const QString ImageBuilder::UBOOT_BRANCH = "next-dev-v2024.03";
const QString ImageBuilder::UBOOT_COMMIT = "f73b1eede495c82cd5d7ed20cc484a22d670136f";

ImageBuilder::ImageBuilder(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_progressTimer(new QTimer(this))
    , m_isBuilding(false)
    , m_progress(0)
    , m_totalSteps(6)
    , m_currentStepIndex(0)
    , m_currentBuildStep(StepInitialization)
    , m_config() // default construct, will set dirs below
{
    // Initialize build steps
    // Ensure default directories use invoking user home when run under sudo, not /root
    QString sudoUser = qgetenv("SUDO_USER");
    QString homePath = sudoUser.isEmpty() ? QDir::homePath() : QString("/home/%1").arg(QString(sudoUser));
    m_config.baseDir = homePath + "/tweaker";
    m_config.sourcesDir = m_config.baseDir + "/sources";
    m_config.buildDir = m_config.baseDir + "/build";
    m_config.outputDir = m_config.baseDir + "/images";
    m_buildSteps << "Initialization" << "Kernel Build" << "U-Boot Build" 
                 << "Rootfs Build" << "Image Configuration" << "Image Creation";
    
    connect(m_progressTimer, &QTimer::timeout, [this]() {
        // Periodic progress updates during long operations
        if (m_isBuilding && m_progress < 95) {
            updateProgress(m_progress + 1);
        }
    });
}

ImageBuilder::~ImageBuilder()
{
    cancelBuild();
}

void ImageBuilder::setConfiguration(const BuildConfiguration &config)
{
    if (m_isBuilding) {
        logError("Cannot change configuration while build is in progress");
        return;
    }
    
    m_config = config;
    // Override sourcesDir with custom download location if set
    if (!m_downloadDir.isEmpty()) {
        m_config.sourcesDir = m_downloadDir;
    }
    // Update directory paths
    m_livecdRootfsDir = m_config.sourcesDir + "/livecd-rootfs";
    m_linuxRockchipDir = m_config.sourcesDir + "/linux-rockchip";
    m_ubootSourceDir = m_config.buildDir + "/u-boot-source";
    m_buildRootfsDir = m_config.buildDir + "/rootfs";
    m_chrootDir = m_config.buildDir + "/chroot";
}

QString ImageBuilder::suiteToString(Suite suite)
{
    switch (suite) {
        case Jammy: return "jammy";
        case Noble: return "noble";
        case Oracular: return "oracular";
        case Plucky: return "plucky";
        default: return "noble";
    }
}

QString ImageBuilder::flavorToString(Flavor flavor)
{
    switch (flavor) {
        case Desktop: return "desktop";
        case Server: return "server";
        default: return "desktop";
    }
}

ImageBuilder::Suite ImageBuilder::stringToSuite(const QString &suiteStr)
{
    if (suiteStr == "jammy") return Jammy;
    if (suiteStr == "noble") return Noble;
    if (suiteStr == "oracular") return Oracular;
    if (suiteStr == "plucky") return Plucky;
    return Noble; // default
}

ImageBuilder::Flavor ImageBuilder::stringToFlavor(const QString &flavorStr)
{
    if (flavorStr == "desktop") return Desktop;
    if (flavorStr == "server") return Server;
    return Desktop; // default
}

QString ImageBuilder::partitionTypeToString(PartitionType partitionType)
{
    switch (partitionType) {
        case EXT4: return "ext4";
        case F2FS: return "f2fs";
        default: return "ext4";
    }
}

ImageBuilder::PartitionType ImageBuilder::stringToPartitionType(const QString &partitionStr)
{
    if (partitionStr == "f2fs") return F2FS;
    if (partitionStr == "ext4") return EXT4;
    return EXT4; // default
}

QString ImageBuilder::getLivecdRootfsBranch(Suite suite) const
{
    switch (suite) {
        case Jammy: return "jammy";
        case Noble: return "main";
        case Oracular: return "oracular";
        case Plucky: return "upstream";
        default: return "main";
    }
}

QString ImageBuilder::getKernelBranch(Suite suite) const
{
    return suiteToString(suite); // Kernel branch matches suite name
}

QStringList ImageBuilder::getPPAsForSuite(Suite suite) const
{
    QStringList ppas;
    
    switch (suite) {
        case Jammy:
        case Noble:
            ppas << "ppa:jjriek/rockchip" 
                 << "ppa:jjriek/rockchip-multimedia"
                 << "ppa:jjriek/panfork-mesa";
            break;
        case Oracular:
        case Plucky:
            ppas << "ppa:jjriek/rockchip";
            break;
    }
    
    return ppas;
}

void ImageBuilder::startBuild()
{
    if (m_isBuilding) {
        logError("Build already in progress");
        return;
    }
    
    if (!hasRequiredTools()) {
        logError("Required build tools are not available");
        return;
    }
    
    m_isBuilding = true;
    m_progress = 0;
    m_currentStepIndex = 0;
    m_currentBuildStep = StepInitialization;
    
    emit buildStarted();
    logMessage("Starting build process for " + BOARD_NAME);
    logMessage("Suite: " + suiteToString(m_config.suite) + ", Flavor: " + flavorToString(m_config.flavor));
    
    initializeBuild();
}

void ImageBuilder::startKernelOnlyBuild()
{
    if (m_isBuilding) {
        logError("Build already in progress");
        return;
    }
    
    m_config.buildMode = KernelOnly;
    m_totalSteps = 2; // Initialization + Kernel Build
    m_buildSteps.clear();
    m_buildSteps << "Initialization" << "Kernel Build";
    
    startBuild();
}

void ImageBuilder::cancelBuild()
{
    if (!m_isBuilding) return;
    
    m_isBuilding = false;
    m_progressTimer->stop();
    
    if (m_process) {
        m_process->terminate();
        if (!m_process->waitForFinished(5000)) {
            m_process->kill();
        }
        m_process->deleteLater();
        m_process = nullptr;
    }
    
    logMessage("Build cancelled by user");
    emit buildCompleted(false, "Build cancelled");
}

void ImageBuilder::initializeBuild()
{
    setCurrentStep("Initialization", "Setting up build environment");
    
    try {
        setupBuildEnvironment();
        createDirectories();
        
        if (m_config.buildMode == KernelOnly) {
            setupKernelSource();
        } else {
            // Setup all source repositories for full build
            cloneRepository(LIVECD_ROOTFS_URL, getLivecdRootfsBranch(m_config.suite), m_livecdRootfsDir);
        }
        
        updateProgress(15);
        
        // Move to next step based on build mode
        if (m_config.buildMode == KernelOnly) {
            buildKernel();
        } else {
            setupKernelSource();
        }
        
    } catch (const std::exception &e) {
        logError("Initialization failed: " + QString(e.what()));
        emit buildCompleted(false, "Initialization failed");
        m_isBuilding = false;
    }
}

void ImageBuilder::setupKernelSource()
{
    logMessage("Setting up kernel source...");
    
    switch (m_config.kernelSource) {
        case RemoteKernel: {
            QString branch = getKernelBranch(m_config.suite);
            cloneRepository(LINUX_ROCKCHIP_URL, branch, m_linuxRockchipDir);
            break;
        }
        case LocalKernel: {
            if (!validateKernelSource()) {
                logError("Invalid local kernel source path: " + m_config.localKernelPath);
                emit buildCompleted(false, "Invalid kernel source");
                m_isBuilding = false;
                return;
            }
            m_linuxRockchipDir = m_config.localKernelPath;
            logMessage("Using local kernel source: " + m_linuxRockchipDir);
            break;
        }
        case CustomGitKernel: {
            QString branch = m_config.customKernelBranch.isEmpty() ? "main" : m_config.customKernelBranch;
            cloneRepository(m_config.customKernelGitUrl, branch, m_linuxRockchipDir);
            break;
        }
    }
    
    if (m_config.buildMode == FullBuild) {
        buildKernel();
    }
}

bool ImageBuilder::validateKernelSource()
{
    if (m_config.localKernelPath.isEmpty()) return false;
    
    QDir kernelDir(m_config.localKernelPath);
    if (!kernelDir.exists()) return false;
    
    // Check for essential kernel files
    return kernelDir.exists("Makefile") && 
           kernelDir.exists("arch") && 
           kernelDir.exists("drivers");
}

void ImageBuilder::buildKernel()
{
    m_currentBuildStep = StepKernelBuild;
    setCurrentStep("Kernel Build", "Building Linux kernel for RK3588");
    
    QStringList commands;
    QString suite = suiteToString(m_config.suite);
    QString defconfig = "rockchip_linux_defconfig";
    
    // Clean build if requested
    if (m_config.cleanBuild) {
        commands << QString("cd %1 && make clean").arg(m_linuxRockchipDir);
    }
    
    // Configure kernel
    commands << QString("cd %1 && make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- %2")
                    .arg(m_linuxRockchipDir).arg(defconfig);
    
    // Build kernel
    commands << QString("cd %1 && make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) Image modules dtbs")
                    .arg(m_linuxRockchipDir);
    
    // Install modules to build directory
    commands << QString("cd %1 && make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=%2/kernel-modules modules_install")
                    .arg(m_linuxRockchipDir).arg(m_config.buildDir);
    
    // Create kernel package directory structure
    commands << QString("mkdir -p %1/kernel-package").arg(m_config.buildDir);
    
    // Copy kernel image and dtbs
    commands << QString("cp %1/arch/arm64/boot/Image %2/kernel-package/")
                    .arg(m_linuxRockchipDir).arg(m_config.buildDir);
    commands << QString("cp %1/arch/arm64/boot/dts/rockchip/rk3588-orangepi-5-plus.dtb %2/kernel-package/")
                    .arg(m_linuxRockchipDir).arg(m_config.buildDir);
    
    executeCommand(commands.join(" && "), m_config.buildDir);
}

void ImageBuilder::buildUBoot()
{
    if (m_config.buildMode == KernelOnly) {
        // Skip U-Boot for kernel-only builds
        if (m_currentStepIndex < m_totalSteps - 1) {
            buildRootfs();
        } else {
            emit buildCompleted(true, "Kernel build completed successfully");
            m_isBuilding = false;
        }
        return;
    }
    
    m_currentBuildStep = StepUBootBuild;
    setCurrentStep("U-Boot Build", "Building U-Boot bootloader");
    
    QStringList commands;
    
    // Prepare U-Boot source directory and ensure clean clone
    commands << QString("mkdir -p %1").arg(m_ubootSourceDir);
    // Remove existing u-boot directory if present
    commands << QString("if [ -d %1/u-boot ]; then rm -rf %1/u-boot; fi").arg(m_ubootSourceDir);
    // Clone fresh U-Boot repository
    commands << QString("cd %1 && git clone --depth 1 --branch %2 %3 u-boot")
                    .arg(m_ubootSourceDir).arg(UBOOT_BRANCH).arg(UBOOT_RADXA_URL);
    
    // Build U-Boot
    commands << QString("cd %1/u-boot && make distclean").arg(m_ubootSourceDir);
    commands << QString("cd %1/u-boot && make %2_defconfig").arg(m_ubootSourceDir).arg(UBOOT_RULES_TARGET);
    commands << QString("cd %1/u-boot && make CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)").arg(m_ubootSourceDir);
    
    executeCommand(commands.join(" && "), m_config.buildDir);
}

void ImageBuilder::buildRootfs()
{
    if (m_config.buildMode == KernelOnly) {
        emit buildCompleted(true, "Kernel-only build completed successfully");
        m_isBuilding = false;
        return;
    }
    
    m_currentBuildStep = StepRootfsBuild;
    setCurrentStep("Rootfs Build", "Creating Ubuntu root filesystem");
    
    QString suite = suiteToString(m_config.suite);
    QString flavor = flavorToString(m_config.flavor);
    QString version = getVersionString();
    
    QStringList commands;
    
    // Create temporary directory for livecd-rootfs work
    commands << QString("TMP_DIR=$(mktemp -d)");
    commands << QString("cd $TMP_DIR");
    
    // Install livecd-rootfs build dependencies
    commands << "apt-get update";
    commands << QString("cd %1 && apt-get build-dep . -y").arg(m_livecdRootfsDir);
    
    // Build livecd-rootfs package
    commands << QString("cd %1 && dpkg-buildpackage -us -uc").arg(m_livecdRootfsDir);
    
    // Install custom livecd-rootfs
    commands << QString("apt-get install %1/../livecd-rootfs_*.deb --assume-yes --allow-downgrades --allow-change-held-packages")
                    .arg(m_livecdRootfsDir);
    
    // Create live-build workspace
    commands << QString("mkdir -p %1/live-build && cd %1/live-build").arg(m_config.buildDir);
    
    // Copy auto scripts
    commands << QString("cp -r $(dpkg -L livecd-rootfs | grep 'auto$') auto");
    
    // Configure live-build
    QString lbConfig = QString("lb config "
        "--architecture arm64 "
        "--bootstrap-qemu-arch arm64 "
        "--bootstrap-qemu-static /usr/bin/qemu-aarch64-static "
        "--archive-areas 'main restricted universe multiverse' "
        "--parent-archive-areas 'main restricted universe multiverse' "
        "--mirror-bootstrap 'http://ports.ubuntu.com' "
        "--parent-mirror-bootstrap 'http://ports.ubuntu.com' "
        "--mirror-chroot-security 'http://ports.ubuntu.com' "
        "--parent-mirror-chroot-security 'http://ports.ubuntu.com' "
        "--mirror-binary-security 'http://ports.ubuntu.com' "
        "--parent-mirror-binary-security 'http://ports.ubuntu.com' "
        "--mirror-binary 'http://ports.ubuntu.com' "
        "--parent-mirror-binary 'http://ports.ubuntu.com' "
        "--keyring-packages ubuntu-keyring "
        "--linux-flavours %1").arg(KERNEL_FLAVOR);
    
    commands << lbConfig;
    
    // Configure PPAs
    configurePPAs(m_config.buildDir + "/live-build");
    
    // Configure snap packages
    configureSnapPackages(m_config.buildDir + "/live-build");
    
    // Configure package lists
    configurePackageList(m_config.buildDir + "/live-build");
    
    // Build rootfs
    commands << "lb build";
    
    // Package rootfs
    QString rootfsFile = QString("ubuntu-%1-preinstalled-%2-arm64.rootfs.tar.xz").arg(version).arg(flavor);
    commands << QString("(cd chroot/ && tar -p -c --sort=name --xattrs ./*) | xz -3 -T0 > %1").arg(rootfsFile);
    commands << QString("mv %1 %2/").arg(rootfsFile).arg(m_config.buildDir);
    
    executeCommand(commands.join(" && "), m_config.buildDir);
}

void ImageBuilder::configureImage()
{
    m_currentBuildStep = StepImageConfig;
    setCurrentStep("Image Configuration", "Configuring system packages and settings");
    
    QString version = getVersionString();
    QString flavor = flavorToString(m_config.flavor);
    QString rootfsFile = QString("ubuntu-%1-preinstalled-%2-arm64.rootfs.tar.xz").arg(version).arg(flavor);
    
    QStringList commands;
    
    // Extract rootfs
    commands << QString("rm -rf %1 && mkdir -p %1").arg(m_chrootDir);
    commands << QString("tar -xpJf %1/%2 -C %3").arg(m_config.buildDir).arg(rootfsFile).arg(m_chrootDir);
    
    // Setup chroot environment
    commands << QString("mount dev-live -t devtmpfs %1/dev").arg(m_chrootDir);
    commands << QString("mount devpts-live -t devpts -o nodev,nosuid %1/dev/pts").arg(m_chrootDir);
    commands << QString("mount proc-live -t proc %1/proc").arg(m_chrootDir);
    commands << QString("mount sysfs-live -t sysfs %1/sys").arg(m_chrootDir);
    commands << QString("mount -t tmpfs none %1/tmp").arg(m_chrootDir);
    commands << QString("mount -t tmpfs none %1/var/lib/apt/lists").arg(m_chrootDir);
    commands << QString("mount -t tmpfs none %1/var/cache/apt").arg(m_chrootDir);
    
    // Copy resolv.conf
    commands << QString("cp /etc/resolv.conf %1/etc/resolv.conf").arg(m_chrootDir);
    
    // Update packages
    commands << QString("chroot %1 apt-get update").arg(m_chrootDir);
    commands << QString("chroot %1 apt-get -y upgrade").arg(m_chrootDir);
    
    // Apply board-specific configuration
    applyBoardSpecificHook(m_chrootDir);
    
    // Install kernel packages
    QString kernelPackageDir = m_config.buildDir + "/kernel-package";
    commands << QString("cp %1/Image %2/boot/").arg(kernelPackageDir).arg(m_chrootDir);
    commands << QString("cp %1/*.dtb %2/boot/").arg(kernelPackageDir).arg(m_chrootDir);
    
    // Install kernel modules
    QString moduleDir = m_config.buildDir + "/kernel-modules";
    commands << QString("if [ -d %1 ]; then cp -r %1/* %2/; fi").arg(moduleDir).arg(m_chrootDir);
    
    // Update initramfs
    commands << QString("chroot %1 update-initramfs -u").arg(m_chrootDir);
    
    // Clean up
    commands << QString("chroot %1 apt-get -y clean").arg(m_chrootDir);
    commands << QString("chroot %1 apt-get -y autoclean").arg(m_chrootDir);
    commands << QString("chroot %1 apt-get -y autoremove").arg(m_chrootDir);
    
    // Unmount chroot
    commands << QString("umount -lf %1/dev/pts %1/dev %1/proc %1/sys %1/tmp %1/var/lib/apt/lists %1/var/cache/apt || true").arg(m_chrootDir);
    
    // Create final rootfs tar
    QString finalRootfs = QString("ubuntu-%1-preinstalled-%2-arm64-%3.rootfs.tar")
                             .arg(version).arg(flavor).arg(m_config.board);
    commands << QString("cd %1 && tar -cpf %2/%3 .").arg(m_chrootDir).arg(m_config.buildDir).arg(finalRootfs);
    
    executeCommand(commands.join(" && "), m_config.buildDir);
}

void ImageBuilder::createDiskImage()
{
    m_currentBuildStep = StepImageCreation;
    setCurrentStep("Image Creation", "Creating bootable disk image");
    
    QString imageFile = getImageFileName();
    QString imagePath = m_config.outputDir + "/" + imageFile;
    
    QString version = getVersionString();
    QString flavor = flavorToString(m_config.flavor);
    QString rootfsTar = QString("ubuntu-%1-preinstalled-%2-arm64-%3.rootfs.tar")
                           .arg(version).arg(flavor).arg(m_config.board);
    QString rootfsPath = m_config.buildDir + "/" + rootfsTar;
    
    QStringList commands;
    
    // Calculate image size
    commands << QString("SIZE=$(( $(wc -c < %1) / 1024 / 1024 ))").arg(rootfsPath);
    commands << QString("truncate -s $(( SIZE + 2048 ))M %1").arg(imagePath);
    
    // Setup loop device
    commands << "LOOP_DEV=$(losetup -f)";
    commands << QString("losetup -P $LOOP_DEV %1").arg(imagePath);
    
    // Create partitions based on flavor and filesystem type
    if (m_config.partitionType == F2FS) {
        createF2FSPartitions(imagePath, "$LOOP_DEV");
    } else if (m_config.flavor == Desktop) {
        createDesktopPartitions(imagePath, "$LOOP_DEV");
    } else {
        createServerPartitions(imagePath, "$LOOP_DEV");
    }
    
    // Install bootloader
    installBootloader(imagePath, "$LOOP_DEV");
    
    // Cleanup
    commands << "umount ${LOOP_DEV}p* 2>/dev/null || true";
    commands << "losetup -d $LOOP_DEV";
    
    // Compress image
    commands << QString("cd %1 && xz -6 --force --keep --quiet --threads=0 %2").arg(m_config.outputDir).arg(imageFile);
    commands << QString("rm -f %1").arg(imagePath);
    
    // Generate checksum
    commands << QString("cd %1 && sha256sum %2.xz > %2.xz.sha256").arg(m_config.outputDir).arg(imageFile);
    
    executeCommand(commands.join(" && "), m_config.buildDir);
}

void ImageBuilder::createDesktopPartitions(const QString &imagePath, const QString &loopDevice)
{
    QStringList commands;
    QString fsType = getFilesystemTypeString();
    QString label = getPartitionLabel();
    
    // Create GPT partition table
    commands << QString("parted --script %1 mklabel gpt").arg(loopDevice);
    commands << QString("parted --script %1 mkpart primary %2 16MiB 100%").arg(loopDevice).arg(fsType);
    
    // Set partition type
    commands << QString("fdisk %1 <<EOF\nt\n1\nC12A7328-F81F-11D2-BA4B-00A0C93EC93B\nw\nEOF").arg(loopDevice);
    
    commands << "partprobe $LOOP_DEV";
    commands << "sleep 2";
    
    // Generate UUID and format
    commands << "ROOT_UUID=$(uuidgen)";
    if (m_config.partitionType == F2FS) {
        commands << QString("mkfs.f2fs -f -l %1 ${LOOP_DEV}p1").arg(label);
    } else {
        commands << QString("mkfs.ext4 -U $ROOT_UUID -L %1 ${LOOP_DEV}p1").arg(label);
    }
    
    // Mount and extract rootfs
    QString mountDir = m_config.baseDir + "/mnt";
    commands << QString("mkdir -p %1/writable").arg(mountDir);
    commands << QString("mount ${LOOP_DEV}p1 %1/writable").arg(mountDir);
    
    QString rootfsTar = QString("ubuntu-%1-preinstalled-%2-arm64-%3.rootfs.tar")
                           .arg(getVersionString()).arg(flavorToString(m_config.flavor)).arg(m_config.board);
    commands << QString("tar -xpf %1/%2 -C %3/writable").arg(m_config.buildDir).arg(rootfsTar).arg(mountDir);
    
    // Create fstab
    commands << QString("echo '# <file system> <mount point> <type> <options> <dump> <fsck>' > %1/writable/etc/fstab").arg(mountDir);
    
    if (m_config.partitionType == F2FS) {
        commands << QString("echo 'LABEL=%1 / f2fs defaults,x-systemd.growfs 0 1' >> %2/writable/etc/fstab").arg(label).arg(mountDir);
    } else {
        commands << QString("echo 'UUID=$ROOT_UUID / ext4 defaults,x-systemd.growfs 0 1' >> %1/writable/etc/fstab").arg(mountDir);
    }
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::createServerPartitions(const QString &imagePath, const QString &loopDevice)
{
    QStringList commands;
    
    // Create GPT with boot and root partitions
    commands << QString("parted --script %1 mklabel gpt").arg(loopDevice);
    commands << QString("parted --script %1 mkpart primary fat32 16MiB 20MiB").arg(loopDevice);
    commands << QString("parted --script %1 mkpart primary ext4 20MiB 100%").arg(loopDevice);
    
    commands << "partprobe $LOOP_DEV";
    commands << "sleep 2";
    
    // Generate UUIDs and format
    commands << "BOOT_UUID=$(uuidgen | head -c8)";
    commands << "ROOT_UUID=$(uuidgen)";
    commands << "mkfs.vfat -i $BOOT_UUID -F32 -n CIDATA ${LOOP_DEV}p1";
    commands << "mkfs.ext4 -U $ROOT_UUID -L cloudimg-rootfs ${LOOP_DEV}p2";
    
    // Mount partitions
    QString mountDir = m_config.baseDir + "/mnt";
    commands << QString("mkdir -p %1/{system-boot,writable}").arg(mountDir);
    commands << QString("mount ${LOOP_DEV}p1 %1/system-boot").arg(mountDir);
    commands << QString("mount ${LOOP_DEV}p2 %1/writable").arg(mountDir);
    
    // Extract rootfs
    QString rootfsTar = QString("ubuntu-%1-preinstalled-%2-arm64-%3.rootfs.tar")
                           .arg(getVersionString()).arg(flavorToString(m_config.flavor)).arg(m_config.board);
    commands << QString("tar -xpf %1/%2 -C %3/writable").arg(m_config.buildDir).arg(rootfsTar).arg(mountDir);
    
    // Create fstab
    commands << QString("echo '# <file system> <mount point> <type> <options> <dump> <fsck>' > %1/writable/etc/fstab").arg(mountDir);
    commands << QString("echo 'UUID=$ROOT_UUID / ext4 defaults,x-systemd.growfs 0 1' >> %1/writable/etc/fstab").arg(mountDir);
    
    // Copy cloud-init files (placeholder - would need actual files)
    commands << QString("echo 'instance-id: i-$(uuidgen)' > %1/system-boot/meta-data").arg(mountDir);
    commands << QString("echo '#cloud-config' > %1/system-boot/user-data").arg(mountDir);
    commands << QString("echo 'version: 2' > %1/system-boot/network-config").arg(mountDir);
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::createF2FSPartitions(const QString &imagePath, const QString &loopDevice)
{
    QStringList commands;
    QString label = getPartitionLabel();
    
    // Create GPT partition table
    commands << QString("parted --script %1 mklabel gpt").arg(loopDevice);
    commands << QString("parted --script %1 mkpart primary f2fs 16MiB 100%").arg(loopDevice);
    
    commands << "partprobe $LOOP_DEV";
    commands << "sleep 2";
    
    // Generate UUID and format with F2FS
    commands << "ROOT_UUID=$(uuidgen)";
    commands << QString("mkfs.f2fs -f -l %1 ${LOOP_DEV}p1").arg(label);
    
    // Mount partition  
    QString mountDir = m_config.baseDir + "/mnt";
    commands << QString("mkdir -p %1/writable").arg(mountDir);
    commands << QString("mount ${LOOP_DEV}p1 %1/writable").arg(mountDir);
    
    // Extract rootfs
    QString rootfsTar = QString("ubuntu-%1-preinstalled-%2-arm64-%3.rootfs.tar")
                           .arg(getVersionString()).arg(flavorToString(m_config.flavor)).arg(m_config.board);
    commands << QString("tar -xpf %1/%2 -C %3/writable").arg(m_config.buildDir).arg(rootfsTar).arg(mountDir);
    
    // Create fstab for F2FS with compression support
    commands << QString("echo '# <file system> <mount point> <type> <options> <dump> <fsck>' > %1/writable/etc/fstab").arg(mountDir);
    commands << QString("echo 'UUID=$ROOT_UUID / f2fs defaults,compress_algorithm=lz4,compress_chksum,atgc,gc_merge,lazytime 0 0' >> %1/writable/etc/fstab").arg(mountDir);
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::installBootloader(const QString &imagePath, const QString &loopDevice)
{
    QStringList commands;
    
    // Install U-Boot bootloader
    QString ubootBin = m_ubootSourceDir + "/u-boot/u-boot-rockchip.bin";
    commands << QString("if [ -f %1 ]; then dd if=%1 of=%2 seek=1 bs=32k conv=fsync; fi")
                    .arg(ubootBin).arg(loopDevice);
    
    // Alternative installation method
    QString idbloader = m_ubootSourceDir + "/u-boot/idbloader.img";
    QString ubootItb = m_ubootSourceDir + "/u-boot/u-boot.itb";
    commands << QString("if [ -f %1 ] && [ -f %2 ]; then")
                    .arg(idbloader).arg(ubootItb);
    commands << QString("  dd if=%1 of=%2 seek=64 conv=notrunc;").arg(idbloader).arg(loopDevice);
    commands << QString("  dd if=%1 of=%2 seek=16384 conv=notrunc;").arg(ubootItb).arg(loopDevice);
    commands << "fi";
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::configurePPAs(const QString &chrootDir)
{
    QStringList ppas = getPPAsForSuite(m_config.suite);
    if (ppas.isEmpty()) return;
    
    QStringList commands;
    QString prefFile = chrootDir + "/config/archives/extra-ppas.pref.chroot";
    
    // Create PPA preferences
    commands << QString("mkdir -p %1/config/archives").arg(chrootDir);
    
    for (const QString &ppa : ppas) {
        QString ppaName = ppa.mid(4); // Remove "ppa:" prefix
        commands << QString("echo 'Package: *' >> %1").arg(prefFile);
        commands << QString("echo 'Pin: release o=LP-PPA-%1' >> %1").arg(ppaName.replace("/", "-")).arg(prefFile);
        commands << QString("echo 'Pin-Priority: 1001' >> %1").arg(prefFile);
        commands << QString("echo '' >> %1").arg(prefFile);
    }
    
    // Noble-specific exclusions
    if (m_config.suite == Noble) {
        QString ignoreFile = chrootDir + "/config/archives/extra-ppas-ignore.pref.chroot";
        commands << QString("echo 'Package: oem-*' > %1").arg(ignoreFile);
        commands << QString("echo 'Pin: release o=LP-PPA-jjriek-rockchip-multimedia' >> %1").arg(ignoreFile);
        commands << QString("echo 'Pin-Priority: -1' >> %1").arg(ignoreFile);
        commands << QString("echo '' >> %1").arg(ignoreFile);
        commands << QString("echo 'Package: ubiquity*' >> %1").arg(ignoreFile);
        commands << QString("echo 'Pin: release o=LP-PPA-jjriek-rockchip-multimedia' >> %1").arg(ignoreFile);
        commands << QString("echo 'Pin-Priority: -1' >> %1").arg(ignoreFile);
    }
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::configureSnapPackages(const QString &chrootDir)
{
    QStringList commands;
    QString snapFile = chrootDir + "/config/seeded-snaps";
    
    commands << QString("mkdir -p %1/config").arg(chrootDir);
    
    if (m_config.suite == Oracular) {
        // Oracular-specific snaps
        commands << QString("echo 'snapd/classic=stable' > %1").arg(snapFile);
        commands << QString("echo 'snap-store/classic=stable' >> %1").arg(snapFile);
        commands << QString("echo 'firefox/latest=stable' >> %1").arg(snapFile);
        commands << QString("echo 'thunderbird/latest=stable' >> %1").arg(snapFile);
        commands << QString("echo 'core22/classic=stable' >> %1").arg(snapFile);
        commands << QString("echo 'lxd/classic=stable' >> %1").arg(snapFile);
    } else {
        // Default snaps for other suites
        commands << QString("echo 'snapd/classic=stable' > %1").arg(snapFile);
        commands << QString("echo 'core22/classic=stable' >> %1").arg(snapFile);
        commands << QString("echo 'lxd/classic=stable' >> %1").arg(snapFile);
    }
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::configurePackageList(const QString &chrootDir)
{
    QStringList commands;
    QString packageFile = chrootDir + "/config/package-lists/my.list.chroot";
    
    commands << QString("mkdir -p %1/config/package-lists").arg(chrootDir);
    commands << QString("echo 'software-properties-common' > %1").arg(packageFile);
    
    if (m_config.flavor == Desktop) {
        commands << QString("echo 'ubuntu-desktop-rockchip' >> %1").arg(packageFile);
        commands << QString("echo 'oem-config-gtk' >> %1").arg(packageFile);
        commands << QString("echo 'ubiquity-frontend-gtk' >> %1").arg(packageFile);
        commands << QString("echo 'ubiquity-slideshow-ubuntu' >> %1").arg(packageFile);
        commands << QString("echo 'localechooser-data' >> %1").arg(packageFile);
    } else {
        commands << QString("echo 'ubuntu-server-rockchip' >> %1").arg(packageFile);
    }
    
    executeCommand(commands.join(" && "));
}

void ImageBuilder::applyBoardSpecificHook(const QString &chrootDir)
{
    // Only apply hardware-specific configuration for jammy and noble
    if (m_config.suite != Jammy && m_config.suite != Noble) {
        return;
    }
    
    QStringList commands;
    
    if (m_config.includeGpuDrivers) {
        // Add panfork PPA and install Mali packages
        commands << QString("install -m 0755 -d %1/etc/apt/keyrings/").arg(chrootDir);
        commands << QString("gpg --no-default-keyring --keyring /usr/share/keyrings/ubuntu-archive-keyring.gpg --export | gpg --no-default-keyring --keyring trustedkeys.gpg --import");
        commands << QString("chroot %1 apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 52B544131B704542");
        commands << QString("chroot %1 add-apt-repository -y ppa:jjriek/panfork-mesa").arg(chrootDir);
        commands << QString("chroot %1 apt-get update").arg(chrootDir);
        commands << QString("chroot %1 apt-get -y install mali-g610-firmware").arg(chrootDir);
        commands << QString("chroot %1 apt-get -y dist-upgrade").arg(chrootDir);
        commands << QString("chroot %1 apt-get -y install libmali-g610-x11").arg(chrootDir);
    }
    
    if (m_config.includeCameraEngine) {
        commands << QString("chroot %1 apt-get -y install camera-engine-rkaiq-rk3588").arg(chrootDir);
    }
    
    if (m_config.includeWiringPi) {
        commands << QString("chroot %1 apt-get -y install wiringpi-opi libwiringpi2-opi libwiringpi-opi-dev").arg(chrootDir);
        commands << QString("echo 'BOARD=orangepi5plus' > %1/etc/orangepi-release").arg(chrootDir);
    }
    
    if (m_config.includeWifi && m_config.includeBluetooth) {
        // Create RTL8852BE WiFi/Bluetooth service files (placeholder)
        commands << QString("mkdir -p %1/usr/lib/systemd/system %1/usr/lib/scripts").arg(chrootDir);
        commands << QString("echo '[Unit]' > %1/usr/lib/systemd/system/rtl8852be-reload.service").arg(chrootDir);
        commands << QString("echo 'Description=RTL8852BE reload service' >> %1/usr/lib/systemd/system/rtl8852be-reload.service").arg(chrootDir);
        commands << QString("echo '#!/bin/bash' > %1/usr/lib/scripts/rtl8852be-reload.sh").arg(chrootDir);
        commands << QString("chmod +x %1/usr/lib/scripts/rtl8852be-reload.sh").arg(chrootDir);
        commands << QString("chroot %1 systemctl enable rtl8852be-reload").arg(chrootDir);
    }
    
    if (!commands.isEmpty()) {
        executeCommand(commands.join(" && "));
    }
}

void ImageBuilder::cloneRepository(const QString &url, const QString &branch, const QString &targetDir)
{
    if (QDir(targetDir).exists()) {
        logMessage(QString("Directory %1 already exists. Removing it.").arg(targetDir));
        if (!QDir(targetDir).removeRecursively()) {
            logError(QString("Failed to remove existing directory: %1").arg(targetDir));
            emit buildCompleted(false, "Failed to remove existing directory.");
            m_isBuilding = false;
            return;
        }
    }

    logMessage(QString("Cloning %1 (branch: %2) to %3").arg(url).arg(branch).arg(targetDir));

    QString command = QString("git clone --depth 1 --branch %1 %2 %3")
                         .arg(branch).arg(url).arg(targetDir);

    executeCommand(command);
}

void ImageBuilder::setDownloadLocation(const QString &path)
{
    if (m_isBuilding) {
        logError("Cannot change download location while build is in progress");
        return;
    }
    // Set custom download directory
    m_downloadDir = path;
    logMessage("Custom download location set to: " + m_downloadDir);
    // Recalculate source directories
    m_config.sourcesDir = m_downloadDir;
    m_livecdRootfsDir = m_config.sourcesDir + "/livecd-rootfs";
    m_linuxRockchipDir = m_config.sourcesDir + "/linux-rockchip";
}

void ImageBuilder::setupBuildEnvironment()
{
    // Ensure we have root privileges
    QString whoami = "whoami";
    QProcess checkUser;
    checkUser.start("bash", QStringList() << "-c" << whoami);
    checkUser.waitForFinished();
    
    if (checkUser.readAllStandardOutput().trimmed() != "root") {
        throw std::runtime_error("Root privileges required for building");
    }
    
    // Set environment variables
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DEBIAN_FRONTEND", "noninteractive");
    env.insert("LC_ALL", "C");
    
    if (m_process) {
        m_process->setProcessEnvironment(env);
    }
}

void ImageBuilder::createDirectories()
{
    QDir().mkpath(m_config.baseDir);
    QDir().mkpath(m_config.sourcesDir);
    QDir().mkpath(m_config.buildDir);
    QDir().mkpath(m_config.outputDir);
    QDir().mkpath(m_config.buildDir + "/logs");
}

QString ImageBuilder::getVersionString() const
{
    switch (m_config.suite) {
        case Jammy: return "22.04";
        case Noble: return "24.04";
        case Oracular: return "24.10";
        case Plucky: return "25.04";
        default: return "24.04";
    }
}

QString ImageBuilder::getImageFileName() const
{
    return QString("ubuntu-%1-preinstalled-%2-arm64-%3.img")
              .arg(getVersionString())
              .arg(flavorToString(m_config.flavor))
              .arg(m_config.board);
}

bool ImageBuilder::hasRequiredTools()
{
    QStringList tools;

    if (m_config.buildMode == KernelOnly) {
        tools << "git" << "make" << "gcc" << "g++" << "aarch64-linux-gnu-gcc";
    } else {
        tools << "git" << "make" << "gcc" << "g++" << "debootstrap" << "parted" << "xz-utils" << "qemu-user-static" << "aarch64-linux-gnu-gcc";
        if (m_config.partitionType == F2FS) {
            tools << "mkfs.f2fs";
        }
    }

    for (const QString &tool : tools) {
        QProcess check;
        check.start("which", QStringList() << tool);
        check.waitForFinished();
        if (check.exitCode() != 0) {
            logError(QString("Missing required tool: %1").arg(tool));
            return false;
        }
    }

    return true;
}

QString ImageBuilder::generateUUID() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString ImageBuilder::getFilesystemTypeString() const
{
    return partitionTypeToString(m_config.partitionType);
}

QString ImageBuilder::getPartitionLabel() const
{
    QString baseLabel = (m_config.flavor == Desktop) ? "desktop-rootfs" : "cloudimg-rootfs";
    if (m_config.partitionType == F2FS) {
        baseLabel += "-f2fs";
    }
    return baseLabel;
}

void ImageBuilder::executeCommand(const QString &command, const QString &workingDir)
{
    if (!m_process) {
        m_process = new QProcess(this);
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        
        connect(m_process, &QProcess::readyRead, this, &ImageBuilder::onProcessOutput);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &ImageBuilder::onProcessFinished);
        connect(m_process, &QProcess::errorOccurred, this, &ImageBuilder::onProcessError);
    }
    
    if (!workingDir.isEmpty()) {
        m_process->setWorkingDirectory(workingDir);
    }
    
    logMessage("Executing: " + command);
    m_process->start("bash", QStringList() << "-c" << command);
}

void ImageBuilder::setCurrentStep(const QString &step, const QString &description)
{
    m_currentStepDescription = description;
    logMessage("=== " + step + ": " + description + " ===");
    emit buildStepChanged(step, description);
}

void ImageBuilder::updateProgress(int percentage)
{
    m_progress = qBound(0, percentage, 100);
    emit buildProgress(m_progress, m_currentStepDescription);
}

void ImageBuilder::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logMsg = QString("[%1] %2").arg(timestamp).arg(message);
    emit buildLogMessage(logMsg);
}

void ImageBuilder::logError(const QString &error)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString errorMsg = QString("[%1] ERROR: %2").arg(timestamp).arg(error);
    emit buildLogMessage(errorMsg);
    emit buildError(error);
}

void ImageBuilder::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        logError(QString("Process failed with exit code: %1").arg(exitCode));
        emit buildCompleted(false, "Build process failed");
        m_isBuilding = false;
        return;
    }
    
    // Move to next build step
    switch (m_currentBuildStep) {
        case StepInitialization:
            updateProgress(20);
            if (m_config.buildMode == KernelOnly) {
                emit buildCompleted(true, "Kernel-only build completed successfully");
                m_isBuilding = false;
            } else {
                buildUBoot();
            }
            break;
        case StepKernelBuild:
            updateProgress(40);
            if (m_config.buildMode == KernelOnly) {
                emit buildCompleted(true, "Kernel build completed successfully");
                m_isBuilding = false;
            } else {
                buildUBoot();
            }
            break;
        case StepUBootBuild:
            updateProgress(55);
            buildRootfs();
            break;
        case StepRootfsBuild:
            updateProgress(75);
            configureImage();
            break;
        case StepImageConfig:
            updateProgress(90);
            createDiskImage();
            break;
        case StepImageCreation:
            updateProgress(100);
            logMessage("Build completed successfully!");
            emit buildCompleted(true, "Image build completed successfully");
            m_isBuilding = false;
            break;
        default:
            break;
    }
}

void ImageBuilder::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Process failed to start";
            break;
        case QProcess::Crashed:
            errorMsg = "Process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Process timed out";
            break;
        default:
            errorMsg = "Unknown process error";
    }
    
    logError(errorMsg);
    emit buildCompleted(false, errorMsg);
    m_isBuilding = false;
}

void ImageBuilder::onProcessOutput()
{
    if (!m_process) return;
    
    QByteArray data = m_process->readAll();
    QString output = QString::fromUtf8(data);
    
    // Split into lines and emit each line
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (!line.trimmed().isEmpty()) {
            emit buildLogMessage(line);
        }
    }
}