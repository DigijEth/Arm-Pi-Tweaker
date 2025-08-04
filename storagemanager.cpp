#include "storagemanager.h"
#include "systemmanager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QDateTime>

StorageManager::StorageManager(SystemManager *systemManager, QWidget *parent)
    : QWidget(parent)
    , m_systemManager(systemManager)
    , m_currentProcess(nullptr)
    , m_isLiveSystem(false)
{
    setupUI();
    
    // Set up scan timer
    m_scanTimer = new QTimer(this);
    connect(m_scanTimer, &QTimer::timeout, this, &StorageManager::scanStorageDevices);
    m_scanTimer->start(5000); // Scan every 5 seconds
    
    // Initial scan
    QTimer::singleShot(100, this, [this]() {
        detectSystemInstallation();
        scanStorageDevices();
    });
}

void StorageManager::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *title = new QLabel("Storage Manager");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #000000; margin: 10px;");
    mainLayout->addWidget(title);
    
    // Create groups
    createSystemInfoGroup();
    createDeviceListGroup();
    createActionsGroup();
    createProgressGroup();
    
    // Top horizontal layout for System Info and Storage Operations
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_systemInfoGroup);
    topLayout->addWidget(m_actionsGroup);
    mainLayout->addLayout(topLayout);
    
    mainLayout->addWidget(m_deviceListGroup);
    mainLayout->addWidget(m_progressGroup);
    mainLayout->addStretch();
}

void StorageManager::createSystemInfoGroup()
{
    m_systemInfoGroup = new QGroupBox("💾 System Installation Info");
    m_systemInfoGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    m_systemInfoGroup->setMaximumHeight(120);  // Limit height
    
    QVBoxLayout *layout = new QVBoxLayout(m_systemInfoGroup);
    layout->setContentsMargins(10, 5, 10, 5);  // Reduced top/bottom padding
    layout->setSpacing(2);  // Tight spacing
    
    m_systemLocationLabel = new QLabel("System Location: Detecting...");
    m_systemLocationLabel->setStyleSheet("color: #000000; font-weight: bold; font-size: 9pt;");
    layout->addWidget(m_systemLocationLabel);
    
    m_systemTypeLabel = new QLabel("System Type: Detecting...");
    m_systemTypeLabel->setStyleSheet("color: #000000; font-size: 9pt;");
    layout->addWidget(m_systemTypeLabel);
    
    m_bootDeviceLabel = new QLabel("Boot Device: Detecting...");
    m_bootDeviceLabel->setStyleSheet("color: #000000; font-size: 9pt;");
    layout->addWidget(m_bootDeviceLabel);
}

void StorageManager::createDeviceListGroup()
{
    m_deviceListGroup = new QGroupBox("📱 Storage Devices");
    m_deviceListGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_deviceListGroup);
    
    // Device list
    m_deviceList = new QListWidget();
    m_deviceList->setMaximumHeight(200);
    m_deviceList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    connect(m_deviceList, &QListWidget::itemSelectionChanged,
            this, &StorageManager::onDeviceSelectionChanged);
    layout->addWidget(m_deviceList);
    
    // Device info
    m_deviceInfoText = new QTextEdit();
    m_deviceInfoText->setMaximumHeight(100);
    m_deviceInfoText->setReadOnly(true);
    m_deviceInfoText->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    layout->addWidget(m_deviceInfoText);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_refreshButton = new QPushButton("🔄 Refresh");
    m_refreshButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_refreshButton, &QPushButton::clicked, this, &StorageManager::scanStorageDevices);
    buttonLayout->addWidget(m_refreshButton);
    
    m_mountButton = new QPushButton("📌 Mount");
    m_mountButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #00FF00; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_mountButton->setEnabled(false);
    connect(m_mountButton, &QPushButton::clicked, this, &StorageManager::onMountDevice);
    buttonLayout->addWidget(m_mountButton);
    
    m_unmountButton = new QPushButton("⏏️ Unmount");
    m_unmountButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF0000; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_unmountButton->setEnabled(false);
    connect(m_unmountButton, &QPushButton::clicked, this, &StorageManager::onUnmountDevice);
    buttonLayout->addWidget(m_unmountButton);
    
    layout->addLayout(buttonLayout);
}

