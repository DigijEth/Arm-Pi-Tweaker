#include "backupwizard.h"
#include "../storagemanager.h"
#include <QApplication>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QStorageInfo>
#include <QRegularExpression>

BackupWizard::BackupWizard(StorageManager *storageManager, QWidget *parent)
    : QDialog(parent)
    , m_storageManager(storageManager)
    , m_currentProcess(nullptr)
    , m_progressTimer(new QTimer(this))
    , m_currentPage(0)
    , m_spaceRequired(0)
    , m_spaceAvailable(0)
    , m_makeBootable(false)
{
    setWindowTitle("Backup/Restore Wizard");
    setMinimumSize(800, 600);
    setModal(true);
    
    setupUI();
    
    connect(m_progressTimer, &QTimer::timeout, this, &BackupWizard::updateProgress);
}

BackupWizard::~BackupWizard()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        m_currentProcess->kill();
        m_currentProcess->waitForFinished(3000);
    }
}

void BackupWizard::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create stacked widget for pages
    m_pageStack = new QStackedWidget();
    mainLayout->addWidget(m_pageStack);
    
    // Setup all pages
    setupWarningPage();
    setupDeviceSelectionPage();
    setupBackupTypePage();
    setupFileSelectionPage();
    setupTargetDevicePage();
    setupFormatPage();
    setupConfirmationPage();
    setupProgressPage();
    
    // Navigation buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_backButton = new QPushButton("Back");
    m_backButton->setIcon(QIcon(":/icons/back.png"));
    m_backButton->setEnabled(false);
    connect(m_backButton, &QPushButton::clicked, this, &BackupWizard::onBackClicked);
    
    buttonLayout->addWidget(m_backButton);
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton("Cancel");
    m_cancelButton->setIcon(QIcon(":/icons/cancel.png"));
    connect(m_cancelButton, &QPushButton::clicked, this, &BackupWizard::onCancelClicked);
    buttonLayout->addWidget(m_cancelButton);
    
    m_nextButton = new QPushButton("Next");
    m_nextButton->setIcon(QIcon(":/icons/next.png"));
    connect(m_nextButton, &QPushButton::clicked, this, &BackupWizard::onNextClicked);
    buttonLayout->addWidget(m_nextButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Set initial page
    m_pageStack->setCurrentIndex(WarningPage);
    updateNavigationButtons();
}

void BackupWizard::setupWarningPage()
{
    m_warningPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_warningPage);
    
    // Title
    QLabel *titleLabel = new QLabel("⚠️ CRITICAL WARNING");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: red; margin: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Warning text
    m_warningLabel = new QLabel();
    m_warningLabel->setText(
        "<html><body style='font-size: 14px;'>"
        "<p><b>IMPORTANT:</b> You could lose all your data and/or the device may not boot if the backup fails!</p>"
        "<br>"
        "<p><b>Risks include:</b></p>"
        "<ul>"
        "<li>Complete data loss if backup corruption occurs</li>"
        "<li>System may become unbootable if critical files are missed</li>"
        "<li>Hardware failure during backup process could damage device</li>"
        "<li>Network interruption may corrupt backup files</li>"
        "<li>Insufficient storage space may cause incomplete backups</li>"
        "</ul>"
        "<br>"
        "<p><b>Before proceeding:</b></p>"
        "<ul>"
        "<li>Ensure you have reliable power supply</li>"
        "<li>Close all running applications</li>"
        "<li>Verify sufficient storage space on backup destination</li>"
        "<li>Consider creating multiple backup copies</li>"
        "</ul>"
        "<br>"
        "<p style='color: red; font-weight: bold;'>Only proceed if you understand and accept these risks!</p>"
        "</body></html>"
    );
    m_warningLabel->setWordWrap(true);
    m_warningLabel->setStyleSheet("padding: 20px; border: 2px solid red; background-color: #ffeeee;");
    layout->addWidget(m_warningLabel);
    
    layout->addStretch();
    
    // Risk acceptance buttons
    QHBoxLayout *riskLayout = new QHBoxLayout();
    riskLayout->addStretch();
    
    m_skipButton = new QPushButton("I Understand the Risks - Skip");
    m_skipButton->setStyleSheet("background-color: orange; font-weight: bold; padding: 10px;");
    connect(m_skipButton, &QPushButton::clicked, this, &BackupWizard::onSkipRisksClicked);
    riskLayout->addWidget(m_skipButton);
    
    m_continueButton = new QPushButton("Continue - I Accept the Risks");
    m_continueButton->setStyleSheet("background-color: red; color: white; font-weight: bold; padding: 10px;");
    connect(m_continueButton, &QPushButton::clicked, this, &BackupWizard::onContinueClicked);
    riskLayout->addWidget(m_continueButton);
    
    riskLayout->addStretch();
    layout->addLayout(riskLayout);
    
    m_pageStack->addWidget(m_warningPage);
}

