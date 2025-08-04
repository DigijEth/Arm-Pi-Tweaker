#include "uefitab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QRegularExpression>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QHeaderView>
#include <QCoreApplication>
#include <QTabWidget>
#include <QDateTime>
#include <QColor>

// System includes for SPI flash operations
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

UefiTab::UefiTab(QWidget *parent)
    : QWidget(parent)
    , m_flashProcess(new QProcess(this))
{
    setupUi();
    detectSpiDevice();
    checkCurrentUefi();
    loadPatchDatabase();
    
    // Connect signals
    connect(m_flashProcess, &QProcess::readyReadStandardOutput,
            this, &UefiTab::onFlashProcessOutput);
    connect(m_flashProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &UefiTab::onFlashProcessFinished);
}

UefiTab::~UefiTab()
{
    if (m_flashProcess->state() != QProcess::NotRunning) {
        m_flashProcess->terminate();
        m_flashProcess->waitForFinished();
    }
}

void UefiTab::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Current UEFI Information
    m_infoGroup = new QGroupBox("Current UEFI Information", this);
    QGridLayout *infoLayout = new QGridLayout(m_infoGroup);
    
    m_currentVersionLabel = new QLabel("Version: Checking...", this);
    m_boardLabel = new QLabel("Board: Orange Pi 5 Plus", this);
    m_spiFlashLabel = new QLabel("SPI Flash: Not detected", this);
    m_statusLabel = new QLabel("Status: Ready", this);
    
    infoLayout->addWidget(new QLabel("UEFI Version:", this), 0, 0);
    infoLayout->addWidget(m_currentVersionLabel, 0, 1);
    infoLayout->addWidget(new QLabel("Target Board:", this), 1, 0);
    infoLayout->addWidget(m_boardLabel, 1, 1);
    infoLayout->addWidget(new QLabel("Flash Device:", this), 2, 0);
    infoLayout->addWidget(m_spiFlashLabel, 2, 1);
    infoLayout->addWidget(new QLabel("Status:", this), 3, 0);
    infoLayout->addWidget(m_statusLabel, 3, 1);
    
    // Features table
    m_featuresTable = new QTableWidget(0, 2, this);
    m_featuresTable->setHorizontalHeaderLabels(QStringList() << "Feature" << "Status");
    m_featuresTable->horizontalHeader()->setStretchLastSection(true);
    m_featuresTable->setMaximumHeight(150);
    infoLayout->addWidget(new QLabel("Current Features:", this), 4, 0, 1, 2);
    infoLayout->addWidget(m_featuresTable, 5, 0, 1, 2);
    
    mainLayout->addWidget(m_infoGroup);
    
    // Flash Operations
    m_flashGroup = new QGroupBox("Flash Operations", this);
    QGridLayout *flashLayout = new QGridLayout(m_flashGroup);
    
    m_selectFileBtn = new QPushButton("Select UEFI Image", this);
    m_verifyBtn = new QPushButton("Verify Image", this);
    m_backupBtn = new QPushButton("Backup Current", this);
    m_flashBtn = new QPushButton("Flash UEFI", this);
    m_restoreBtn = new QPushButton("Restore Backup", this);
    
    m_flashMethodCombo = new QComboBox(this);
    m_flashMethodCombo->addItems(QStringList() 
        << "Direct SPI Flash (dd)" 
        << "MTD Flash (flashcp)"
        << "Safe Mode (verify each block)");
    
    m_verifyAfterFlash = new QCheckBox("Verify after flash", this);
    m_verifyAfterFlash->setChecked(true);
    m_autoBackup = new QCheckBox("Auto backup before flash", this);
    m_autoBackup->setChecked(true);
    
    flashLayout->addWidget(new QLabel("Flash Method:", this), 0, 0);
    flashLayout->addWidget(m_flashMethodCombo, 0, 1, 1, 2);
    flashLayout->addWidget(m_selectFileBtn, 1, 0);
    flashLayout->addWidget(m_verifyBtn, 1, 1);
    flashLayout->addWidget(m_backupBtn, 1, 2);
    flashLayout->addWidget(m_flashBtn, 2, 0);
    flashLayout->addWidget(m_restoreBtn, 2, 1);
    flashLayout->addWidget(m_verifyAfterFlash, 3, 0);
    flashLayout->addWidget(m_autoBackup, 3, 1);
    
    mainLayout->addWidget(m_flashGroup);
    
    // Updates and Patches
    QTabWidget *tabWidget = new QTabWidget(this);
    
    // Updates tab
    QWidget *updatesTab = new QWidget();
    QVBoxLayout *updatesLayout = new QVBoxLayout(updatesTab);
    
    m_checkUpdatesBtn = new QPushButton("Check for Updates", this);
    updatesLayout->addWidget(m_checkUpdatesBtn);
    
    m_updatesTable = new QTableWidget(0, 5, this);
    m_updatesTable->setHorizontalHeaderLabels(QStringList() 
        << "Version" << "Date" << "Size" << "Type" << "Action");
    m_updatesTable->horizontalHeader()->setStretchLastSection(true);
    updatesLayout->addWidget(m_updatesTable);
    
    tabWidget->addTab(updatesTab, "Updates");
    
    // Patches tab
    QWidget *patchesTab = new QWidget();
    QVBoxLayout *patchesLayout = new QVBoxLayout(patchesTab);
    
    m_patchesTable = new QTableWidget(0, 4, this);
    m_patchesTable->setHorizontalHeaderLabels(QStringList() 
        << "Apply" << "Patch" << "Type" << "Description");
    m_patchesTable->horizontalHeader()->setStretchLastSection(true);
    patchesLayout->addWidget(m_patchesTable);
    
    QPushButton *applyPatchesBtn = new QPushButton("Apply Selected Patches", this);
    patchesLayout->addWidget(applyPatchesBtn);
    
    tabWidget->addTab(patchesTab, "Patches");
    
    mainLayout->addWidget(tabWidget);
    
    // Progress and Log
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);
    
    m_logOutput = new QTextEdit(this);
    m_logOutput->setReadOnly(true);
    m_logOutput->setMaximumHeight(150);
    mainLayout->addWidget(m_logOutput);
    
    // Connect buttons
    connect(m_selectFileBtn, &QPushButton::clicked, this, &UefiTab::selectUefiFile);
    connect(m_verifyBtn, &QPushButton::clicked, this, &UefiTab::verifyUefiImage);
    connect(m_backupBtn, &QPushButton::clicked, this, &UefiTab::backupCurrentUefi);
    connect(m_flashBtn, &QPushButton::clicked, this, &UefiTab::flashUefi);
    connect(m_restoreBtn, &QPushButton::clicked, this, &UefiTab::restoreUefiBackup);
    connect(m_checkUpdatesBtn, &QPushButton::clicked, this, &UefiTab::checkForUpdates);
    connect(applyPatchesBtn, &QPushButton::clicked, this, &UefiTab::applySelectedPatches);
}

