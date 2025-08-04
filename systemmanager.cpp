#include "systemmanager.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>
#include <QDateTime>
#include <QRegularExpression>

SystemManager::SystemManager(QObject *parent)
    : QObject(parent)
    , m_currentProcess(nullptr)
    , m_currentOperation("")
    , m_progressTimer(new QTimer(this))
    , m_simulatedProgress(0)
{
    connect(m_progressTimer, &QTimer::timeout, [this]() {
        m_simulatedProgress += 2;
        if (m_simulatedProgress <= 95) {
            emit progressUpdated(m_simulatedProgress);
        }
    });
}

void SystemManager::extractDrivers()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "extract_drivers";
    emit statusUpdated("Starting Orange Pi 5+ driver extraction...");
    
    if (!checkPrerequisites()) {
        emit operationCompleted(false, "Prerequisites check failed");
        return;
    }
    
    // Detect GPU drivers in /gpu directory
    QString gpuPath = detectGpuDrivers();
    
    // Check for upgrade files in local directories
    QString upgradeBase = "/home/snake/Arm-Pi-Tweaker/upgrade";
    emit statusUpdated("Scanning upgrade directories for kernel files...");
    
    // Search for kernel files in upgrade subdirectories
    QStringList kernelPatterns = {"vmlinuz*", "initrd*", "config-*", "System.map-*"};
    QStringList kernelFiles = findFilesInDirectory(upgradeBase, kernelPatterns);
    
    // Search for device tree files
    QStringList dtPatterns = {"*.dtb", "*.dts"};
    QStringList dtFiles = findFilesInDirectory(upgradeBase, dtPatterns);
    
    // Search for module files
    QStringList modulePatterns = {"*.ko", "modules.*"};
    QStringList moduleFiles = findFilesInDirectory(upgradeBase, modulePatterns);
    
    emit statusUpdated(QString("Found %1 kernel files, %2 device tree files, %3 module files")
                      .arg(kernelFiles.size()).arg(dtFiles.size()).arg(moduleFiles.size()));
    
    if (kernelFiles.isEmpty() && dtFiles.isEmpty() && moduleFiles.isEmpty() && gpuPath.isEmpty()) {
        emit operationCompleted(false, 
            "No extractable files found in /gpu or /upgrade directories. "
            "Please ensure upgrade.img is extracted or kernel files are present.");
        return;
    }
    
    // Create destination directory structure
    QString destPath = "/home/snake/Arm-Pi-Tweaker/extracted_drivers";
    QDir().mkpath(destPath + "/boot");
    QDir().mkpath(destPath + "/lib/modules");
    QDir().mkpath(destPath + "/lib/firmware");
    QDir().mkpath(destPath + "/usr/lib/aarch64-linux-gnu");
    QDir().mkpath(destPath + "/etc/X11");
    QDir().mkpath(destPath + "/gpu");
    
    // Start extraction process
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    connect(m_currentProcess, &QProcess::readyReadStandardError, this, &SystemManager::onProcessOutput);
    
    // Create comprehensive extraction script
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "UPGRADE_BASE='%1'\n"
        "GPU_PATH='%2'\n"
        "DEST='%3'\n"
        "COPIED_COUNT=0\n"
        "\n"
        "log_copy() {\n"
        "    echo \"[$(date '+%%H:%%M:%%S')] $1\"\n"
        "}\n"
        "\n"
        "safe_copy_file() {\n"
        "    local src=\"$1\"\n"
        "    local dst_dir=\"$2\"\n"
        "    local desc=\"$3\"\n"
        "    \n"
        "    if [ -f \"$src\" ]; then\n"
        "        mkdir -p \"$dst_dir\"\n"
        "        cp -v \"$src\" \"$dst_dir/\" && COPIED_COUNT=$((COPIED_COUNT + 1))\n"
        "        log_copy \"Copied $desc: $src -> $dst_dir/\"\n"
        "        return 0\n"
        "    fi\n"
        "    return 1\n"
        "}\n"
        "\n"
        "safe_copy_dir() {\n"
        "    local src=\"$1\"\n"
        "    local dst=\"$2\"\n"
        "    local desc=\"$3\"\n"
        "    \n"
        "    if [ -d \"$src\" ]; then\n"
        "        mkdir -p \"$dst\"\n"
        "        cp -rv \"$src\"/* \"$dst/\" && COPIED_COUNT=$((COPIED_COUNT + 1))\n"
        "        log_copy \"Copied $desc: $src -> $dst\"\n"
        "        return 0\n"
        "    fi\n"
        "    return 1\n"
        "}\n"
        "\n"
        "log_copy \"🔍 Starting comprehensive Orange Pi 5+ extraction...\"\n"
        "\n"
        "# Extract GPU drivers from /gpu directory\n"
        "if [ -n \"$GPU_PATH\" ] && [ -d \"$GPU_PATH\" ]; then\n"
        "    log_copy \"📱 Extracting GPU drivers from $GPU_PATH...\"\n"
        "    \n"
        "    # Copy all GPU subdirectories\n"
        "    find \"$GPU_PATH\" -mindepth 1 -maxdepth 1 -type d | while read gpu_subdir; do\n"
        "        subdir_name=$(basename \"$gpu_subdir\")\n"
        "        log_copy \"Processing GPU driver: $subdir_name\"\n"
        "        safe_copy_dir \"$gpu_subdir\" \"$DEST/gpu/$subdir_name\" \"$subdir_name GPU drivers\"\n"
        "    done\n"
        "    \n"
        "    # Copy individual GPU files\n"
        "    find \"$GPU_PATH\" -maxdepth 1 -name '*.deb' -o -name 'libmali*' -o -name '*.so*' | while read gpu_file; do\n"
        "        safe_copy_file \"$gpu_file\" \"$DEST/gpu\" \"GPU driver file\"\n"
        "    done\n"
        "fi\n"
        "\n"
        "# Extract kernel and system files from upgrade directory\n"
        "if [ -d \"$UPGRADE_BASE\" ]; then\n"
        "    log_copy \"🐧 Extracting kernel files from $UPGRADE_BASE...\"\n"
        "    \n"
        "    # Find and copy kernel files\n"
        "    find \"$UPGRADE_BASE\" -name 'vmlinuz*' -type f | while read kernel; do\n"
        "        safe_copy_file \"$kernel\" \"$DEST/boot\" \"kernel image\"\n"
        "    done\n"
        "    \n"
        "    find \"$UPGRADE_BASE\" -name 'initrd*' -type f | while read initrd; do\n"
        "        safe_copy_file \"$initrd\" \"$DEST/boot\" \"initrd image\"\n"
        "    done\n"
        "    \n"
        "    find \"$UPGRADE_BASE\" -name 'config-*' -type f | while read config; do\n"
        "        safe_copy_file \"$config\" \"$DEST/boot\" \"kernel config\"\n"
        "    done\n"
        "    \n"
        "    find \"$UPGRADE_BASE\" -name 'System.map-*' -type f | while read sysmap; do\n"
        "        safe_copy_file \"$sysmap\" \"$DEST/boot\" \"kernel symbols\"\n"
        "    done\n"
        "    \n"
        "    # Find and copy device tree files\n"
        "    log_copy \"🌳 Extracting device tree files...\"\n"
        "    find \"$UPGRADE_BASE\" -name '*.dtb' -o -name '*.dts' | while read dt_file; do\n"
        "        safe_copy_file \"$dt_file\" \"$DEST/boot/dtb\" \"device tree file\"\n"
        "    done\n"
        "    \n"
        "    # Find and copy module directories\n"
        "    log_copy \"🔧 Extracting kernel modules...\"\n"
        "    find \"$UPGRADE_BASE\" -path '*/lib/modules/*' -type d -name '[0-9]*' | while read module_dir; do\n"
        "        module_version=$(basename \"$module_dir\")\n"
        "        safe_copy_dir \"$module_dir\" \"$DEST/lib/modules/$module_version\" \"kernel modules $module_version\"\n"
        "    done\n"
        "    \n"
        "    # Find and copy firmware\n"
        "    log_copy \"💾 Extracting firmware...\"\n"
        "    find \"$UPGRADE_BASE\" -path '*/lib/firmware' -type d | while read fw_dir; do\n"
        "        safe_copy_dir \"$fw_dir\" \"$DEST/lib/firmware\" \"firmware files\"\n"
        "    done\n"
        "fi\n"
        "\n"
        "# Create extraction manifest\n"
        "MANIFEST=\"$DEST/extraction_manifest.txt\"\n"
        "echo \"# Arm-Pi Tweaker Extraction Manifest\" > \"$MANIFEST\"\n"
        "echo \"Extraction Date: $(date)\" >> \"$MANIFEST\"\n"
        "echo \"GPU Path: $GPU_PATH\" >> \"$MANIFEST\"\n"
        "echo \"Upgrade Base: $UPGRADE_BASE\" >> \"$MANIFEST\"\n"
        "echo \"Items Copied: $COPIED_COUNT\" >> \"$MANIFEST\"\n"
        "echo \"\" >> \"$MANIFEST\"\n"
        "echo \"Extracted Files:\" >> \"$MANIFEST\"\n"
        "find \"$DEST\" -type f | sort >> \"$MANIFEST\"\n"
        "\n"
        "log_copy \"✅ Extraction completed - $COPIED_COUNT items copied\"\n"
        "log_copy \"📄 Manifest: $MANIFEST\"\n"
        "log_copy \"📁 Files extracted to: $DEST\"\n"
    ).arg(upgradeBase, gpuPath, destPath);
    
    // Write extraction script
    QString scriptPath = "/tmp/extract_armpi_drivers.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        // Make script executable
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        // Start extraction with progress tracking
        m_simulatedProgress = 0;
        m_progressTimer->start(1000);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create extraction script");
    }
}