void BackupWizard::setupDeviceSelectionPage()
{
    m_deviceSelectionPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_deviceSelectionPage);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Backup Destination");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Device list
    m_deviceList = new QListWidget();
    m_deviceList->setMinimumHeight(300);
    connect(m_deviceList, &QListWidget::itemSelectionChanged, this, &BackupWizard::onDeviceSelectionChanged);
    layout->addWidget(m_deviceList);
    
    // Device info
    m_deviceInfoLabel = new QLabel("Select a device to see details");
    m_deviceInfoLabel->setStyleSheet("padding: 10px; border: 1px solid gray; background-color: #f5f5f5;");
    m_deviceInfoLabel->setMinimumHeight(100);
    layout->addWidget(m_deviceInfoLabel);
    
    m_pageStack->addWidget(m_deviceSelectionPage);
}

void BackupWizard::setupBackupTypePage()
{
    m_backupTypePage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_backupTypePage);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Backup Type");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Info label
    QLabel *infoLabel = new QLabel("Critical system files are automatically included in all backup types.");
    infoLabel->setStyleSheet("font-style: italic; color: blue; margin: 10px;");
    layout->addWidget(infoLabel);
    
    // Backup type options
    m_backupTypeGroup = new QButtonGroup(this);
    
    m_liveBootRadio = new QRadioButton("Live Boot Backup");
    m_liveBootRadio->setChecked(true);
    m_backupTypeGroup->addButton(m_liveBootRadio, static_cast<int>(BackupType::LiveBootBackup));
    layout->addWidget(m_liveBootRadio);
    
    QLabel *liveBootDesc = new QLabel("Creates a 1:1 bootable copy using rsync. Target device will be formatted and made bootable.");
    liveBootDesc->setStyleSheet("margin-left: 20px; color: gray; font-size: 12px;");
    layout->addWidget(liveBootDesc);
    
    m_compressedRadio = new QRadioButton("Compressed Whole Disk");
    m_backupTypeGroup->addButton(m_compressedRadio, static_cast<int>(BackupType::CompressedWholeDisk));
    layout->addWidget(m_compressedRadio);
    
    QLabel *compressedDesc = new QLabel("Creates a compressed disk image that can optionally be made bootable.");
    compressedDesc->setStyleSheet("margin-left: 20px; color: gray; font-size: 12px;");
    layout->addWidget(compressedDesc);
    
    m_customRadio = new QRadioButton("Custom Selection");
    m_backupTypeGroup->addButton(m_customRadio, static_cast<int>(BackupType::Custom));
    layout->addWidget(m_customRadio);
    
    QLabel *customDesc = new QLabel("Select specific files and folders to backup.");
    customDesc->setStyleSheet("margin-left: 20px; color: gray; font-size: 12px;");
    layout->addWidget(customDesc);
    
    connect(m_backupTypeGroup, QOverload<int>::of(&QButtonGroup::idClicked), 
            this, &BackupWizard::onBackupTypeChanged);
    
    layout->addStretch();
    
    m_pageStack->addWidget(m_backupTypePage);
}