void StorageManager::createActionsGroup()
{
    m_actionsGroup = new QGroupBox("🛠️ Storage Operations");
    m_actionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    m_actionsGroup->setMaximumHeight(200);  // Limit height
    
    QHBoxLayout *mainLayout = new QHBoxLayout(m_actionsGroup);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    
    // Left side - Options
    QVBoxLayout *optionsLayout = new QVBoxLayout();
    optionsLayout->setSpacing(2);
    
    // Target device selection
    QHBoxLayout *targetLayout = new QHBoxLayout();
    QLabel *targetLabel = new QLabel("Target:");
    targetLabel->setStyleSheet("color: #000000; font-size: 9pt;");
    targetLayout->addWidget(targetLabel);
    
    m_targetDeviceCombo = new QComboBox();
    m_targetDeviceCombo->setStyleSheet(
        "QComboBox { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; padding: 2px; font-size: 9pt; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: #F0F0F0; color: #000000; selection-background-color: #000000; selection-color: #FFFFFF; }"
    );
    m_targetDeviceCombo->setMaximumWidth(150);
    targetLayout->addWidget(m_targetDeviceCombo);
    targetLayout->addStretch();
    optionsLayout->addLayout(targetLayout);
    
    // Options checkboxes
    m_includeHomeCheck = new QCheckBox("Include /home");
    m_includeHomeCheck->setStyleSheet("color: #000000; font-size: 9pt;");
    m_includeHomeCheck->setChecked(true);
    optionsLayout->addWidget(m_includeHomeCheck);
    
    m_compressCheck = new QCheckBox("Compress");
    m_compressCheck->setStyleSheet("color: #000000; font-size: 9pt;");
    optionsLayout->addWidget(m_compressCheck);
    
    m_verifyCheck = new QCheckBox("Verify");
    m_verifyCheck->setStyleSheet("color: #000000; font-size: 9pt;");
    m_verifyCheck->setChecked(true);
    optionsLayout->addWidget(m_verifyCheck);
    
    mainLayout->addLayout(optionsLayout);
    mainLayout->addSpacing(20);
    
    // Right side - Action buttons in 2x2 grid
    QGridLayout *buttonGrid = new QGridLayout();
    buttonGrid->setSpacing(5);
    
    // Copy Live Image button
    m_copyLiveImageButton = new QPushButton("💿");
    m_copyLiveImageButton->setFixedSize(40, 40);
    m_copyLiveImageButton->setStyleSheet("QPushButton { background-color: #F0F0F0; font-size: 20px; border: 2px solid #000000; border-radius: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_copyLiveImageButton->setToolTip("Copy Live Image to Storage");
    connect(m_copyLiveImageButton, &QPushButton::clicked, this, &StorageManager::onCopyLiveImage);
    buttonGrid->addWidget(m_copyLiveImageButton, 0, 0);
    
    // Burn SD Card button
    m_burnSDCardButton = new QPushButton("🔥");
    m_burnSDCardButton->setFixedSize(40, 40);
    m_burnSDCardButton->setStyleSheet("QPushButton { background-color: #F0F0F0; font-size: 20px; border: 2px solid #000000; border-radius: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_burnSDCardButton->setToolTip("Burn Image to SD Card");
    connect(m_burnSDCardButton, &QPushButton::clicked, this, &StorageManager::onBurnToSDCard);
    buttonGrid->addWidget(m_burnSDCardButton, 0, 1);
    
    // Create Snapshot button
    m_createSnapshotButton = new QPushButton("📸");
    m_createSnapshotButton->setFixedSize(40, 40);
    m_createSnapshotButton->setStyleSheet("QPushButton { background-color: #F0F0F0; font-size: 20px; border: 2px solid #000000; border-radius: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_createSnapshotButton->setToolTip("Create System Snapshot");
    connect(m_createSnapshotButton, &QPushButton::clicked, this, &StorageManager::onCreateSnapshot);
    buttonGrid->addWidget(m_createSnapshotButton, 1, 0);
    
    // 1:1 Drive Copy button
    m_driveCopyButton = new QPushButton("💾");
    m_driveCopyButton->setFixedSize(40, 40);
    m_driveCopyButton->setStyleSheet("QPushButton { background-color: #F0F0F0; font-size: 20px; border: 2px solid #000000; border-radius: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_driveCopyButton->setToolTip("1:1 Drive Copy");
    connect(m_driveCopyButton, &QPushButton::clicked, this, &StorageManager::onDriveCopy);
    buttonGrid->addWidget(m_driveCopyButton, 1, 1);
    
    mainLayout->addLayout(buttonGrid);
    mainLayout->addStretch();
}

void StorageManager::createProgressGroup()
{
    m_progressGroup = new QGroupBox("📊 Operation Progress");
    m_progressGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    m_progressGroup->setVisible(false);
    
    QVBoxLayout *layout = new QVBoxLayout(m_progressGroup);
    
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("color: #000000; font-weight: bold;");
    layout->addWidget(m_statusLabel);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 2px solid #000000; border-radius: 5px; background-color: #F0F0F0; }"
        "QProgressBar::chunk { background-color: #000000; }"
    );
    layout->addWidget(m_progressBar);
    
    m_logOutput = new QTextEdit();
    m_logOutput->setMaximumHeight(150);
    m_logOutput->setReadOnly(true);
    m_logOutput->setStyleSheet("background-color: #000000; color: #00FF00; font-family: monospace; border: 1px solid #00FF00;");
    layout->addWidget(m_logOutput);
    
    m_cancelButton = new QPushButton("❌ Cancel");
    m_cancelButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF0000; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_cancelButton, &QPushButton::clicked, [this]() {
        if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
            m_currentProcess->terminate();
            m_statusLabel->setText("Operation cancelled");
        }
    });
    layout->addWidget(m_cancelButton);
}

void StorageManager::detectSystemInstallation()
{
    // Check if running from live system
    QFile cmdlineFile("/proc/cmdline");
    if (cmdlineFile.open(QIODevice::ReadOnly)) {
        QString cmdline = cmdlineFile.readAll();
        cmdlineFile.close();
        
        if (cmdline.contains("toram") || cmdline.contains("live")) {
            m_isLiveSystem = true;
            m_systemTypeLabel->setText("System Type: Live System");
        } else {
            m_isLiveSystem = false;
            m_systemTypeLabel->setText("System Type: Installed System");
        }
    }
    
    // Find root device
    QProcess *dfProcess = new QProcess(this);
    connect(dfProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, dfProcess](int, QProcess::ExitStatus) {
        QString output = dfProcess->readAllStandardOutput();
        QStringList lines = output.split('\n');
        
        for (const QString &line : lines) {
            if (line.contains(" / ")) {
                QStringList parts = line.split(QRegularExpression("\\s+"));
                if (!parts.isEmpty()) {
                    m_systemDevice = parts.first();
                    m_systemLocationLabel->setText(QString("System Location: %1").arg(m_systemDevice));
                    break;
                }
            }
        }
        dfProcess->deleteLater();
    });
    dfProcess->start("df", QStringList() << "-h");
    
    // Get boot device
    QFile fstabFile("/etc/fstab");
    if (fstabFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&fstabFile);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains("/boot") && !line.startsWith("#")) {
                QStringList parts = line.split(QRegularExpression("\\s+"));
                if (!parts.isEmpty()) {
                    m_bootDeviceLabel->setText(QString("Boot Device: %1").arg(parts.first()));
                    break;
                }
            }
        }
        fstabFile.close();
    }
}