void SystemManager::runUbuntuUpgrade()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "ubuntu_upgrade";
    emit statusUpdated("Preparing Ubuntu upgrade to 24.10...");
    
    // Check prerequisites first
    if (!checkUpgradePrerequisites()) {
        emit operationCompleted(false, "Prerequisites check failed for Ubuntu upgrade");
        return;
    }
    
    // Prepare system for upgrade
    if (!prepareSystemForUpgrade()) {
        emit operationCompleted(false, "Failed to prepare system for upgrade");
        return;
    }
    
    emit statusUpdated("Starting Ubuntu upgrade to 24.10...");
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    connect(m_currentProcess, &QProcess::readyReadStandardError, this, &SystemManager::onProcessOutput);
    
    m_simulatedProgress = 0;
    m_progressTimer->start(2000); // Slower progress for long upgrade
    emit progressUpdated(0);
    
    // Set environment for non-interactive upgrade
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DEBIAN_FRONTEND", "noninteractive");
    env.insert("DEBIAN_PRIORITY", "critical");
    m_currentProcess->setProcessEnvironment(env);
    
    // Run the actual Ubuntu upgrade
    m_currentProcess->start("bash", QStringList() << "-c" << 
        "sudo DEBIAN_FRONTEND=noninteractive do-release-upgrade -f DistUpgradeViewNonInteractive -d");
}