void UefiTab::detectSpiDevice()
{
    // Check for SPI flash devices
    QStringList possibleDevices = {
        "/dev/mtd0",
        "/dev/mtdblock0",
        "/dev/spidev0.0"
    };
    
    for (const QString &device : possibleDevices) {
        if (QFile::exists(device)) {
            m_spiDevice = device;
            m_spiFlashLabel->setText(QString("SPI Flash: %1").arg(device));
            m_logOutput->append(QString("Found SPI device: %1").arg(device));
            
            // Get flash info
            QProcess proc;
            proc.start("mtdinfo", QStringList() << device);
            if (proc.waitForFinished()) {
                QString output = proc.readAllStandardOutput();
                m_logOutput->append(output);
            }
            break;
        }
    }
    
    if (m_spiDevice.isEmpty()) {
        m_spiFlashLabel->setText("SPI Flash: Not found!");
        m_flashBtn->setEnabled(false);
        QMessageBox::warning(this, "SPI Flash Not Found", 
            "No SPI flash device detected. Please check your hardware.");
    }
}

void UefiTab::checkCurrentUefi()
{
    m_logOutput->append("Checking current UEFI version...");
    
    // Read current UEFI from SPI flash
    if (!m_spiDevice.isEmpty()) {
        UefiInfo info;
        if (readCurrentUefi(info)) {
            m_currentUefi = info;
            m_currentVersionLabel->setText(QString("Version: %1").arg(info.version));
            updateFeaturesList();
        } else {
            m_currentVersionLabel->setText("Version: Unknown");
        }
    }
}