void StorageManager::scanStorageDevices()
{
    m_devices.clear();
    m_deviceList->clear();
    
    // Use lsblk to get device information
    QProcess *lsblkProcess = new QProcess(this);
    connect(lsblkProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, lsblkProcess](int, QProcess::ExitStatus) {
        QString output = lsblkProcess->readAllStandardOutput();
        QStringList lines = output.split('\n');
        
        for (const QString &line : lines) {
            if (line.isEmpty()) continue;
            
            // Parse KEY="value" format from lsblk -P
            QRegularExpression rx("(\\w+)=\"([^\"]*)\"");
            QRegularExpressionMatchIterator it = rx.globalMatch(line);
            
            QMap<QString, QString> deviceInfo;
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                deviceInfo[match.captured(1)] = match.captured(2);
            }
            
            // Skip loop devices
            if (deviceInfo["TYPE"] == "loop") continue;
            
            // Only process disk and partition entries
            if (deviceInfo["TYPE"] == "disk" || deviceInfo["TYPE"] == "part") {
                StorageDevice device;
                device.device = "/dev/" + deviceInfo["NAME"];
                device.size = deviceInfo["SIZE"];
                device.filesystem = deviceInfo["FSTYPE"];
                device.mountPoint = deviceInfo["MOUNTPOINT"];
                device.label = deviceInfo["LABEL"];
                device.isRemovable = (deviceInfo["RM"] == "1");
                device.isMounted = !device.mountPoint.isEmpty();
                device.isSystemDrive = (device.device == m_systemDevice) || 
                                      (device.mountPoint == "/");
                
                // Get usage info if mounted
                if (device.isMounted && !device.mountPoint.isEmpty()) {
                    QStorageInfo storageInfo(device.mountPoint);
                    device.used = formatSize(storageInfo.bytesTotal() - storageInfo.bytesAvailable());
                    device.available = formatSize(storageInfo.bytesAvailable());
                }
                
                m_devices[device.device] = device;
                
                // Add to list
                QString icon = device.isRemovable ? "💾" : "💿";
                if (device.isSystemDrive) icon = "🖥️";
                QString status = device.isMounted ? " [Mounted]" : "";
                
                QListWidgetItem *item = new QListWidgetItem(
                    QString("%1 %2 - %3%4").arg(icon, device.device, device.size, status)
                );
                item->setData(Qt::UserRole, device.device);
                m_deviceList->addItem(item);
            }
        }
        
        lsblkProcess->deleteLater();
        
        // Update target device combo
        m_targetDeviceCombo->clear();
        for (const StorageDevice &device : m_devices) {
            if (!device.isSystemDrive && device.size != "0B") {
                m_targetDeviceCombo->addItem(device.device + " - " + device.size, device.device);
            }
        }
    });
    
    lsblkProcess->start("lsblk", QStringList() 
        << "-o" << "NAME,FSTYPE,LABEL,SIZE,RM,TYPE,MOUNTPOINT"
        << "-n" << "-P");
}