void SystemManager::patchSystem()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "patch_system";
    emit statusUpdated("Preparing to patch system with Orange Pi 5+ support...");
    
    // Verify upgrade files exist
    QString upgradeDir = "/home/snake/Arm-Pi-Tweaker/upgrade";
    if (!QDir(upgradeDir).exists()) {
        emit operationCompleted(false, "Upgrade directory not found - run driver extraction first");
        return;
    }
    
    // Create backup before patching
    createBackup();
    
    emit statusUpdated("Patching system with Orange Pi 5+ support...");
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    connect(m_currentProcess, &QProcess::readyReadStandardError, this, &SystemManager::onProcessOutput);
    
    // Create comprehensive patching script
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "UPGRADE_DIR='%1'\n"
        "PATCHED_COUNT=0\n"
        "\n"
        "log_patch() {\n"
        "    echo \"[$(date '+%%H:%%M:%%S')] $1\"\n"
        "}\n"
        "\n"
        "safe_patch() {\n"
        "    local src=\"$1\"\n"
        "    local dst=\"$2\"\n"
        "    local desc=\"$3\"\n"
        "    \n"
        "    if [ -e \"$src\" ]; then\n"
        "        log_patch \"Installing $desc: $src -> $dst\"\n"
        "        sudo cp -rv \"$src\" \"$dst\" && PATCHED_COUNT=$((PATCHED_COUNT + 1))\n"
        "        return 0\n"
        "    else\n"
        "        log_patch \"Not found: $src (skipping $desc)\"\n"
        "        return 1\n"
        "    fi\n"
        "}\n"
        "\n"
        "safe_patch_glob() {\n"
        "    local pattern=\"$1\"\n"
        "    local dst=\"$2\"\n"
        "    local desc=\"$3\"\n"
        "    local found=false\n"
        "    \n"
        "    for file in $pattern; do\n"
        "        if [ -e \"$file\" ]; then\n"
        "            safe_patch \"$file\" \"$dst\" \"$desc\"\n"
        "            found=true\n"
        "        fi\n"
        "    done\n"
        "    \n"
        "    if [ \"$found\" = false ]; then\n"
        "        log_patch \"No files found matching: $pattern\"\n"
        "    fi\n"
        "}\n"
        "\n"
        "log_patch \"🚀 Starting Orange Pi 5+ system patching...\"\n"
        "\n"
        "# Install kernel files\n"
        "log_patch \"📦 Installing kernel files...\"\n"
        "safe_patch_glob \"$UPGRADE_DIR/boot/vmlinuz*\" \"/boot/\" \"kernel image\"\n"
        "safe_patch_glob \"$UPGRADE_DIR/boot/initrd*\" \"/boot/\" \"initrd image\"\n"
        "safe_patch_glob \"$UPGRADE_DIR/boot/config-*\" \"/boot/\" \"kernel config\"\n"
        "safe_patch_glob \"$UPGRADE_DIR/boot/System.map-*\" \"/boot/\" \"kernel symbols\"\n"
        "\n"
        "# Install device tree files\n"
        "log_patch \"🌳 Installing device tree files...\"\n"
        "if [ -d \"$UPGRADE_DIR/boot/dtbs\" ]; then\n"
        "    safe_patch \"$UPGRADE_DIR/boot/dtbs\" \"/boot/\" \"device tree files\"\n"
        "fi\n"
        "if [ -d \"$UPGRADE_DIR/boot/dtb\" ]; then\n"
        "    safe_patch \"$UPGRADE_DIR/boot/dtb\" \"/boot/\" \"device tree files\"\n"
        "fi\n"
        "\n"
        "# Install kernel modules\n"
        "log_patch \"🔧 Installing kernel modules...\"\n"
        "if [ -d \"$UPGRADE_DIR/lib/modules\" ]; then\n"
        "    for module_dir in \"$UPGRADE_DIR\"/lib/modules/*; do\n"
        "        if [ -d \"$module_dir\" ]; then\n"
        "            module_name=$(basename \"$module_dir\")\n"
        "            safe_patch \"$module_dir\" \"/lib/modules/\" \"kernel modules for $module_name\"\n"
        "        fi\n"
        "    done\n"
        "fi\n"
        "\n"
        "# Install firmware\n"
        "log_patch \"💾 Installing firmware...\"\n"
        "if [ -d \"$UPGRADE_DIR/lib/firmware\" ]; then\n"
        "    # Create firmware directory if it doesn't exist\n"
        "    sudo mkdir -p /lib/firmware\n"
        "    safe_patch \"$UPGRADE_DIR/lib/firmware/.\" \"/lib/firmware/\" \"firmware files\"\n"
        "fi\n"
        "\n"
        "# Install GPU drivers\n"
        "log_patch \"🎮 Installing GPU drivers...\"\n"
        "if [ -d \"$UPGRADE_DIR/usr/lib/aarch64-linux-gnu\" ]; then\n"
        "    sudo mkdir -p /usr/lib/aarch64-linux-gnu\n"
        "    safe_patch_glob \"$UPGRADE_DIR/usr/lib/aarch64-linux-gnu/libmali*\" \"/usr/lib/aarch64-linux-gnu/\" \"Mali GPU drivers\"\n"
        "    safe_patch_glob \"$UPGRADE_DIR/usr/lib/aarch64-linux-gnu/libEGL*\" \"/usr/lib/aarch64-linux-gnu/\" \"EGL libraries\"\n"
        "    safe_patch_glob \"$UPGRADE_DIR/usr/lib/aarch64-linux-gnu/libGLES*\" \"/usr/lib/aarch64-linux-gnu/\" \"GLES libraries\"\n"
        "    \n"
        "    if [ -d \"$UPGRADE_DIR/usr/lib/aarch64-linux-gnu/dri\" ]; then\n"
        "        safe_patch \"$UPGRADE_DIR/usr/lib/aarch64-linux-gnu/dri\" \"/usr/lib/aarch64-linux-gnu/\" \"DRI drivers\"\n"
        "    fi\n"
        "fi\n"
        "\n"
        "# Install X11 configuration\n"
        "log_patch \"🖥️ Installing X11 configuration...\"\n"
        "if [ -d \"$UPGRADE_DIR/etc/X11\" ]; then\n"
        "    sudo mkdir -p /etc/X11\n"
        "    if [ -d \"$UPGRADE_DIR/etc/X11/xorg.conf.d\" ]; then\n"
        "        safe_patch \"$UPGRADE_DIR/etc/X11/xorg.conf.d\" \"/etc/X11/\" \"X11 configuration directory\"\n"
        "    fi\n"
        "    if [ -f \"$UPGRADE_DIR/etc/X11/xorg.conf\" ]; then\n"
        "        safe_patch \"$UPGRADE_DIR/etc/X11/xorg.conf\" \"/etc/X11/\" \"X11 configuration file\"\n"
        "    fi\n"
        "fi\n"
        "\n"
        "# Update system configuration\n"
        "log_patch \"⚙️ Updating system configuration...\"\n"
        "\n"
        "# Update initramfs for all installed kernels\n"
        "log_patch \"🔄 Updating initramfs...\"\n"
        "if sudo update-initramfs -u -k all; then\n"
        "    log_patch \"✅ Initramfs updated successfully\"\n"
        "else\n"
        "    log_patch \"⚠️ Initramfs update failed, trying specific kernel...\"\n"
        "    # Try updating for current kernel\n"
        "    CURRENT_KERNEL=$(uname -r)\n"
        "    sudo update-initramfs -u -k \"$CURRENT_KERNEL\" || log_patch \"❌ Failed to update initramfs\"\n"
        "fi\n"
        "\n"
        "# Update GRUB bootloader\n"
        "log_patch \"🥾 Updating GRUB bootloader...\"\n"
        "if sudo update-grub; then\n"
        "    log_patch \"✅ GRUB updated successfully\"\n"
        "else\n"
        "    log_patch \"❌ GRUB update failed\"\n"
        "fi\n"
        "\n"
        "# Update library cache\n"
        "log_patch \"📚 Updating library cache...\"\n"
        "sudo ldconfig\n"
        "\n"
        "# Create patch manifest\n"
        "MANIFEST_FILE=\"/home/snake/Arm-Pi-Tweaker/patch_manifest_$(date +%%Y%%m%%d_%%H%%M%%S).txt\"\n"
        "echo \"# Orange Pi 5+ System Patch Manifest\" > \"$MANIFEST_FILE\"\n"
        "echo \"Patch Date: $(date)\" >> \"$MANIFEST_FILE\"\n"
        "echo \"Files Patched: $PATCHED_COUNT\" >> \"$MANIFEST_FILE\"\n"
        "echo \"Kernel Version: $(uname -r)\" >> \"$MANIFEST_FILE\"\n"
        "echo \"Ubuntu Version: $(lsb_release -d | cut -f2)\" >> \"$MANIFEST_FILE\"\n"
        "echo \"\" >> \"$MANIFEST_FILE\"\n"
        "echo \"Installed Files:\" >> \"$MANIFEST_FILE\"\n"
        "find /boot -name '*orange*' -o -name '*rk3588*' -o -name '*mali*' 2>/dev/null | sort >> \"$MANIFEST_FILE\" || true\n"
        "find /lib/modules -name '*rk3588*' -o -name '*mali*' 2>/dev/null | head -20 >> \"$MANIFEST_FILE\" || true\n"
        "find /lib/firmware -name '*rk3588*' -o -name '*mali*' 2>/dev/null | head -20 >> \"$MANIFEST_FILE\" || true\n"
        "\n"
        "log_patch \"✅ Orange Pi 5+ system patching completed!\"\n"
        "log_patch \"📊 Total files patched: $PATCHED_COUNT\"\n"
        "log_patch \"📄 Patch manifest: $MANIFEST_FILE\"\n"
        "log_patch \"🔄 Please reboot to complete the installation\"\n"
    ).arg(upgradeDir);
    
    QString scriptPath = "/tmp/patch_opi5_system.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        m_simulatedProgress = 0;
        m_progressTimer->start(1000);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create patching script");
    }
}

void SystemManager::rollbackUpgrade()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "rollback";
    emit statusUpdated("Rolling back upgrade...");
    
    QString backupDir = "/home/snake/Arm-Pi-Tweaker/backup";
    if (!QDir(backupDir).exists()) {
        emit operationCompleted(false, "No backup found to rollback to");
        return;
    }
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    // Create rollback script
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "BACKUP_DIR='%1'\n"
        "echo 'Restoring from backup...'\n"
        "sudo cp -rv \"$BACKUP_DIR\"/boot/* /boot/\n"
        "sudo cp -rv \"$BACKUP_DIR\"/lib/* /lib/\n"
        "echo 'Updating initramfs...'\n"
        "sudo update-initramfs -u\n"
        "echo 'Updating GRUB...'\n"
        "sudo update-grub\n"
        "echo 'Rollback completed successfully'\n"
    ).arg(backupDir);
    
    QString scriptPath = "/tmp/rollback.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        m_simulatedProgress = 0;
        m_progressTimer->start(500);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create rollback script");
    }
}

void SystemManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_progressTimer->stop();
    emit progressUpdated(100);
    
    QString operation = m_currentOperation;
    m_currentOperation.clear();
    
    if (m_currentProcess) {
        m_currentProcess->deleteLater();
        m_currentProcess = nullptr;
    }
    
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        QString message;
        if (operation == "extract_drivers") {
            message = "✅ Orange Pi 5+ drivers extracted successfully";
        } else if (operation == "ubuntu_upgrade") {
            message = "✅ Ubuntu upgrade to 24.10 completed successfully";
        } else if (operation == "patch_system") {
            message = "✅ Orange Pi 5+ support patched successfully";
        } else if (operation == "rollback") {
            message = "✅ Rollback completed successfully";
        } else {
            message = "✅ Operation completed successfully";
        }
        
        emit statusUpdated(message);
        emit operationCompleted(true, message);
    } else {
        QString message = QString("❌ Operation failed with exit code %1").arg(exitCode);
        emit statusUpdated(message);
        emit operationCompleted(false, message);
    }
}

void SystemManager::onProcessError(QProcess::ProcessError error)
{
    m_progressTimer->stop();
    
    QString errorMessage;
    switch (error) {
        case QProcess::FailedToStart:
            errorMessage = "Process failed to start";
            break;
        case QProcess::Crashed:
            errorMessage = "Process crashed";
            break;
        case QProcess::Timedout:
            errorMessage = "Process timed out";
            break;
        default:
            errorMessage = "Unknown process error";
            break;
    }
    
    emit statusUpdated(QString("❌ %1").arg(errorMessage));
    emit operationCompleted(false, errorMessage);
    
    if (m_currentProcess) {
        m_currentProcess->deleteLater();
        m_currentProcess = nullptr;
    }
}

void SystemManager::onProcessOutput()
{
    if (m_currentProcess) {
        QByteArray data = m_currentProcess->readAllStandardOutput();
        QString output = QString::fromUtf8(data).trimmed();
        if (!output.isEmpty()) {
            emit statusUpdated(output);
        }
    }
}

bool SystemManager::checkPrerequisites()
{
    // Check if running as root or can sudo
    QProcess process;
    process.start("id", QStringList() << "-u");
    process.waitForFinished();
    
    if (process.readAllStandardOutput().trimmed() != "0") {
        // Not root, check if sudo is available
        QProcess sudoCheck;
        sudoCheck.start("sudo", QStringList() << "-n" << "true");
        sudoCheck.waitForFinished();
        
        if (sudoCheck.exitCode() != 0) {
            emit statusUpdated("⚠️ Root privileges required. Please run with sudo or configure passwordless sudo.");
            return false;
        }
    }
    
    return true;
}

QString SystemManager::getUpgradeSourcePath()
{
    // Check for mounted upgrade image first
    if (QDir("/mnt/upgrade").exists()) {
        return "/mnt/upgrade";
    }
    
    // Check for local upgrade directory
    QString localUpgrade = "/home/snake/Arm-Pi-Tweaker/upgrade";
    if (QDir(localUpgrade).exists()) {
        return localUpgrade;
    }
    
    return QString();
}

QString SystemManager::detectGpuDrivers()
{
    QString gpuDir = "/home/snake/Arm-Pi-Tweaker/gpu";
    
    emit statusUpdated("Scanning GPU driver directory...");
    
    if (!QDir(gpuDir).exists()) {
        emit statusUpdated("⚠️ GPU directory not found: " + gpuDir);
        return QString();
    }
    
    // Look for common GPU driver types
    QStringList driverTypes;
    QDir gpu(gpuDir);
    
    QStringList subdirs = gpu.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subdir : subdirs) {
        QString fullPath = gpuDir + "/" + subdir;
        QDir driverDir(fullPath);
        
        // Check for common driver files
        QStringList driverFiles = driverDir.entryList(QStringList() << "*.deb" << "libmali*" << "*.so*", QDir::Files);
        if (!driverFiles.isEmpty()) {
            driverTypes.append(subdir + " (" + QString::number(driverFiles.size()) + " files)");
            emit statusUpdated(QString("Found GPU drivers in: %1 - %2 files").arg(fullPath).arg(driverFiles.size()));
        }
    }
    
    if (driverTypes.isEmpty()) {
        emit statusUpdated("⚠️ No GPU drivers found in " + gpuDir);
        return QString();
    }
    
    emit statusUpdated(QString("✅ Detected GPU driver types: %1").arg(driverTypes.join(", ")));
    return gpuDir;
}

QStringList SystemManager::findFilesInDirectory(const QString &directory, const QStringList &patterns)
{
    QStringList foundFiles;
    QDir dir(directory);
    
    if (!dir.exists()) {
        return foundFiles;
    }
    
    // Search recursively through subdirectories
    QDirIterator it(directory, patterns, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        foundFiles.append(it.next());
    }
    
    return foundFiles;
}

bool SystemManager::checkUpgradePrerequisites()
{
    emit statusUpdated("Checking upgrade prerequisites...");
    
    // Check disk space (need at least 10GB free)
    if (!checkDiskSpace()) {
        emit statusUpdated("❌ Insufficient disk space for upgrade");
        return false;
    }
    
    // Check internet connectivity
    QProcess networkCheck;
    networkCheck.start("ping", QStringList() << "-c" << "1" << "archive.ubuntu.com");
    networkCheck.waitForFinished(5000);
    
    if (networkCheck.exitCode() != 0) {
        emit statusUpdated("❌ No internet connection to Ubuntu repositories");
        return false;
    }
    
    // Check current Ubuntu version
    QProcess versionCheck;
    versionCheck.start("lsb_release", QStringList() << "-r" << "-s");
    versionCheck.waitForFinished(2000);
    
    QString currentVersion = versionCheck.readAllStandardOutput().trimmed();
    if (!currentVersion.startsWith("22.04")) {
        emit statusUpdated(QString("❌ Current version %1 is not supported for upgrade").arg(currentVersion));
        return false;
    }
    
    emit statusUpdated("✅ Prerequisites check passed");
    return true;
}

bool SystemManager::prepareSystemForUpgrade()
{
    emit statusUpdated("Preparing system for upgrade...");
    
    // Update package lists
    if (!updatePackageLists()) {
        return false;
    }
    
    // Fix any broken packages
    if (!fixBrokenPackages()) {
        return false;
    }
    
    // Install update-manager-core if not present
    QProcess checkManager;
    checkManager.start("dpkg", QStringList() << "-l" << "update-manager-core");
    checkManager.waitForFinished(3000);
    
    if (checkManager.exitCode() != 0) {
        emit statusUpdated("Installing update-manager-core...");
        QProcess installManager;
        installManager.start("sudo", QStringList() << "apt" << "install" << "-y" << "update-manager-core");
        installManager.waitForFinished(60000);
        
        if (installManager.exitCode() != 0) {
            emit statusUpdated("❌ Failed to install update-manager-core");
            return false;
        }
    }
    
    // Enable development release upgrades
    QProcess enableDevel;
    enableDevel.start("sudo", QStringList() << "sed" << "-i" << "s/Prompt=lts/Prompt=normal/" << "/etc/update-manager/release-upgrades");
    enableDevel.waitForFinished(3000);
    
    emit statusUpdated("✅ System prepared for upgrade");
    return true;
}