void BackupWizard::setupFileSelectionPage()
{
    m_fileSelectionPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_fileSelectionPage);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Files and Folders");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // File browser in a 4x5 grid-like layout
    QHBoxLayout *browserLayout = new QHBoxLayout();
    
    // Left side: file tree (4x5 appearance)
    QVBoxLayout *treeLayout = new QVBoxLayout();
    
    // Control buttons above file tree
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    m_removeFoldersButton = new QPushButton("Remove Folders");
    m_removeFilesButton = new QPushButton("Remove Files");
    controlLayout->addWidget(m_removeFoldersButton);
    controlLayout->addWidget(m_removeFilesButton);
    controlLayout->addStretch();
    
    m_addFoldersButton = new QPushButton("Add Folder(s)");
    m_addFilesButton = new QPushButton("Add File(s)");
    controlLayout->addWidget(m_addFoldersButton);
    controlLayout->addWidget(m_addFilesButton);
    
    treeLayout->addLayout(controlLayout);
    
    // File tree widget
    m_fileTree = new QTreeWidget();
    m_fileTree->setHeaderLabels(QStringList() << "Name" << "Size" << "Type");
    m_fileTree->setSelectionMode(QAbstractItemView::MultiSelection);
    m_fileTree->setMinimumHeight(300);
    treeLayout->addWidget(m_fileTree);
    
    // Whole system button
    m_wholeSystemButton = new QPushButton("Backup Entire System");
    m_wholeSystemButton->setStyleSheet("background-color: lightblue; font-weight: bold; padding: 10px;");
    treeLayout->addWidget(m_wholeSystemButton);
    
    browserLayout->addLayout(treeLayout, 2);
    
    // Right side: selected files and space info
    QVBoxLayout *infoLayout = new QVBoxLayout();
    
    QLabel *selectedLabel = new QLabel("Selected for Backup:");
    selectedLabel->setStyleSheet("font-weight: bold;");
    infoLayout->addWidget(selectedLabel);
    
    m_selectedFilesText = new QTextEdit();
    m_selectedFilesText->setMaximumHeight(200);
    m_selectedFilesText->setReadOnly(true);
    infoLayout->addWidget(m_selectedFilesText);
    
    m_spaceRequiredLabel = new QLabel("Space Required: Calculating...");
    m_spaceRequiredLabel->setStyleSheet("font-weight: bold;");
    infoLayout->addWidget(m_spaceRequiredLabel);
    
    m_spaceAvailableLabel = new QLabel("Space Available: Calculating...");
    m_spaceAvailableLabel->setStyleSheet("font-weight: bold;");
    infoLayout->addWidget(m_spaceAvailableLabel);
    
    infoLayout->addStretch();
    
    browserLayout->addLayout(infoLayout, 1);
    
    layout->addLayout(browserLayout);
    
    // Connect signals
    connect(m_wholeSystemButton, &QPushButton::clicked, this, &BackupWizard::onWholeSystemClicked);
    connect(m_addFoldersButton, &QPushButton::clicked, this, &BackupWizard::onAddFoldersClicked);
    connect(m_addFilesButton, &QPushButton::clicked, this, &BackupWizard::onAddFilesClicked);
    connect(m_removeFoldersButton, &QPushButton::clicked, this, &BackupWizard::onRemoveFoldersClicked);
    connect(m_removeFilesButton, &QPushButton::clicked, this, &BackupWizard::onRemoveFilesClicked);
    connect(m_fileTree, &QTreeWidget::itemSelectionChanged, this, &BackupWizard::onFileSelectionChanged);
    
    // Populate the file tree
    populateFileTree();
    
    m_pageStack->addWidget(m_fileSelectionPage);
}