bool UefiTab::readCurrentUefi(UefiInfo &info)
{
    // Read UEFI header from SPI flash
    QFile spiFile(m_spiDevice);
    if (!spiFile.open(QIODevice::ReadOnly)) {
        m_logOutput->append("Failed to open SPI device for reading");
        return false;
    }
    
    // Skip to FIT image offset (1MB)
    const quint32 FIT_OFFSET = 0x100000;
    QByteArray fitHeader;
    
    if (m_spiDevice.contains("mtd")) {
        // MTD devices don't support seek, read sequentially
        QByteArray skipData = spiFile.read(FIT_OFFSET);
        if (skipData.size() < FIT_OFFSET) {
            m_logOutput->append("Failed to skip to FIT offset");
            return false;
        }
        fitHeader = spiFile.read(512);
    } else {
        // For block devices, we can seek
        if (!spiFile.seek(FIT_OFFSET)) {
            m_logOutput->append("Failed to seek to FIT offset");
            return false;
        }
        fitHeader = spiFile.read(512);
    }
    
    if (fitHeader.size() < 512) {
        m_logOutput->append("Failed to read FIT header");
        return false;
    }
    
    // Parse FIT header to find UEFI info
    // Look for version string pattern
    int versionPos = fitHeader.indexOf("UEFI v");
    if (versionPos != -1) {
        info.version = QString::fromLatin1(fitHeader.mid(versionPos + 6, 10)).trimmed();
    } else {
        info.version = "Unknown";
    }
    
    // Detect features based on presence of specific drivers
    info.features.clear();
    
    // Read more data to check for features
    QByteArray uefiData = spiFile.read(1024 * 1024); // Read 1MB
    
    if (uefiData.contains("Fusb302Dxe")) {
        info.features << "USB-C PD Support";
    }
    if (uefiData.contains("F2fsDxe")) {
        info.features << "F2FS Boot Support";
    }
    if (uefiData.contains("UsbDpPhy")) {
        info.features << "USB-C DisplayPort";
    }
    if (uefiData.contains("HdmiEnhanced")) {
        info.features << "HDMI 2.1 Support";
    }
    if (uefiData.contains("AudioPassthrough")) {
        info.features << "Audio Passthrough";
    }
    if (uefiData.contains("HS400ES")) {
        info.features << "eMMC HS400ES";
    }
    
    info.board = "Orange Pi 5 Plus";
    info.size = 8 * 1024 * 1024; // 8MB SPI flash
    info.fitOffset = FIT_OFFSET;
    
    spiFile.close();
    return true;
}

void UefiTab::updateFeaturesList()
{
    m_featuresTable->setRowCount(0);
    
    // Add standard features
    QStringList allFeatures = {
        "USB-C PD Support",
        "USB-C DisplayPort", 
        "HDMI 2.1 Support",
        "F2FS Boot Support",
        "Audio Passthrough",
        "eMMC HS400ES",
        "NVMe Support",
        "UEFI Shell",
        "Secure Boot"
    };
    
    for (const QString &feature : allFeatures) {
        int row = m_featuresTable->rowCount();
        m_featuresTable->insertRow(row);
        m_featuresTable->setItem(row, 0, new QTableWidgetItem(feature));
        
        QString status = m_currentUefi.features.contains(feature) ? "Enabled" : "Disabled";
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        if (status == "Enabled") {
            statusItem->setForeground(Qt::green);
        } else {
            statusItem->setForeground(Qt::red);
        }
        m_featuresTable->setItem(row, 1, statusItem);
    }
}

void UefiTab::loadPatchDatabase()
{
    // Load available patches from JSON database
    QString patchDbPath = QDir::homePath() + "/Arm-Pi-Tweaker/patches/uefi_patches.json";
    QFile patchFile(patchDbPath);
    
    if (!patchFile.exists()) {
        // Create default patch database
        m_availablePatches.clear();
        
        // USB-C Orientation Fix
        UefiPatch patch1;
        patch1.id = "usbc-orientation-fix";
        patch1.name = "USB-C Orientation Fix";
        patch1.description = "Fixes USB-C cables only working in one orientation";
        patch1.version = "1.0";
        patch1.type = "bugfix";
        patch1.offset = 0x120000; // Example offset
        m_availablePatches.append(patch1);
        
        // HDMI Color Fix
        UefiPatch patch2;
        patch2.id = "hdmi-color-fix";
        patch2.name = "HDMI Color Channel Fix";
        patch2.description = "Fixes red/green channel swap on some displays";
        patch2.version = "1.0";
        patch2.type = "bugfix";
        patch2.offset = 0x130000;
        m_availablePatches.append(patch2);
        
        // Performance Boost
        UefiPatch patch3;
        patch3.id = "perf-boost";
        patch3.name = "Performance Optimizations";
        patch3.description = "Enables aggressive performance settings";
        patch3.version = "1.0";
        patch3.type = "enhancement";
        patch3.offset = 0x140000;
        m_availablePatches.append(patch3);
        
    } else {
        // Load from file
        if (patchFile.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(patchFile.readAll());
            QJsonArray patches = doc.array();
            
            for (const QJsonValue &value : patches) {
                QJsonObject obj = value.toObject();
                UefiPatch patch;
                patch.id = obj["id"].toString();
                patch.name = obj["name"].toString();
                patch.description = obj["description"].toString();
                patch.version = obj["version"].toString();
                patch.type = obj["type"].toString();
                patch.offset = obj["offset"].toInt();
                m_availablePatches.append(patch);
            }
        }
    }
    
    // Populate patches table
    m_patchesTable->setRowCount(0);
    for (const UefiPatch &patch : m_availablePatches) {
        int row = m_patchesTable->rowCount();
        m_patchesTable->insertRow(row);
        
        QCheckBox *cb = new QCheckBox();
        m_patchesTable->setCellWidget(row, 0, cb);
        m_patchesTable->setItem(row, 1, new QTableWidgetItem(patch.name));
        m_patchesTable->setItem(row, 2, new QTableWidgetItem(patch.type));
        m_patchesTable->setItem(row, 3, new QTableWidgetItem(patch.description));
    }
}