bool SystemManager::checkDiskSpace()
{
    QProcess dfCheck;
    dfCheck.start("df", QStringList() << "/" << "--output=avail" << "-B1G");
    dfCheck.waitForFinished(3000);
    
    QString output = dfCheck.readAllStandardOutput();
    QStringList lines = output.split('\n');
    if (lines.size() >= 2) {
        bool ok;
        int availableGB = lines[1].trimmed().toInt(&ok);
        if (ok && availableGB >= 10) {
            emit statusUpdated(QString("✅ Sufficient disk space: %1GB available").arg(availableGB));
            return true;
        } else {
            emit statusUpdated(QString("❌ Insufficient disk space: %1GB available, need 10GB").arg(availableGB));
            return false;
        }
    }
    
    emit statusUpdated("⚠️ Could not determine disk space, proceeding anyway");
    return true;
}

bool SystemManager::updatePackageLists()
{
    emit statusUpdated("Updating package lists...");
    
    QProcess aptUpdate;
    aptUpdate.start("sudo", QStringList() << "apt" << "update");
    aptUpdate.waitForFinished(120000); // 2 minutes timeout
    
    if (aptUpdate.exitCode() != 0) {
        QString error = aptUpdate.readAllStandardError();
        emit statusUpdated(QString("❌ Failed to update package lists: %1").arg(error));
        return false;
    }
    
    emit statusUpdated("✅ Package lists updated");
    return true;
}

bool SystemManager::fixBrokenPackages()
{
    emit statusUpdated("Checking and fixing broken packages...");
    
    // First check if there are broken packages
    QProcess checkBroken;
    checkBroken.start("apt", QStringList() << "list" << "--broken");
    checkBroken.waitForFinished(10000);
    
    QString brokenOutput = checkBroken.readAllStandardOutput();
    if (brokenOutput.contains("WARNING: apt does not have a stable CLI interface")) {
        // No broken packages found (just the warning)
        emit statusUpdated("✅ No broken packages found");
        return true;
    }
    
    // Fix broken packages
    QProcess fixBroken;
    fixBroken.start("sudo", QStringList() << "apt" << "--fix-broken" << "install" << "-y");
    fixBroken.waitForFinished(300000); // 5 minutes timeout
    
    if (fixBroken.exitCode() != 0) {
        QString error = fixBroken.readAllStandardError();
        emit statusUpdated(QString("❌ Failed to fix broken packages: %1").arg(error));
        return false;
    }
    
    emit statusUpdated("✅ Broken packages fixed");
    return true;
}

void SystemManager::createBackup()
{
    QString backupDir = QString("/home/snake/Arm-Pi-Tweaker/backup_%1")
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    
    emit statusUpdated(QString("Creating backup to %1...").arg(backupDir));
    
    QDir().mkpath(backupDir);
    QDir().mkpath(backupDir + "/boot");
    QDir().mkpath(backupDir + "/lib");
    
    // Create backup of important system files
    QProcess bootBackup;
    bootBackup.start("sudo", QStringList() << "cp" << "-r" << "/boot/." << backupDir + "/boot/");
    bootBackup.waitForFinished(60000);
    
    QProcess modulesBackup;
    modulesBackup.start("sudo", QStringList() << "cp" << "-r" << "/lib/modules" << backupDir + "/lib/");
    modulesBackup.waitForFinished(60000);
    
    QProcess firmwareBackup;
    firmwareBackup.start("sudo", QStringList() << "cp" << "-r" << "/lib/firmware" << backupDir + "/lib/");
    firmwareBackup.waitForFinished(60000);
    
    // Backup sources.list
    QProcess sourcesBackup;
    sourcesBackup.start("sudo", QStringList() << "cp" << "/etc/apt/sources.list" << backupDir + "/sources.list");
    sourcesBackup.waitForFinished(5000);
    
    emit statusUpdated(QString("💾 Backup created: %1").arg(backupDir));
}

// GPU Management Implementation
void SystemManager::installGpuDriver(const QString &driverPath)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "install_gpu_driver";
    emit statusUpdated(QString("Installing GPU driver: %1").arg(QFileInfo(driverPath).fileName()));
    
    if (!QFile::exists(driverPath)) {
        emit operationCompleted(false, "Driver file not found");
        return;
    }
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    // Create GPU driver installation script
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "DRIVER_PATH='%1'\n"
        "DRIVER_NAME=$(basename \"$DRIVER_PATH\")\n"
        "\n"
        "log_gpu() {\n"
        "    echo \"[$(date '+%%H:%%M:%%S')] $1\"\n"
        "}\n"
        "\n"
        "log_gpu \"🎮 Installing GPU driver: $DRIVER_NAME\"\n"
        "\n"
        "# Stop display manager if running\n"
        "if systemctl is-active --quiet display-manager; then\n"
        "    log_gpu \"Stopping display manager...\"\n"
        "    sudo systemctl stop display-manager\n"
        "fi\n"
        "\n"
        "# Install .deb package\n"
        "if [[ \"$DRIVER_PATH\" == *.deb ]]; then\n"
        "    log_gpu \"Installing .deb package...\"\n"
        "    sudo dpkg -i \"$DRIVER_PATH\" || sudo apt-get install -f -y\n"
        "elif [[ \"$DRIVER_PATH\" == *.tar.* ]]; then\n"
        "    log_gpu \"Extracting and installing from archive...\"\n"
        "    TEMP_DIR=$(mktemp -d)\n"
        "    tar -xf \"$DRIVER_PATH\" -C \"$TEMP_DIR\"\n"
        "    \n"
        "    # Look for install script\n"
        "    if [ -f \"$TEMP_DIR/install.sh\" ]; then\n"
        "        cd \"$TEMP_DIR\" && sudo bash install.sh\n"
        "    else\n"
        "        # Manual installation\n"
        "        find \"$TEMP_DIR\" -name '*.so*' | while read lib; do\n"
        "            sudo cp \"$lib\" /usr/lib/aarch64-linux-gnu/\n"
        "        done\n"
        "    fi\n"
        "    \n"
        "    rm -rf \"$TEMP_DIR\"\n"
        "else\n"
        "    log_gpu \"❌ Unsupported driver format\"\n"
        "    exit 1\n"
        "fi\n"
        "\n"
        "# Update library cache\n"
        "log_gpu \"Updating library cache...\"\n"
        "sudo ldconfig\n"
        "\n"
        "# Create/update GPU configuration\n"
        "log_gpu \"Configuring GPU...\"\n"
        "sudo mkdir -p /etc/X11/xorg.conf.d\n"
        "\n"
        "# Restart display manager\n"
        "if systemctl list-unit-files | grep -q display-manager; then\n"
        "    log_gpu \"Restarting display manager...\"\n"
        "    sudo systemctl start display-manager\n"
        "fi\n"
        "\n"
        "log_gpu \"✅ GPU driver installation completed\"\n"
        "log_gpu \"Please reboot to ensure all changes take effect\"\n"
    ).arg(driverPath);
    
    QString scriptPath = "/tmp/install_gpu_driver.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        m_simulatedProgress = 0;
        m_progressTimer->start(1000);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create installation script");
    }
}