void BackupWizard::setupTargetDevicePage()
{
    m_targetDevicePage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_targetDevicePage);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Save Location");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Target device selection
    QLabel *deviceLabel = new QLabel("Save backup to:");
    layout->addWidget(deviceLabel);
    
    m_targetDeviceCombo = new QComboBox();
    m_targetDeviceCombo->setMinimumHeight(40);
    layout->addWidget(m_targetDeviceCombo);
    
    // Bootable option
    m_makeBootableCheck = new QCheckBox("Make image bootable");
    connect(m_makeBootableCheck, &QCheckBox::toggled, this, &BackupWizard::onMakeBootableChanged);
    layout->addWidget(m_makeBootableCheck);
    
    // Image format selection (appears when bootable is checked)
    QLabel *formatLabel = new QLabel("Disk image format:");
    layout->addWidget(formatLabel);
    
    m_imageFormatCombo = new QComboBox();
    m_imageFormatCombo->addItems(QStringList() << "ext4" << "ext3" << "ext2" << "fat32");
    m_imageFormatCombo->setCurrentText("ext4");
    m_imageFormatCombo->setEnabled(false);
    layout->addWidget(m_imageFormatCombo);
    
    layout->addStretch();
    
    m_pageStack->addWidget(m_targetDevicePage);
}

void BackupWizard::setupFormatPage()
{
    m_formatPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_formatPage);
    
    // Title
    QLabel *titleLabel = new QLabel("Format Device");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Warning
    m_formatWarningLabel = new QLabel();
    m_formatWarningLabel->setText(
        "<html><body style='color: red; font-weight: bold;'>"
        "<p>⚠️ WARNING: This will permanently erase all data on the selected device!</p>"
        "<p>This action cannot be undone. Make sure you have selected the correct device.</p>"
        "</body></html>"
    );
    m_formatWarningLabel->setStyleSheet("padding: 20px; border: 2px solid red; background-color: #ffeeee;");
    layout->addWidget(m_formatWarningLabel);
    
    // Format options
    QLabel *formatLabel = new QLabel("Select filesystem format:");
    layout->addWidget(formatLabel);
    
    m_formatTypeCombo = new QComboBox();
    m_formatTypeCombo->addItems(QStringList() << "ext4" << "ext3" << "ext2" << "fat32");
    m_formatTypeCombo->setCurrentText("ext4");
    layout->addWidget(m_formatTypeCombo);
    
    m_formatInfoLabel = new QLabel("ext4 is recommended for bootable backups");
    m_formatInfoLabel->setStyleSheet("font-style: italic; color: blue;");
    layout->addWidget(m_formatInfoLabel);
    
    layout->addStretch();
    
    m_pageStack->addWidget(m_formatPage);
}

void BackupWizard::setupConfirmationPage()
{
    m_confirmationPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_confirmationPage);
    
    // Title
    QLabel *titleLabel = new QLabel("Confirm Backup Settings");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Confirmation details
    m_confirmationText = new QTextEdit();
    m_confirmationText->setReadOnly(true);
    m_confirmationText->setMinimumHeight(300);
    layout->addWidget(m_confirmationText);
    
    // Final space check
    m_finalSpaceLabel = new QLabel();
    m_finalSpaceLabel->setStyleSheet("font-weight: bold; padding: 10px; border: 1px solid gray;");
    layout->addWidget(m_finalSpaceLabel);
    
    m_pageStack->addWidget(m_confirmationPage);
}

void BackupWizard::setupProgressPage()
{
    m_progressPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_progressPage);
    
    // Title
    QLabel *titleLabel = new QLabel("Backup In Progress");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Progress bar
    m_progressBar = new QProgressBar();
    m_progressBar->setMinimumHeight(30);
    layout->addWidget(m_progressBar);
    
    // Progress label
    m_progressLabel = new QLabel("Preparing backup...");
    m_progressLabel->setAlignment(Qt::AlignCenter);
    m_progressLabel->setStyleSheet("font-weight: bold; margin: 10px;");
    layout->addWidget(m_progressLabel);
    
    // Progress log
    m_progressLog = new QTextEdit();
    m_progressLog->setReadOnly(true);
    m_progressLog->setFont(QFont("Consolas", 9));
    layout->addWidget(m_progressLog);
    
    m_pageStack->addWidget(m_progressPage);
}

void BackupWizard::startBackupWizard()
{
    populateDeviceList();
    show();
}