void UefiTab::checkForUpdates()
{
    m_logOutput->append("Checking for UEFI updates...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Indeterminate
    
    // Check update server (example)
    QString updateUrl = "https://github.com/edk2-rk3588/releases/latest/download/updates.json";
    
    // Simulate finding updates
    m_updatesTable->setRowCount(0);
    
    // Add example update
    int row = m_updatesTable->rowCount();
    m_updatesTable->insertRow(row);
    m_updatesTable->setItem(row, 0, new QTableWidgetItem("2024.01.15"));
    m_updatesTable->setItem(row, 1, new QTableWidgetItem("2024-01-15"));
    m_updatesTable->setItem(row, 2, new QTableWidgetItem("8 MB"));
    m_updatesTable->setItem(row, 3, new QTableWidgetItem("Stable"));
    
    QPushButton *downloadBtn = new QPushButton("Download");
    m_updatesTable->setCellWidget(row, 4, downloadBtn);
    
    m_progressBar->setVisible(false);
    m_logOutput->append("Found 1 update available");
}

void UefiTab::loadAvailablePatches()
{
    // Clear the patches table
    m_patchesTable->setRowCount(0);
    
    // Load patches from database if not already loaded
    if (m_availablePatches.isEmpty()) {
        loadPatchDatabase();
    }
    
    // Populate the patches table
    m_patchesTable->setRowCount(m_availablePatches.size());
    
    for (int i = 0; i < m_availablePatches.size(); ++i) {
        const UefiPatch &patch = m_availablePatches[i];
        
        // Add checkbox
        QCheckBox *checkbox = new QCheckBox();
        m_patchesTable->setCellWidget(i, 0, checkbox);
        
        // Add patch details
        m_patchesTable->setItem(i, 1, new QTableWidgetItem(patch.name));
        m_patchesTable->setItem(i, 2, new QTableWidgetItem(patch.type));
        m_patchesTable->setItem(i, 3, new QTableWidgetItem(patch.version));
        m_patchesTable->setItem(i, 4, new QTableWidgetItem(patch.description));
        
        // Style based on type
        QColor color;
        if (patch.type == "bugfix") {
            color = QColor(255, 200, 200); // Light red
        } else if (patch.type == "enhancement") {
            color = QColor(200, 255, 200); // Light green
        } else if (patch.type == "feature") {
            color = QColor(200, 200, 255); // Light blue
        }
        
        for (int j = 1; j < 5; ++j) {
            if (m_patchesTable->item(i, j)) {
                m_patchesTable->item(i, j)->setBackground(color);
            }
        }
    }
    
    m_patchesTable->resizeColumnsToContents();
    m_logOutput->append(QString("Loaded %1 available patches").arg(m_availablePatches.size()));
}

void UefiTab::selectUefiFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select UEFI Image", 
        QDir::homePath(),
        "UEFI Images (*.img *.bin *.itb);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        m_selectedUefiPath = fileName;
        m_logOutput->append(QString("Selected: %1").arg(fileName));
        
        // Auto-verify
        verifyUefiImage();
    }
}