void SystemManager::removeGpuDriver(const QString &driverName)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "remove_gpu_driver";
    emit statusUpdated(QString("Removing GPU driver: %1").arg(driverName));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    // Create GPU driver removal script
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "DRIVER_NAME='%1'\n"
        "\n"
        "log_gpu() {\n"
        "    echo \"[$(date '+%%H:%%M:%%S')] $1\"\n"
        "}\n"
        "\n"
        "log_gpu \"🗑️ Removing GPU driver: $DRIVER_NAME\"\n"
        "\n"
        "# Stop display manager\n"
        "if systemctl is-active --quiet display-manager; then\n"
        "    log_gpu \"Stopping display manager...\"\n"
        "    sudo systemctl stop display-manager\n"
        "fi\n"
        "\n"
        "# Remove packages\n"
        "if dpkg -l | grep -q \"$DRIVER_NAME\"; then\n"
        "    log_gpu \"Removing package: $DRIVER_NAME\"\n"
        "    sudo apt-get remove --purge -y \"$DRIVER_NAME\"\n"
        "    sudo apt-get autoremove -y\n"
        "fi\n"
        "\n"
        "# Remove Mali-specific packages\n"
        "for pkg in libmali mali-driver; do\n"
        "    if dpkg -l | grep -q \"$pkg\"; then\n"
        "        log_gpu \"Removing $pkg packages...\"\n"
        "        sudo apt-get remove --purge -y \"$pkg\"*\n"
        "    fi\n"
        "done\n"
        "\n"
        "# Clean up library files\n"
        "log_gpu \"Cleaning up driver files...\"\n"
        "sudo rm -f /usr/lib/aarch64-linux-gnu/libmali*\n"
        "sudo rm -f /usr/lib/aarch64-linux-gnu/libEGL*mali*\n"
        "sudo rm -f /usr/lib/aarch64-linux-gnu/libGLES*mali*\n"
        "\n"
        "# Remove X11 configuration\n"
        "sudo rm -f /etc/X11/xorg.conf.d/*mali*\n"
        "sudo rm -f /etc/X11/xorg.conf.d/*gpu*\n"
        "\n"
        "# Update library cache\n"
        "log_gpu \"Updating library cache...\"\n"
        "sudo ldconfig\n"
        "\n"
        "# Restart display manager\n"
        "if systemctl list-unit-files | grep -q display-manager; then\n"
        "    log_gpu \"Restarting display manager...\"\n"
        "    sudo systemctl start display-manager\n"
        "fi\n"
        "\n"
        "log_gpu \"✅ GPU driver removal completed\"\n"
    ).arg(driverName);
    
    QString scriptPath = "/tmp/remove_gpu_driver.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        m_simulatedProgress = 0;
        m_progressTimer->start(1000);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create removal script");
    }
}

void SystemManager::switchGpuDriver(const QString &driverType)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "switch_gpu_driver";
    emit statusUpdated(QString("Switching to GPU driver: %1").arg(driverType));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "DRIVER_TYPE='%1'\n"
        "\n"
        "log_gpu() {\n"
        "    echo \"[$(date '+%%H:%%M:%%S')] $1\"\n"
        "}\n"
        "\n"
        "log_gpu \"🔄 Switching to GPU driver: $DRIVER_TYPE\"\n"
        "\n"
        "# Stop display manager\n"
        "if systemctl is-active --quiet display-manager; then\n"
        "    sudo systemctl stop display-manager\n"
        "fi\n"
        "\n"
        "case \"$DRIVER_TYPE\" in\n"
        "    *Mali*Proprietary*)\n"
        "        log_gpu \"Installing Mali proprietary driver...\"\n"
        "        # Install Mali proprietary packages\n"
        "        if [ -f \"/home/snake/Arm-Pi-Tweaker/gpu/proprietary/libmali-valhall-g610-g13p0-wayland-gbm_1.9-1_arm64.deb\" ]; then\n"
        "            sudo dpkg -i /home/snake/Arm-Pi-Tweaker/gpu/proprietary/libmali-valhall-g610-*_arm64.deb || true\n"
        "            sudo apt-get install -f -y\n"
        "        fi\n"
        "        ;;\n"
        "    *Mesa*|*Panfrost*)\n"
        "        log_gpu \"Installing Mesa/Panfrost driver...\"\n"
        "        sudo apt-get update\n"
        "        sudo apt-get install -y mesa-utils mesa-vulkan-drivers\n"
        "        # Remove Mali proprietary if present\n"
        "        sudo apt-get remove --purge -y libmali* || true\n"
        "        ;;\n"
        "    *Software*)\n"
        "        log_gpu \"Switching to software rendering...\"\n"
        "        # Disable hardware acceleration\n"
        "        sudo apt-get remove --purge -y libmali* mesa-vulkan-drivers || true\n"
        "        ;;\n"
        "    *)\n"
        "        log_gpu \"❌ Unknown driver type: $DRIVER_TYPE\"\n"
        "        exit 1\n"
        "        ;;\n"
        "esac\n"
        "\n"
        "# Update library cache\n"
        "sudo ldconfig\n"
        "\n"
        "# Restart display manager\n"
        "if systemctl list-unit-files | grep -q display-manager; then\n"
        "    sudo systemctl start display-manager\n"
        "fi\n"
        "\n"
        "log_gpu \"✅ GPU driver switch completed\"\n"
    ).arg(driverType);
    
    QString scriptPath = "/tmp/switch_gpu_driver.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        m_simulatedProgress = 0;
        m_progressTimer->start(1000);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create switch script");
    }
}

void SystemManager::testGpuDriver()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "test_gpu_driver";
    emit statusUpdated("Testing GPU driver functionality...");
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    QString script = 
        "#!/bin/bash\n"
        "echo \"🧪 GPU Driver Test Results\"\n"
        "echo \"=========================\"\n"
        "echo \"\"\n"
        "echo \"OpenGL Information:\"\n"
        "glxinfo -B 2>/dev/null | grep -E '(OpenGL vendor|OpenGL renderer|OpenGL version)' || echo \"OpenGL not available\"\n"
        "echo \"\"\n"
        "echo \"Vulkan Information:\"\n"
        "vulkaninfo --summary 2>/dev/null | head -10 || echo \"Vulkan not available\"\n"
        "echo \"\"\n"
        "echo \"EGL Information:\"\n"
        "eglinfo 2>/dev/null | head -5 || echo \"EGL not available\"\n"
        "echo \"\"\n"
        "echo \"GPU Memory:\"\n"
        "cat /proc/meminfo | grep -i gpu || echo \"GPU memory info not available\"\n"
        "echo \"\"\n"
        "echo \"Display Driver:\"\n"
        "lsmod | grep -E '(mali|panfrost|drm)' || echo \"No GPU modules loaded\"\n";
    
    QString scriptPath = "/tmp/test_gpu_driver.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create test script");
    }
}

QString SystemManager::detectCurrentGpuDriver()
{
    QProcess process;
    process.start("lsmod", QStringList());
    process.waitForFinished(3000);
    
    QString output = process.readAllStandardOutput();
    
    if (output.contains("mali_kbase")) {
        return "Mali Proprietary Driver";
    } else if (output.contains("panfrost")) {
        return "Panfrost (Open Source)";
    } else if (output.contains("drm")) {
        return "Generic DRM Driver";
    } else {
        return "Software Rendering";
    }
}