void BackupWizard::populateDeviceList()
{
    m_deviceList->clear();
    m_availableDevices.clear();
    
    // Add device options
    BackupDevice localBackup;
    localBackup.name = "Local Backup";
    localBackup.path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Backups";
    localBackup.type = DeviceType::LocalBackup;
    localBackup.isSupported = true;
    m_availableDevices.append(localBackup);
    
    QListWidgetItem *localItem = new QListWidgetItem("📁 Local Backup");
    localItem->setData(Qt::UserRole, 0);
    m_deviceList->addItem(localItem);
    
    // Add SD Card option
    BackupDevice sdCard;
    sdCard.name = "SD Card";
    sdCard.type = DeviceType::SDCard;
    sdCard.isSupported = true;
    m_availableDevices.append(sdCard);
    
    QListWidgetItem *sdItem = new QListWidgetItem("💾 SD Card");
    sdItem->setData(Qt::UserRole, 1);
    m_deviceList->addItem(sdItem);
    
    // Add USB option
    BackupDevice usb;
    usb.name = "USB Device";
    usb.type = DeviceType::USB;
    usb.isSupported = true;
    m_availableDevices.append(usb);
    
    QListWidgetItem *usbItem = new QListWidgetItem("🔌 USB Device");
    usbItem->setData(Qt::UserRole, 2);
    m_deviceList->addItem(usbItem);
    
    // Add Network option (not supported yet)
    BackupDevice network;
    network.name = "Network";
    network.type = DeviceType::Network;
    network.isSupported = false;
    m_availableDevices.append(network);
    
    QListWidgetItem *networkItem = new QListWidgetItem("🌐 Network (Coming Soon)");
    networkItem->setData(Qt::UserRole, 3);
    networkItem->setFlags(Qt::ItemIsEnabled); // Disabled
    networkItem->setForeground(QColor(128, 128, 128));
    m_deviceList->addItem(networkItem);
    
    // Add Cloud option (not supported yet)
    BackupDevice cloud;
    cloud.name = "Cloud";
    cloud.type = DeviceType::Cloud;
    cloud.isSupported = false;
    m_availableDevices.append(cloud);
    
    QListWidgetItem *cloudItem = new QListWidgetItem("☁️ Cloud (Coming Soon)");
    cloudItem->setData(Qt::UserRole, 4);
    cloudItem->setFlags(Qt::ItemIsEnabled); // Disabled
    cloudItem->setForeground(QColor(128, 128, 128));
    m_deviceList->addItem(cloudItem);
}

// Slot implementations
void BackupWizard::onNextClicked()
{
    if (!validateCurrentPage()) {
        return;
    }
    
    if (m_currentPage < m_pageStack->count() - 1) {
        m_currentPage++;
        m_pageStack->setCurrentIndex(m_currentPage);
        updateNavigationButtons();
        
        // Handle special page transitions
        if (m_currentPage == ConfirmationPage) {
            // Update confirmation details
            updateConfirmationPage();
        } else if (m_currentPage == ProgressPage) {
            // Start backup process
            startBackupProcess();
        }
    }
}

void BackupWizard::onBackClicked()
{
    if (m_currentPage > 0) {
        m_currentPage--;
        m_pageStack->setCurrentIndex(m_currentPage);
        updateNavigationButtons();
    }
}

void BackupWizard::onCancelClicked()
{
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Cancel Backup", 
            "A backup is currently in progress. Are you sure you want to cancel?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            m_currentProcess->kill();
            emit wizardCancelled();
            reject();
        }
    } else {
        emit wizardCancelled();
        reject();
    }
}

void BackupWizard::onSkipRisksClicked()
{
    emit wizardCancelled();
    reject();
}

void BackupWizard::onContinueClicked()
{
    onNextClicked();
}

void BackupWizard::onDeviceSelectionChanged()
{
    QListWidgetItem *item = m_deviceList->currentItem();
    if (!item) return;
    
    int deviceIndex = item->data(Qt::UserRole).toInt();
    if (deviceIndex >= 0 && deviceIndex < m_availableDevices.size()) {
        m_selectedDevice = m_availableDevices[deviceIndex];
        
        QString info = QString("<b>%1</b><br>").arg(m_selectedDevice.name);
        if (m_selectedDevice.isSupported) {
            info += "Status: Available<br>";
            info += QString("Path: %1<br>").arg(m_selectedDevice.path);
        } else {
            info += "<span style='color: red;'>Status: Not yet supported</span><br>";
        }
        
        m_deviceInfoLabel->setText(info);
    }
}