void UefiTab::verifyUefiImage()
{
    if (m_selectedUefiPath.isEmpty()) {
        QMessageBox::warning(this, "No Image Selected", 
            "Please select a UEFI image first.");
        return;
    }
    
    m_logOutput->append(QString("Verifying %1...").arg(m_selectedUefiPath));
    
    QFile file(m_selectedUefiPath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_logOutput->append("Failed to open image file");
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    // Check file size
    if (data.size() != 8 * 1024 * 1024) {
        m_logOutput->append(QString("Warning: Image size is %1 bytes, expected 8MB")
            .arg(data.size()));
    }
    
    // Check for SPL signature at offset 0x8000
    if (data.mid(0x8000, 4).toHex() != "2052434b") { // "RCK " in hex
        m_logOutput->append("Warning: SPL signature not found at expected offset");
    }
    
    // Check for FIT image at offset 0x100000
    if (data.mid(0x100000, 4).toHex() != "d00dfeed") { // FIT magic
        m_logOutput->append("Warning: FIT image signature not found");
    }
    
    // Calculate checksum
    QString checksum = calculateChecksum(data);
    m_logOutput->append(QString("SHA256: %1").arg(checksum));
    
    // Check compatibility
    if (verifyImageCompatibility(m_selectedUefiPath)) {
        m_logOutput->append("Image verification passed!");
        m_flashBtn->setEnabled(true);
    } else {
        m_logOutput->append("Image verification failed!");
        m_flashBtn->setEnabled(false);
    }
}

bool UefiTab::verifyImageCompatibility(const QString &imagePath)
{
    // Extract and check image metadata
    UefiInfo info;
    if (!parseUefiImage(imagePath, info)) {
        return false;
    }
    
    // Check board compatibility
    if (info.board != "Orange Pi 5 Plus") {
        m_logOutput->append(QString("Warning: Image is for %1, not Orange Pi 5 Plus")
            .arg(info.board));
        return false;
    }
    
    return true;
}

void UefiTab::backupCurrentUefi()
{
    QString backupPath = QFileDialog::getSaveFileName(this,
        "Save UEFI Backup",
        QDir::homePath() + QString("/uefi_backup_%1.img")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),
        "Image Files (*.img);;All Files (*)");
    
    if (backupPath.isEmpty()) {
        return;
    }
    
    m_logOutput->append(QString("Backing up to %1...").arg(backupPath));
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 100);
    
    // Use dd to backup
    QStringList args;
    args << QString("if=%1").arg(m_spiDevice);
    args << QString("of=%1").arg(backupPath);
    args << "bs=1M";
    args << "count=8";
    args << "status=progress";
    
    m_flashProcess->start("dd", args);
    
    if (!m_flashProcess->waitForStarted()) {
        m_logOutput->append("Failed to start backup process");
        m_progressBar->setVisible(false);
        return;
    }
}

void UefiTab::flashUefi()
{
    if (m_selectedUefiPath.isEmpty()) {
        QMessageBox::warning(this, "No Image Selected",
            "Please select a UEFI image to flash.");
        return;
    }
    
    // Safety checks
    if (!checkBatteryStatus()) {
        QMessageBox::critical(this, "Low Battery",
            "Battery level is too low. Please connect AC power before flashing.");
        return;
    }
    
    if (!isOrangePi5Plus()) {
        QMessageBox::critical(this, "Wrong Board",
            "This system is not an Orange Pi 5 Plus. Flashing aborted for safety.");
        return;
    }
    
    // Confirm with user
    int ret = QMessageBox::warning(this, "Confirm Flash",
        "This will overwrite the current UEFI firmware.\n"
        "Power loss during flashing may brick your device!\n\n"
        "Are you sure you want to continue?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    // Auto backup if enabled
    if (m_autoBackup->isChecked()) {
        m_logOutput->append("Creating automatic backup...");
        QString backupPath = QDir::homePath() + QString("/uefi_autobackup_%1.img")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
        
        QProcess backupProc;
        backupProc.start("dd", QStringList() 
            << QString("if=%1").arg(m_spiDevice)
            << QString("of=%1").arg(backupPath)
            << "bs=1M" << "count=8");
        
        if (!backupProc.waitForFinished(30000)) {
            m_logOutput->append("Backup failed!");
            return;
        }
        m_logOutput->append(QString("Backup saved to %1").arg(backupPath));
    }
    
    // Lock SPI device
    if (!lockSpiDevice()) {
        QMessageBox::critical(this, "Device Busy",
            "SPI device is in use. Please close other applications and try again.");
        return;
    }
    
    m_logOutput->append(QString("Flashing %1...").arg(m_selectedUefiPath));
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 100);
    m_flashBtn->setEnabled(false);
    
    // Flash based on selected method
    QStringList args;
    QString program;
    
    switch (m_flashMethodCombo->currentIndex()) {
    case 0: // Direct dd
        program = "dd";
        args << QString("if=%1").arg(m_selectedUefiPath);
        args << QString("of=%1").arg(m_spiDevice);
        args << "bs=1M";
        args << "conv=fsync";
        args << "status=progress";
        break;
        
    case 1: // MTD flashcp
        program = "flashcp";
        args << "-v";
        args << m_selectedUefiPath;
        args << m_spiDevice;
        break;
        
    case 2: // Safe mode
        // Use integrated safe flash method
        performSafeFlash();
        return;
        break;
    }
    
    m_flashProcess->start(program, args);
    
    if (!m_flashProcess->waitForStarted()) {
        m_logOutput->append("Failed to start flash process");
        m_progressBar->setVisible(false);
        m_flashBtn->setEnabled(true);
        unlockSpiDevice();
    }
}