void StorageManager::onDeviceSelectionChanged()
{
    QListWidgetItem *item = m_deviceList->currentItem();
    if (!item) {
        m_mountButton->setEnabled(false);
        m_unmountButton->setEnabled(false);
        return;
    }
    
    QString devicePath = item->data(Qt::UserRole).toString();
    m_selectedDevice = devicePath;
    
    if (m_devices.contains(devicePath)) {
        const StorageDevice &device = m_devices[devicePath];
        updateDeviceInfo();
        
        // Enable/disable buttons
        m_mountButton->setEnabled(!device.isMounted && !device.isSystemDrive);
        m_unmountButton->setEnabled(device.isMounted && !device.isSystemDrive);
    }
}

void StorageManager::updateDeviceInfo()
{
    if (!m_devices.contains(m_selectedDevice)) return;
    
    const StorageDevice &device = m_devices[m_selectedDevice];
    
    QString info = QString(
        "Device: %1\n"
        "Label: %2\n"
        "Filesystem: %3\n"
        "Size: %4\n"
        "Mount Point: %5\n"
        "Type: %6\n"
    ).arg(device.device)
     .arg(device.label.isEmpty() ? "None" : device.label)
     .arg(device.filesystem.isEmpty() ? "Unknown" : device.filesystem)
     .arg(device.size)
     .arg(device.mountPoint.isEmpty() ? "Not mounted" : device.mountPoint)
     .arg(device.isRemovable ? "Removable" : "Fixed");
    
    if (device.isMounted) {
        info += QString("Used: %1\nAvailable: %2\n").arg(device.used, device.available);
    }
    
    m_deviceInfoText->setPlainText(info);
}

void StorageManager::onMountDevice()
{
    if (!m_devices.contains(m_selectedDevice)) return;
    
    const StorageDevice &device = m_devices[m_selectedDevice];
    
    // Create mount point
    QString mountPoint = "/mnt/" + QFileInfo(device.device).fileName();
    
    m_statusLabel->setText(QString("Mounting %1...").arg(device.device));
    m_progressGroup->setVisible(true);
    
    // Create mount point directory
    QProcess *mkdirProcess = new QProcess(this);
    connect(mkdirProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, device, mountPoint, mkdirProcess](int, QProcess::ExitStatus) {
        mkdirProcess->deleteLater();
        
        // Mount the device
        executeCommand("mount", QStringList() << device.device << mountPoint);
    });
    
    mkdirProcess->start("sudo", QStringList() << "mkdir" << "-p" << mountPoint);
}