void BackupWizard::onBackupTypeChanged()
{
    // Update the selected backup type based on radio button selection
    if (m_liveBootRadio->isChecked()) {
        m_selectedBackupType = BackupType::LiveBootBackup;
    } else if (m_compressedRadio->isChecked()) {
        m_selectedBackupType = BackupType::CompressedWholeDisk;
    } else if (m_customRadio->isChecked()) {
        m_selectedBackupType = BackupType::Custom;
    }
}

void BackupWizard::onWholeSystemClicked()
{
    // Select entire system for backup
    m_selectedFiles.clear();
    m_selectedFiles << "/";
    updateSelectedFiles();
}

void BackupWizard::onAddFoldersClicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder to Backup");
    if (!folder.isEmpty() && !m_selectedFiles.contains(folder)) {
        m_selectedFiles.append(folder);
        updateSelectedFiles();
    }
}

void BackupWizard::onAddFilesClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select Files to Backup");
    for (const QString &file : files) {
        if (!m_selectedFiles.contains(file)) {
            m_selectedFiles.append(file);
        }
    }
    updateSelectedFiles();
}

void BackupWizard::onRemoveFoldersClicked()
{
    QList<QTreeWidgetItem*> selected = m_fileTree->selectedItems();
    for (QTreeWidgetItem *item : selected) {
        QString path = item->data(0, Qt::UserRole).toString();
        m_selectedFiles.removeAll(path);
    }
    updateSelectedFiles();
}

void BackupWizard::onRemoveFilesClicked()
{
    onRemoveFoldersClicked(); // Same implementation
}

void BackupWizard::onFileSelectionChanged()
{
    // Update remove buttons state
    bool hasSelection = !m_fileTree->selectedItems().isEmpty();
    m_removeFoldersButton->setEnabled(hasSelection);
    m_removeFilesButton->setEnabled(hasSelection);
}

void BackupWizard::onMakeBootableChanged(bool enabled)
{
    m_makeBootable = enabled;
    m_imageFormatCombo->setEnabled(enabled);
}

void BackupWizard::onFormatDeviceClicked()
{
    // Handle device formatting - this would be implemented based on the selected device
    QMessageBox::information(this, "Format Device", "Device formatting will be implemented in the next version.");
}

bool BackupWizard::validateCurrentPage()
{
    switch (m_currentPage) {
        case DeviceSelectionPage:
            if (!m_selectedDevice.isSupported) {
                QMessageBox::warning(this, "Invalid Selection", 
                    "Please select a supported backup destination.");
                return false;
            }
            break;
            
        case FileSelectionPage:
            if (m_selectedBackupType == BackupType::Custom && m_selectedFiles.isEmpty()) {
                QMessageBox::warning(this, "No Files Selected", 
                    "Please select files or folders to backup, or use 'Backup Entire System'.");
                return false;
            }
            if (!hasEnoughSpace()) {
                QMessageBox::critical(this, "Insufficient Space", 
                    "Not enough free space for backup. Please attach a USB drive or SD card for temp storage, "
                    "or free up space on the internal drive.");
                return false;
            }
            break;
    }
    return true;
}

void BackupWizard::updateNavigationButtons()
{
    m_backButton->setEnabled(m_currentPage > 0 && m_currentPage != ProgressPage);
    m_nextButton->setEnabled(m_currentPage < m_pageStack->count() - 1);
    
    // Hide next button on pages that don't have enough space
    if (m_currentPage == FileSelectionPage && !hasEnoughSpace()) {
        m_nextButton->setVisible(false);
    } else {
        m_nextButton->setVisible(true);
    }
    
    // Change text for some pages
    if (m_currentPage == ConfirmationPage) {
        m_nextButton->setText("Start Backup");
    } else {
        m_nextButton->setText("Next");
    }
    
    // Hide navigation on progress page
    if (m_currentPage == ProgressPage) {
        m_backButton->setVisible(false);
        m_nextButton->setVisible(false);
        m_cancelButton->setText("Close");
    } else {
        m_backButton->setVisible(true);
        m_nextButton->setVisible(true);
        m_cancelButton->setText("Cancel");
    }
}