QStringList SystemManager::scanAvailableGpuDrivers()
{
    QStringList drivers;
    
    // Scan GPU directory
    QString gpuDir = "/home/snake/Arm-Pi-Tweaker/gpu";
    QDir gpu(gpuDir);
    
    if (gpu.exists()) {
        // Scan proprietary drivers
        QDir proprietary(gpuDir + "/proprietary");
        if (proprietary.exists()) {
            QStringList debFiles = proprietary.entryList(QStringList() << "*.deb", QDir::Files);
            for (const QString &deb : debFiles) {
                drivers.append(QString("Proprietary: %1").arg(deb));
            }
        }
        
        // Scan Mesa drivers
        QDir mesa(gpuDir + "/mesa");
        if (mesa.exists()) {
            QStringList mesaFiles = mesa.entryList(QStringList() << "*.deb", QDir::Files);
            for (const QString &deb : mesaFiles) {
                drivers.append(QString("Mesa: %1").arg(deb));
            }
        }
    }
    
    // Check for system packages
    QProcess dpkgProcess;
    dpkgProcess.start("dpkg", QStringList() << "-l" << "*mali*" << "*mesa*" << "*panfrost*");
    dpkgProcess.waitForFinished(5000);
    
    QString dpkgOutput = dpkgProcess.readAllStandardOutput();
    if (dpkgOutput.contains("libmali")) {
        drivers.append("System: Mali driver package");
    }
    if (dpkgOutput.contains("mesa")) {
        drivers.append("System: Mesa driver package");
    }
    
    return drivers;
}

// Kernel Management Implementation
void SystemManager::installKernel(const QString &kernelPackage)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "install_kernel";
    emit statusUpdated(QString("Installing kernel: %1").arg(kernelPackage));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    QStringList args;
    args << "install" << "-y" << kernelPackage;
    
    m_simulatedProgress = 0;
    m_progressTimer->start(2000);
    emit progressUpdated(0);
    
    m_currentProcess->start("sudo", QStringList() << "apt-get" << args);
}

void SystemManager::removeKernel(const QString &kernelVersion)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "remove_kernel";
    emit statusUpdated(QString("Removing kernel: %1").arg(kernelVersion));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "KERNEL_VERSION='%1'\n"
        "\n"
        "echo \"Removing kernel $KERNEL_VERSION...\"\n"
        "\n"
        "# Remove kernel image\n"
        "sudo rm -f /boot/vmlinuz-$KERNEL_VERSION\n"
        "sudo rm -f /boot/initrd.img-$KERNEL_VERSION\n"
        "sudo rm -f /boot/config-$KERNEL_VERSION\n"
        "sudo rm -f /boot/System.map-$KERNEL_VERSION\n"
        "\n"
        "# Remove kernel modules\n"
        "sudo rm -rf /lib/modules/$KERNEL_VERSION\n"
        "\n"
        "# Remove kernel packages\n"
        "sudo apt-get remove --purge -y linux-image-$KERNEL_VERSION linux-headers-$KERNEL_VERSION || true\n"
        "\n"
        "# Update GRUB\n"
        "sudo update-grub\n"
        "\n"
        "echo \"Kernel $KERNEL_VERSION removed successfully\"\n"
    ).arg(kernelVersion);
    
    QString scriptPath = "/tmp/remove_kernel.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        
        m_simulatedProgress = 0;
        m_progressTimer->start(1000);
        emit progressUpdated(0);
        
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create removal script");
    }
}

void SystemManager::setDefaultKernel(const QString &kernelVersion)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "set_default_kernel";
    emit statusUpdated(QString("Setting default kernel: %1").arg(kernelVersion));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "KERNEL_VERSION='%1'\n"
        "\n"
        "echo \"Setting default kernel to $KERNEL_VERSION...\"\n"
        "\n"
        "# Update GRUB default\n"
        "GRUB_ENTRY=$(grep -n \"menuentry.*$KERNEL_VERSION\" /boot/grub/grub.cfg | head -1 | cut -d: -f1)\n"
        "if [ -n \"$GRUB_ENTRY\" ]; then\n"
        "    GRUB_INDEX=$((GRUB_ENTRY - 1))\n"
        "    sudo sed -i \"s/GRUB_DEFAULT=.*/GRUB_DEFAULT=$GRUB_INDEX/\" /etc/default/grub\n"
        "    sudo update-grub\n"
        "    echo \"Default kernel set to $KERNEL_VERSION (index $GRUB_INDEX)\"\n"
        "else\n"
        "    echo \"Kernel $KERNEL_VERSION not found in GRUB menu\"\n"
        "    exit 1\n"
        "fi\n"
    ).arg(kernelVersion);
    
    QString scriptPath = "/tmp/set_default_kernel.sh";
    QFile scriptFile(scriptPath);
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&scriptFile);
        out << script;
        scriptFile.close();
        
        QProcess::execute("chmod", QStringList() << "+x" << scriptPath);
        m_currentProcess->start("bash", QStringList() << scriptPath);
    } else {
        emit operationCompleted(false, "Failed to create script");
    }
}

void SystemManager::updateInitramfs(const QString &kernelVersion)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "update_initramfs";
    emit statusUpdated(QString("Updating initramfs for kernel: %1").arg(kernelVersion));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    QStringList args;
    if (kernelVersion == "all") {
        args << "update-initramfs" << "-u" << "-k" << "all";
    } else {
        args << "update-initramfs" << "-u" << "-k" << kernelVersion;
    }
    
    m_currentProcess->start("sudo", args);
}

void SystemManager::updateGrub()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "update_grub";
    emit statusUpdated("Updating GRUB bootloader configuration...");
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    m_currentProcess->start("sudo", QStringList() << "update-grub");
}

QStringList SystemManager::getInstalledKernels()
{
    QStringList kernels;
    QDir bootDir("/boot");
    QStringList kernelFiles = bootDir.entryList(QStringList() << "vmlinuz-*", QDir::Files);
    
    for (const QString &kernel : kernelFiles) {
        QString version = kernel.mid(8); // Remove "vmlinuz-"
        kernels.append(version);
    }
    
    return kernels;
}

QString SystemManager::getCurrentKernel()
{
    QProcess process;
    process.start("uname", QStringList() << "-r");
    process.waitForFinished(2000);
    return process.readAllStandardOutput().trimmed();
}

QString SystemManager::getDefaultKernel()
{
    // This is simplified - real implementation would parse GRUB config
    QFile grubDefault("/etc/default/grub");
    if (grubDefault.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&grubDefault);
        QString content = stream.readAll();
        grubDefault.close();
        
        // Look for GRUB_DEFAULT setting
        QRegularExpression rx("GRUB_DEFAULT=(.*)");
        QRegularExpressionMatch match = rx.match(content);
        if (match.hasMatch()) {
            QString defaultEntry = match.captured(1).trimmed().remove('"');
            if (defaultEntry == "0") {
                return "Latest kernel (auto)";
            }
            return QString("Entry %1").arg(defaultEntry);
        }
    }
    return "Unknown";
}