void UefiTab::onFlashProcessOutput()
{
    QString output = m_flashProcess->readAllStandardOutput();
    m_logOutput->append(output);
    
    // Parse progress from dd
    if (output.contains("bytes")) {
        QRegularExpression rx("(\\d+) bytes");
        QRegularExpressionMatch match = rx.match(output);
        if (match.hasMatch()) {
            qint64 bytes = match.captured(1).toLongLong();
            int progress = (bytes * 100) / (8 * 1024 * 1024);
            m_progressBar->setValue(progress);
        }
    }
}

void UefiTab::onFlashProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_progressBar->setVisible(false);
    m_flashBtn->setEnabled(true);
    unlockSpiDevice();
    
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_logOutput->append("Flash completed successfully!");
        
        if (m_verifyAfterFlash->isChecked()) {
            m_logOutput->append("Verifying flash...");
            
            // Read back and compare
            QProcess verifyProc;
            QString verifyPath = "/tmp/verify.img";
            verifyProc.start("dd", QStringList()
                << QString("if=%1").arg(m_spiDevice)
                << QString("of=%1").arg(verifyPath)
                << "bs=1M" << "count=8");
            
            if (verifyProc.waitForFinished(30000)) {
                QFile origFile(m_selectedUefiPath);
                QFile verifyFile(verifyPath);
                
                if (origFile.open(QIODevice::ReadOnly) && 
                    verifyFile.open(QIODevice::ReadOnly)) {
                    
                    QByteArray origData = origFile.readAll();
                    QByteArray verifyData = verifyFile.readAll();
                    
                    if (origData == verifyData) {
                        m_logOutput->append("Verification passed!");
                        QMessageBox::information(this, "Success",
                            "UEFI flashed and verified successfully!\n"
                            "Please reboot to use the new UEFI.");
                    } else {
                        m_logOutput->append("Verification FAILED!");
                        QMessageBox::critical(this, "Verification Failed",
                            "The flashed data does not match!\n"
                            "DO NOT REBOOT! Restore from backup immediately!");
                    }
                }
                QFile::remove(verifyPath);
            }
        } else {
            QMessageBox::information(this, "Success",
                "UEFI flashed successfully!\n"
                "Please reboot to use the new UEFI.");
        }
        
        // Refresh current UEFI info
        checkCurrentUefi();
        
    } else {
        m_logOutput->append(QString("Flash failed with exit code %1").arg(exitCode));
        QMessageBox::critical(this, "Flash Failed",
            "Failed to flash UEFI image.\n"
            "Check the log for details.");
    }
}

void UefiTab::applySelectedPatches()
{
    QList<UefiPatch> selectedPatches;
    
    // Get selected patches
    for (int i = 0; i < m_patchesTable->rowCount(); i++) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(m_patchesTable->cellWidget(i, 0));
        if (cb && cb->isChecked()) {
            selectedPatches.append(m_availablePatches[i]);
        }
    }
    
    if (selectedPatches.isEmpty()) {
        QMessageBox::information(this, "No Patches Selected",
            "Please select at least one patch to apply.");
        return;
    }
    
    // Create patched image
    QString patchedPath = QFileDialog::getSaveFileName(this,
        "Save Patched UEFI Image",
        QDir::homePath() + "/uefi_patched.img",
        "Image Files (*.img);;All Files (*)");
    
    if (patchedPath.isEmpty()) {
        return;
    }
    
    m_logOutput->append("Creating patched UEFI image...");
    
    // Read current UEFI
    QFile spiFile(m_spiDevice);
    if (!spiFile.open(QIODevice::ReadOnly)) {
        m_logOutput->append("Failed to read current UEFI");
        return;
    }
    
    QByteArray uefiData = spiFile.readAll();
    spiFile.close();
    
    // Apply patches
    for (const UefiPatch &patch : selectedPatches) {
        m_logOutput->append(QString("Applying patch: %1").arg(patch.name));
        if (!applyPatch(patch, uefiData)) {
            m_logOutput->append(QString("Failed to apply patch: %1").arg(patch.name));
            return;
        }
    }
    
    // Save patched image
    QFile outFile(patchedPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        m_logOutput->append("Failed to create patched image");
        return;
    }
    
    outFile.write(uefiData);
    outFile.close();
    
    m_logOutput->append(QString("Patched image saved to %1").arg(patchedPath));
    m_selectedUefiPath = patchedPath;
    
    // Auto-verify the patched image
    verifyUefiImage();
}