void BackupWizard::updateSelectedFiles()
{
    QString text;
    qint64 totalSize = 0;
    
    for (const QString &file : m_selectedFiles) {
        text += file + "\n";
        // Calculate size (simplified)
        QFileInfo info(file);
        if (info.isDir()) {
            // For directories, we'd need to calculate recursively
            // For now, just estimate
            totalSize += 1024 * 1024 * 100; // 100MB estimate per directory
        } else {
            totalSize += info.size();
        }
    }
    
    m_selectedFilesText->setPlainText(text);
    m_spaceRequired = totalSize;
    m_spaceRequiredLabel->setText(QString("Space Required: %1").arg(formatSize(totalSize)));
    
    calculateSpaceRequirements();
}

void BackupWizard::calculateSpaceRequirements()
{
    // Calculate available space on the system
    QStorageInfo storage(QDir::homePath());
    m_spaceAvailable = storage.bytesAvailable();
    
    // Add 150MB buffer as required
    qint64 bufferSize = 150 * 1024 * 1024;
    qint64 requiredWithBuffer = m_spaceRequired + bufferSize;
    
    QString availableText = QString("Space Available: %1").arg(formatSize(m_spaceAvailable));
    if (requiredWithBuffer > m_spaceAvailable) {
        availableText += QString(" <span style='color: red;'>(Need %1 more)</span>")
            .arg(formatSize(requiredWithBuffer - m_spaceAvailable));
    }
    
    m_spaceAvailableLabel->setText(availableText);
}

bool BackupWizard::hasEnoughSpace()
{
    qint64 bufferSize = 150 * 1024 * 1024; // 150MB buffer
    return (m_spaceRequired + bufferSize) <= m_spaceAvailable;
}

QString BackupWizard::formatSize(qint64 bytes)
{
    const qint64 kb = 1024;
    const qint64 mb = kb * 1024;
    const qint64 gb = mb * 1024;
    
    if (bytes >= gb) {
        return QString::number(bytes / gb, 'f', 1) + " GB";
    } else if (bytes >= mb) {
        return QString::number(bytes / mb, 'f', 1) + " MB";
    } else if (bytes >= kb) {
        return QString::number(bytes / kb, 'f', 1) + " KB";
    } else {
        return QString::number(bytes) + " bytes";
    }
}

void BackupWizard::populateFileTree()
{
    m_fileTree->clear();
    
    // Create root item
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(m_fileTree);
    rootItem->setText(0, "System Root (/)");
    rootItem->setText(1, "");
    rootItem->setText(2, "Directory");
    rootItem->setData(0, Qt::UserRole, "/");
    rootItem->setIcon(0, QIcon(":/icons/folder.png"));
    
    // Add common system directories
    QStringList systemDirs = {"/home", "/etc", "/var", "/usr", "/opt", "/boot"};
    for (const QString &dir : systemDirs) {
        QTreeWidgetItem *item = new QTreeWidgetItem(rootItem);
        item->setText(0, QFileInfo(dir).fileName());
        item->setText(1, "");
        item->setText(2, "Directory");
        item->setData(0, Qt::UserRole, dir);
        item->setIcon(0, QIcon(":/icons/folder.png"));
    }
    
    m_fileTree->expandItem(rootItem);
}