void StorageManager::onUnmountDevice()
{
    if (!m_devices.contains(m_selectedDevice)) return;
    
    const StorageDevice &device = m_devices[m_selectedDevice];
    
    m_statusLabel->setText(QString("Unmounting %1...").arg(device.device));
    m_progressGroup->setVisible(true);
    
    executeCommand("umount", QStringList() << device.device);
}

void StorageManager::onCopyLiveImage()
{
    if (!m_isLiveSystem) {
        QMessageBox::warning(this, "Not a Live System",
            "This function is only available when running from a live image.");
        return;
    }
    
    QString targetDevice = m_targetDeviceCombo->currentData().toString();
    if (targetDevice.isEmpty()) {
        QMessageBox::warning(this, "No Target Selected",
            "Please select a target device for installation.");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Copy Live Image",
        QString("This will copy the live system to %1.\n\n"
                "ALL DATA ON THE TARGET DEVICE WILL BE LOST!\n\n"
                "Continue?").arg(targetDevice),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply != QMessageBox::Yes) return;
    
    m_currentOperation = "Copying live image";
    m_progressGroup->setVisible(true);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_statusLabel->setText("Preparing to copy live image...");
    m_logOutput->clear();
    
    // Use Orange Pi's method for live image installation
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "TARGET='%1'\n"
        "echo 'Partitioning target device...'\n"
        "parted -s $TARGET mklabel gpt\n"
        "parted -s $TARGET mkpart primary fat32 1MiB 513MiB\n"
        "parted -s $TARGET mkpart primary ext4 513MiB 100%%\n"
        "parted -s $TARGET set 1 esp on\n"
        "echo 'Creating filesystems...'\n"
        "mkfs.vfat -F32 ${TARGET}1\n"
        "mkfs.ext4 -F ${TARGET}2\n"
        "echo 'Mounting partitions...'\n"
        "mkdir -p /mnt/target\n"
        "mount ${TARGET}2 /mnt/target\n"
        "mkdir -p /mnt/target/boot/efi\n"
        "mount ${TARGET}1 /mnt/target/boot/efi\n"
        "echo 'Copying system files...'\n"
        "rsync -aAXv --exclude={'/dev/*','/proc/*','/sys/*','/tmp/*','/run/*','/mnt/*','/media/*','/lost+found'} / /mnt/target/\n"
        "echo 'Installing bootloader...'\n"
        "mount --bind /dev /mnt/target/dev\n"
        "mount --bind /proc /mnt/target/proc\n"
        "mount --bind /sys /mnt/target/sys\n"
        "chroot /mnt/target grub-install --target=arm64-efi --efi-directory=/boot/efi --bootloader-id=OrangePi\n"
        "chroot /mnt/target update-grub\n"
        "echo 'Updating fstab...'\n"
        "BOOT_UUID=$(blkid -s UUID -o value ${TARGET}1)\n"
        "ROOT_UUID=$(blkid -s UUID -o value ${TARGET}2)\n"
        "echo \"UUID=$ROOT_UUID / ext4 defaults 0 1\" > /mnt/target/etc/fstab\n"
        "echo \"UUID=$BOOT_UUID /boot/efi vfat defaults 0 1\" >> /mnt/target/etc/fstab\n"
        "echo 'Cleaning up...'\n"
        "umount -l /mnt/target/dev\n"
        "umount -l /mnt/target/proc\n"
        "umount -l /mnt/target/sys\n"
        "umount /mnt/target/boot/efi\n"
        "umount /mnt/target\n"
        "echo 'Installation complete!'\n"
    ).arg(targetDevice);
    
    QFile scriptFile("/tmp/install_live.sh");
    if (scriptFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&scriptFile);
        stream << script;
        scriptFile.close();
        scriptFile.setPermissions(QFile::ExeUser | QFile::ReadUser | QFile::WriteUser);
    }
    
    executeCommand("bash", QStringList() << "/tmp/install_live.sh");
}