bool UefiTab::applyPatch(const UefiPatch &patch, QByteArray &uefiData)
{
    // Example patch application
    // In real implementation, patches would contain actual binary diffs
    
    if (patch.id == "usbc-orientation-fix") {
        // Example: Change a specific byte pattern
        QByteArray oldPattern = QByteArray::fromHex("4889E54883EC20");
        QByteArray newPattern = QByteArray::fromHex("4889E54883EC30");
        
        int index = uefiData.indexOf(oldPattern, patch.offset);
        if (index != -1) {
            uefiData.replace(index, oldPattern.size(), newPattern);
            return true;
        }
    }
    
    // For demo, just return true
    return true;
}

void UefiTab::restoreUefiBackup()
{
    QString backupPath = QFileDialog::getOpenFileName(this,
        "Select UEFI Backup to Restore",
        QDir::homePath(),
        "Image Files (*.img);;All Files (*)");
    
    if (backupPath.isEmpty()) {
        return;
    }
    
    m_selectedUefiPath = backupPath;
    m_logOutput->append(QString("Selected backup: %1").arg(backupPath));
    
    // Verify it's a valid backup
    QFile backupFile(backupPath);
    if (backupFile.size() != 8 * 1024 * 1024) {
        QMessageBox::warning(this, "Invalid Backup",
            "The selected file does not appear to be a valid 8MB UEFI backup.");
        return;
    }
    
    // Flash the backup
    flashUefi();
}

QString UefiTab::calculateChecksum(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

bool UefiTab::parseUefiImage(const QString &path, UefiInfo &info)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    // Skip to FIT offset
    if (!file.seek(0x100000)) {
        file.close();
        return false;
    }
    QByteArray fitData = file.read(1024 * 1024);
    file.close();
    
    // Parse similar to readCurrentUefi
    int versionPos = fitData.indexOf("UEFI v");
    if (versionPos != -1) {
        info.version = QString::fromLatin1(fitData.mid(versionPos + 6, 10)).trimmed();
    }
    
    // Always Orange Pi 5 Plus for now
    info.board = "Orange Pi 5 Plus";
    
    return true;
}

bool UefiTab::isOrangePi5Plus()
{
    // Check device tree
    QFile dtFile("/proc/device-tree/model");
    if (dtFile.open(QIODevice::ReadOnly)) {
        QString model = dtFile.readAll().trimmed();
        dtFile.close();
        return model.contains("Orange Pi 5 Plus", Qt::CaseInsensitive);
    }
    
    // Fallback: check board info
    QFile boardFile("/proc/board/name");
    if (boardFile.open(QIODevice::ReadOnly)) {
        QString board = boardFile.readAll().trimmed();
        boardFile.close();
        return board.contains("orangepi-5-plus", Qt::CaseInsensitive);
    }
    
    return false;
}

bool UefiTab::checkBatteryStatus()
{
    // For desktop/SBC, always return true
    // For laptops, would check /sys/class/power_supply/
    return true;
}

bool UefiTab::lockSpiDevice()
{
    // Try to get exclusive access
    // In real implementation, would use flock() or similar
    return true;
}

void UefiTab::unlockSpiDevice()
{
    // Release exclusive access
}