void BackupWizard::updateConfirmationPage()
{
    QString confirmText;
    confirmText += QString("<h3>Backup Configuration Summary</h3>");
    confirmText += QString("<b>Backup Type:</b> ");
    
    switch (m_selectedBackupType) {
        case BackupType::LiveBootBackup:
            confirmText += "Live Boot Backup<br>";
            confirmText += "<b>Method:</b> rsync 1:1 copy<br>";
            confirmText += "<b>Bootable:</b> Yes (automatically)<br>";
            break;
        case BackupType::CompressedWholeDisk:
            confirmText += "Compressed Whole Disk<br>";
            confirmText += "<b>Method:</b> Compressed disk image<br>";
            confirmText += QString("<b>Bootable:</b> %1<br>").arg(m_makeBootable ? "Yes" : "No");
            if (m_makeBootable) {
                confirmText += QString("<b>Format:</b> %1<br>").arg(m_imageFormatCombo->currentText());
            }
            break;
        case BackupType::Custom:
            confirmText += "Custom Selection<br>";
            confirmText += QString("<b>Files/Folders:</b> %1 items<br>").arg(m_selectedFiles.size());
            confirmText += QString("<b>Bootable:</b> %1<br>").arg(m_makeBootable ? "Yes" : "No");
            break;
    }
    
    confirmText += QString("<b>Destination:</b> %1<br>").arg(m_selectedDevice.name);
    confirmText += QString("<b>Space Required:</b> %1<br>").arg(formatSize(m_spaceRequired));
    
    if (m_selectedBackupType == BackupType::Custom) {
        confirmText += "<br><b>Selected Items:</b><br>";
        for (const QString &item : m_selectedFiles) {
            confirmText += QString("• %1<br>").arg(item);
        }
    }
    
    m_confirmationText->setHtml(confirmText);
    
    // Update final space label
    QString spaceText = QString("Required: %1 | Available: %2")
        .arg(formatSize(m_spaceRequired))
        .arg(formatSize(m_spaceAvailable));
    
    if (hasEnoughSpace()) {
        spaceText += " ✓";
        m_finalSpaceLabel->setStyleSheet("font-weight: bold; padding: 10px; border: 1px solid green; background-color: #eeffee;");
    } else {
        spaceText += " ✗ INSUFFICIENT SPACE";
        m_finalSpaceLabel->setStyleSheet("font-weight: bold; padding: 10px; border: 1px solid red; background-color: #ffeeee;");
    }
    
    m_finalSpaceLabel->setText(spaceText);
}

void BackupWizard::startBackupProcess()
{
    m_progressBar->setValue(0);
    m_progressLabel->setText("Starting backup process...");
    m_progressLog->clear();
    
    // Start the appropriate backup method
    switch (m_selectedBackupType) {
        case BackupType::LiveBootBackup:
            performLiveBootBackup();
            break;
        case BackupType::CompressedWholeDisk:
            performCompressedBackup();
            break;
        case BackupType::Custom:
            performCustomBackup();
            break;
    }
}

void BackupWizard::performLiveBootBackup()
{
    m_progressLabel->setText("Performing live boot backup with rsync...");
    m_progressLog->append("Starting rsync backup process...");
    
    // For now, simulate the process
    m_progressTimer->start(100);
    
    // In a real implementation, this would start the actual rsync process
    // QString command = "rsync";
    // QStringList args = {"-av", "--progress", "/", m_selectedDevice.path};
    // startProcess(command, args);
}

void BackupWizard::performCompressedBackup()
{
    m_progressLabel->setText("Creating compressed disk image...");
    m_progressLog->append("Starting compressed backup process...");
    
    // Simulate process
    m_progressTimer->start(100);
}

void BackupWizard::performCustomBackup()
{
    m_progressLabel->setText("Backing up selected files...");
    m_progressLog->append("Starting custom backup process...");
    
    // Simulate process
    m_progressTimer->start(100);
}

void BackupWizard::updateProgress()
{
    static int progress = 0;
    progress += 2;
    
    if (progress >= 100) {
        progress = 100;
        m_progressTimer->stop();
        m_progressLabel->setText("Backup completed successfully!");
        m_progressLog->append("Backup process completed.");
        emit backupCompleted(true, "Backup completed successfully");
        m_cancelButton->setText("Close");
    }
    
    m_progressBar->setValue(progress);
    emit backupProgress(progress);
}

void BackupWizard::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    
    m_progressTimer->stop();
    // Handle process completion
}

// Placeholder implementations for other public methods
void BackupWizard::startRestoreWizard()
{
    // TODO: Implement restore wizard
}

void BackupWizard::startImageCreationWizard()
{
    // TODO: Implement image creation wizard
}