// Module Management Implementation
void SystemManager::loadKernelModule(const QString &moduleName)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "load_module";
    emit statusUpdated(QString("Loading kernel module: %1").arg(moduleName));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    m_currentProcess->start("sudo", QStringList() << "modprobe" << moduleName);
}

void SystemManager::unloadKernelModule(const QString &moduleName)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "unload_module";
    emit statusUpdated(QString("Unloading kernel module: %1").arg(moduleName));
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SystemManager::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &SystemManager::onProcessError);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &SystemManager::onProcessOutput);
    
    m_currentProcess->start("sudo", QStringList() << "modprobe" << "-r" << moduleName);
}

void SystemManager::blacklistKernelModule(const QString &moduleName)
{
    emit statusUpdated(QString("Blacklisting kernel module: %1").arg(moduleName));
    
    QString blacklistFile = "/etc/modprobe.d/blacklist-armpi.conf";
    QFile file(blacklistFile);
    
    // Check if already blacklisted
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString content = stream.readAll();
        file.close();
        
        if (content.contains(QString("blacklist %1").arg(moduleName))) {
            emit statusUpdated(QString("Module %1 is already blacklisted").arg(moduleName));
            return;
        }
    }
    
    // Add to blacklist
    QProcess process;
    process.start("sudo", QStringList() << "bash" << "-c" 
        << QString("echo 'blacklist %1' >> %2").arg(moduleName, blacklistFile));
    process.waitForFinished(3000);
    
    if (process.exitCode() == 0) {
        emit statusUpdated(QString("Module %1 blacklisted successfully").arg(moduleName));
    } else {
        emit statusUpdated(QString("Failed to blacklist module %1").arg(moduleName));
    }
}

QStringList SystemManager::getLoadedModules()
{
    QStringList modules;
    QProcess process;
    process.start("lsmod", QStringList());
    process.waitForFinished(3000);
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');
    
    for (int i = 1; i < lines.size(); ++i) { // Skip header
        QString line = lines[i].trimmed();
        if (!line.isEmpty()) {
            QString moduleName = line.split(' ').first();
            modules.append(moduleName);
        }
    }
    
    return modules;
}

QStringList SystemManager::getAvailableModules()
{
    QStringList modules;
    QString currentKernel = getCurrentKernel();
    QString modulesPath = QString("/lib/modules/%1").arg(currentKernel);
    
    QProcess process;
    process.start("find", QStringList() << modulesPath << "-name" << "*.ko" << "-type" << "f");
    process.waitForFinished(5000);
    
    QString output = process.readAllStandardOutput();
    QStringList files = output.split('\n');
    
    for (const QString &file : files) {
        if (!file.isEmpty()) {
            QFileInfo info(file);
            QString moduleName = info.baseName();
            if (!modules.contains(moduleName)) {
                modules.append(moduleName);
            }
        }
    }
    
    modules.sort();
    return modules;
}

QString SystemManager::getModuleInfo(const QString &moduleName)
{
    QProcess process;
    process.start("modinfo", QStringList() << moduleName);
    process.waitForFinished(3000);
    
    if (process.exitCode() == 0) {
        return process.readAllStandardOutput();
    } else {
        return QString("Module information not available for: %1").arg(moduleName);
    }
}

// Kernel Patching Implementation (simplified)
void SystemManager::applyKernelPatch(const QString &patchFile)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        emit statusUpdated("Another operation is already running");
        return;
    }
    
    m_currentOperation = "apply_patch";
    emit statusUpdated(QString("Applying kernel patch: %1").arg(QFileInfo(patchFile).fileName()));
    
    // This is a simplified implementation
    emit statusUpdated("Kernel patching requires manual review and is not automated");
    emit operationCompleted(false, "Manual patching required for safety");
}

void SystemManager::revertKernelPatch(const QString &patchName)
{
    emit statusUpdated(QString("Reverting patch: %1").arg(patchName));
    emit statusUpdated("Patch reverting requires manual review and is not automated");
    emit operationCompleted(false, "Manual patch reverting required for safety");
}

void SystemManager::createKernelPatch(const QString &originalFile, const QString &modifiedFile)
{
    emit statusUpdated("Creating patch between files...");
    
    QProcess process;
    QString patchName = QString("armpi_patch_%1.patch").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString patchPath = QString("/home/snake/Arm-Pi-Tweaker/patches/%1").arg(patchName);
    
    QDir().mkpath("/home/snake/Arm-Pi-Tweaker/patches");
    
    process.start("diff", QStringList() << "-u" << originalFile << modifiedFile);
    process.waitForFinished(5000);
    
    QString patchContent = process.readAllStandardOutput();
    
    QFile patchFile(patchPath);
    if (patchFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&patchFile);
        stream << patchContent;
        patchFile.close();
        
        emit statusUpdated(QString("Patch created: %1").arg(patchPath));
        emit operationCompleted(true, QString("Patch saved to: %1").arg(patchPath));
    } else {
        emit operationCompleted(false, "Failed to save patch file");
    }
}

QStringList SystemManager::getAppliedPatches()
{
    QStringList patches;
    QDir patchDir("/home/snake/Arm-Pi-Tweaker/patches");
    
    if (patchDir.exists()) {
        QStringList patchFiles = patchDir.entryList(QStringList() << "*.patch", QDir::Files);
        for (const QString &patch : patchFiles) {
            patches.append(patch);
        }
    }
    
    return patches;
}

// Live Configuration Implementation
void SystemManager::applyKernelParameter(const QString &parameter, const QString &value)
{
    emit statusUpdated(QString("Applying kernel parameter: %1=%2").arg(parameter, value));
    
    QString paramPath = parameter;
    paramPath.replace('.', '/');
    QString command = QString("echo '%1' | sudo tee /proc/sys/%2").arg(value, paramPath);
    
    QProcess process;
    process.start("bash", QStringList() << "-c" << command);
    process.waitForFinished(3000);
    
    if (process.exitCode() == 0) {
        emit statusUpdated(QString("Kernel parameter %1 applied successfully").arg(parameter));
        emit operationCompleted(true, QString("Parameter %1 set to %2").arg(parameter, value));
    } else {
        emit operationCompleted(false, QString("Failed to apply parameter %1").arg(parameter));
    }
}

void SystemManager::updateBootParameters(const QStringList &parameters)
{
    emit statusUpdated("Updating boot parameters...");
    
    // Update GRUB cmdline
    QString newCmdline = parameters.join(" ");
    QString command = QString("sudo sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT=.*/GRUB_CMDLINE_LINUX_DEFAULT=\"%1\"/' /etc/default/grub").arg(newCmdline);
    
    QProcess process;
    process.start("bash", QStringList() << "-c" << command);
    process.waitForFinished(3000);
    
    if (process.exitCode() == 0) {
        // Update GRUB
        updateGrub();
        emit statusUpdated("Boot parameters updated successfully");
    } else {
        emit operationCompleted(false, "Failed to update boot parameters");
    }
}

void SystemManager::updateKernelConfig(const QString &configOption, const QString &value)
{
    emit statusUpdated(QString("Updating kernel config: %1=%2").arg(configOption, value));
    
    // This is a placeholder - actual kernel config updating requires kernel rebuild
    emit statusUpdated("Kernel config updating requires kernel rebuild - not implemented for safety");
    emit operationCompleted(false, "Kernel config modification requires manual review");
}