void StorageManager::onBurnToSDCard()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "Select Image File",
        QDir::homePath(), "Image Files (*.img *.iso *.raw);;All Files (*)");
        
    if (imagePath.isEmpty()) return;
    
    QString targetDevice = m_targetDeviceCombo->currentData().toString();
    if (targetDevice.isEmpty()) {
        QMessageBox::warning(this, "No Target Selected",
            "Please select a target SD card.");
        return;
    }
    
    // Check if target is removable
    if (m_devices.contains(targetDevice) && !m_devices[targetDevice].isRemovable) {
        QMessageBox::StandardButton reply = QMessageBox::warning(this, "Non-Removable Device",
            "The selected device appears to be a fixed drive.\n\n"
            "Are you sure you want to continue?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Burn Image to SD Card",
        QString("This will write %1 to %2.\n\n"
                "ALL DATA ON THE TARGET DEVICE WILL BE LOST!\n\n"
                "Continue?").arg(QFileInfo(imagePath).fileName(), targetDevice),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply != QMessageBox::Yes) return;
    
    m_currentOperation = "Burning image to SD card";
    m_progressGroup->setVisible(true);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_statusLabel->setText("Writing image to SD card...");
    m_logOutput->clear();
    
    // Use dd with progress monitoring
    QStringList args;
    args << "dd" << QString("if=%1").arg(imagePath) << QString("of=%1").arg(targetDevice)
         << "bs=4M" << "status=progress" << "conv=fsync";
    
    executeCommand("sh", QStringList() << "-c" << args.join(" "));
}

void StorageManager::onCreateSnapshot()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Save System Snapshot",
        QDir::homePath() + "/system_snapshot_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".img",
        "Image Files (*.img);;All Files (*)");
        
    if (savePath.isEmpty()) return;
    
    bool includeHome = m_includeHomeCheck->isChecked();
    bool compress = m_compressCheck->isChecked();
    
    m_currentOperation = "Creating system snapshot";
    m_progressGroup->setVisible(true);
    m_progressBar->setMaximum(0); // Indeterminate
    m_statusLabel->setText("Creating system snapshot...");
    m_logOutput->clear();
    
    // Create snapshot using tar or dd
    QString script = QString(
        "#!/bin/bash\n"
        "set -e\n"
        "OUTPUT='%1'\n"
        "echo 'Creating system snapshot...'\n"
    ).arg(savePath);
    
    if (compress) {
        // Use tar with compression
        script += QString(
            "tar --exclude='/dev/*' --exclude='/proc/*' --exclude='/sys/*' "
            "--exclude='/tmp/*' --exclude='/run/*' --exclude='/mnt/*' "
            "--exclude='/media/*' --exclude='/lost+found' "
            "%1 "
            "-czpf \"$OUTPUT\" /\n"
        ).arg(includeHome ? "" : "--exclude='/home/*'");
    } else {
        // Create disk image
        script += QString(
            "dd if=%1 of=\"$OUTPUT\" bs=4M status=progress conv=sync,noerror\n"
        ).arg(m_systemDevice);
    }
    
    script += "echo 'Snapshot created successfully!'\n";
    
    QFile scriptFile("/tmp/create_snapshot.sh");
    if (scriptFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&scriptFile);
        stream << script;
        scriptFile.close();
        scriptFile.setPermissions(QFile::ExeUser | QFile::ReadUser | QFile::WriteUser);
    }
    
    executeCommand("bash", QStringList() << "/tmp/create_snapshot.sh");
}