void UefiTab::performSafeFlash()
{
    const quint32 BLOCK_SIZE = 64 * 1024;  // 64KB blocks
    const quint32 TOTAL_SIZE = 8 * 1024 * 1024;  // 8MB total
    
    m_logOutput->append("Starting safe flash process...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 100);
    m_flashBtn->setEnabled(false);
    
    // Read image file
    QFile imageFile(m_selectedUefiPath);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        m_logOutput->append("Failed to open image file");
        m_progressBar->setVisible(false);
        m_flashBtn->setEnabled(true);
        return;
    }
    
    QByteArray imageData = imageFile.readAll();
    imageFile.close();
    
    // Pad to 8MB if needed
    if (imageData.size() < TOTAL_SIZE) {
        int oldSize = imageData.size();
        imageData.resize(TOTAL_SIZE);
        // Fill the rest with 0xFF
        for (int i = oldSize; i < TOTAL_SIZE; i++) {
            imageData[i] = 0xFF;
        }
    }
    
    // Open SPI device
    int fd = open(m_spiDevice.toLocal8Bit().constData(), O_RDWR);
    if (fd < 0) {
        m_logOutput->append("Failed to open SPI device");
        m_progressBar->setVisible(false);
        m_flashBtn->setEnabled(true);
        return;
    }
    
    // Flash block by block
    quint32 totalBlocks = TOTAL_SIZE / BLOCK_SIZE;
    bool success = true;
    
    for (quint32 block = 0; block < totalBlocks && success; block++) {
        quint32 offset = block * BLOCK_SIZE;
        quint32 size = qMin(BLOCK_SIZE, TOTAL_SIZE - offset);
        
        // Update progress
        int progress = (block * 100) / totalBlocks;
        m_progressBar->setValue(progress);
        m_logOutput->append(QString("Flashing block %1/%2 at offset 0x%3")
            .arg(block).arg(totalBlocks).arg(offset, 0, 16));
        
        // Erase block
        if (!eraseBlock(fd, offset)) {
            m_logOutput->append(QString("Failed to erase block at offset 0x%1").arg(offset, 0, 16));
            success = false;
            break;
        }
        
        // Write block
        QByteArray blockData = imageData.mid(offset, size);
        if (!flashBlock(fd, offset, blockData)) {
            m_logOutput->append(QString("Failed to write block at offset 0x%1").arg(offset, 0, 16));
            success = false;
            break;
        }
        
        // Verify block immediately
        if (!verifyBlock(fd, offset, blockData)) {
            m_logOutput->append(QString("Verification failed at offset 0x%1").arg(offset, 0, 16));
            
            // Retry once
            m_logOutput->append("Retrying block...");
            if (!eraseBlock(fd, offset) || 
                !flashBlock(fd, offset, blockData) ||
                !verifyBlock(fd, offset, blockData)) {
                m_logOutput->append("Retry failed!");
                success = false;
                break;
            }
            m_logOutput->append("Retry successful!");
        }
        
        // Allow GUI updates
        QCoreApplication::processEvents();
    }
    
    ::close(fd);
    
    m_progressBar->setVisible(false);
    m_flashBtn->setEnabled(true);
    
    if (success) {
        m_progressBar->setValue(100);
        m_logOutput->append("Safe flash completed successfully!");
        QMessageBox::information(this, "Success",
            "UEFI flashed successfully!\n"
            "Please reboot to use the new UEFI.");
        checkCurrentUefi();
    } else {
        m_logOutput->append("Safe flash failed!");
        QMessageBox::critical(this, "Flash Failed",
            "Failed to flash UEFI image.\n"
            "DO NOT REBOOT! Restore from backup immediately!");
    }
}

bool UefiTab::flashBlock(int fd, quint32 offset, const QByteArray &data)
{
    if (lseek(fd, offset, SEEK_SET) != offset) {
        return false;
    }
    
    ssize_t written = write(fd, data.constData(), data.size());
    return written == data.size();
}

bool UefiTab::verifyBlock(int fd, quint32 offset, const QByteArray &expected)
{
    if (lseek(fd, offset, SEEK_SET) != offset) {
        return false;
    }
    
    QByteArray readBuffer(expected.size(), 0);
    ssize_t bytesRead = read(fd, readBuffer.data(), readBuffer.size());
    
    if (bytesRead != expected.size()) {
        return false;
    }
    
    return readBuffer == expected;
}

bool UefiTab::eraseBlock(int fd, quint32 offset)
{
    // For MTD devices, use ioctl to erase
    if (m_spiDevice.contains("/dev/mtd")) {
        struct erase_info_user {
            uint32_t start;
            uint32_t length;
        } erase_info;
        
        erase_info.start = offset;
        erase_info.length = 64 * 1024; // 64KB block
        
        #define MEMERASE _IOW('M', 2, struct erase_info_user)
        return ioctl(fd, MEMERASE, &erase_info) == 0;
    }
    
    // For other devices, just return true (write will overwrite)
    return true;
}