void StorageManager::onDriveCopy()
{
    // Get source device
    QString sourceDevice = m_selectedDevice;
    if (sourceDevice.isEmpty()) {
        QMessageBox::warning(this, "No Source Selected",
            "Please select a source device from the device list.");
        return;
    }
    
    QString targetDevice = m_targetDeviceCombo->currentData().toString();
    if (targetDevice.isEmpty()) {
        QMessageBox::warning(this, "No Target Selected",
            "Please select a target device.");
        return;
    }
    
    if (sourceDevice == targetDevice) {
        QMessageBox::warning(this, "Same Device",
            "Source and target devices cannot be the same.");
        return;
    }
    
    // Get device sizes
    qint64 sourceSize = 0, targetSize = 0;
    if (m_devices.contains(sourceDevice)) {
        sourceSize = m_devices[sourceDevice].size.toLongLong();
    }
    if (m_devices.contains(targetDevice)) {
        targetSize = m_devices[targetDevice].size.toLongLong();
    }
    
    QString warning = QString(
        "This will create a 1:1 copy from %1 to %2.\n\n"
        "Source size: %3\n"
        "Target size: %4\n\n"
        "ALL DATA ON THE TARGET DEVICE WILL BE LOST!\n\n"
        "Continue?"
    ).arg(sourceDevice, targetDevice, m_devices[sourceDevice].size, m_devices[targetDevice].size);
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "1:1 Drive Copy",
        warning, QMessageBox::Yes | QMessageBox::No);
        
    if (reply != QMessageBox::Yes) return;
    
    m_currentOperation = "Copying drive";
    m_progressGroup->setVisible(true);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_statusLabel->setText(QString("Copying %1 to %2...").arg(sourceDevice, targetDevice));
    m_logOutput->clear();
    
    // Use dd with progress
    QString ddCommand = QString("dd if=%1 of=%2 bs=64M status=progress conv=sync,noerror")
        .arg(sourceDevice, targetDevice);
    
    if (m_verifyCheck->isChecked()) {
        ddCommand += " && sync && echo 'Verifying copy...' && "
                     "cmp " + sourceDevice + " " + targetDevice;
    }
    
    executeCommand("sh", QStringList() << "-c" << ddCommand);
}

void StorageManager::executeCommand(const QString &command, const QStringList &args)
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        QMessageBox::warning(this, "Operation in Progress",
            "Another operation is already running. Please wait or cancel it first.");
        return;
    }
    
    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, &QProcess::readyReadStandardOutput,
            this, &StorageManager::onProcessOutput);
    connect(m_currentProcess, &QProcess::readyReadStandardError,
            this, &StorageManager::onProcessOutput);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &StorageManager::onProcessFinished);
    
    // Run with sudo
    QStringList sudoArgs;
    sudoArgs << "-S" << command;
    sudoArgs.append(args);
    
    m_currentProcess->start("sudo", sudoArgs);
}

void StorageManager::onProcessOutput()
{
    if (!m_currentProcess) return;
    
    QString output = m_currentProcess->readAllStandardOutput();
    QString error = m_currentProcess->readAllStandardError();
    
    if (!output.isEmpty()) {
        m_logOutput->append(output);
        
        // Parse dd progress
        if (output.contains("bytes") && output.contains("copied")) {
            QRegularExpression rx("(\\d+) bytes .* copied");
            QRegularExpressionMatch match = rx.match(output);
            if (match.hasMatch()) {
                qint64 bytesCopied = match.captured(1).toLongLong();
                // Update progress if we know the total size
                // This is simplified - real implementation would track source size
            }
        }
    }
    
    if (!error.isEmpty()) {
        m_logOutput->append(QString("<span style='color: #FF0000;'>%1</span>").arg(error));
    }
}

void StorageManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (!m_currentProcess) return;
    
    bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);
    
    if (success) {
        m_statusLabel->setText(m_currentOperation + " completed successfully!");
        m_progressBar->setValue(100);
        emit operationCompleted(true, m_currentOperation + " completed");
    } else {
        m_statusLabel->setText(m_currentOperation + " failed!");
        emit operationCompleted(false, m_currentOperation + " failed with exit code: " + QString::number(exitCode));
    }
    
    m_currentProcess->deleteLater();
    m_currentProcess = nullptr;
    
    // Refresh device list after operation
    QTimer::singleShot(2000, this, &StorageManager::scanStorageDevices);
}

QString StorageManager::formatSize(qint64 bytes)
{
    const qint64 kb = 1024;
    const qint64 mb = kb * 1024;
    const qint64 gb = mb * 1024;
    const qint64 tb = gb * 1024;
    
    if (bytes >= tb) {
        return QString("%1 TB").arg(bytes / double(tb), 0, 'f', 2);
    } else if (bytes >= gb) {
        return QString("%1 GB").arg(bytes / double(gb), 0, 'f', 2);
    } else if (bytes >= mb) {
        return QString("%1 MB").arg(bytes / double(mb), 0, 'f', 2);
    } else if (bytes >= kb) {
        return QString("%1 KB").arg(bytes / double(kb), 0, 'f', 2);
    } else {
        return QString("%1 B").arg(bytes);
    }
}

bool StorageManager::isLiveSystem()
{
    return m_isLiveSystem;
}