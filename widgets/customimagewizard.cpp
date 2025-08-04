#include <QProcessEnvironment>
#include <QDir>
// Helper to get invoking user's home (handles sudo and login)
#include <unistd.h>
#include <pwd.h>
static QString invokingUserHome() {
    // If running under sudo, SUDO_UID holds original user's UID
    QByteArray sudoUidBa = qgetenv("SUDO_UID");
    if (!sudoUidBa.isEmpty()) {
        uid_t sudoUid = (uid_t)QString(sudoUidBa).toUInt();
        struct passwd *pw = getpwuid(sudoUid);
        if (pw && pw->pw_dir) return QString(pw->pw_dir);
    }
    // Fallback to SUDO_USER if SUDO_UID not set
    QByteArray sudoUserBa = qgetenv("SUDO_USER");
    if (!sudoUserBa.isEmpty()) {
        struct passwd *pw = getpwnam(sudoUserBa.constData());
        if (pw && pw->pw_dir) return QString(pw->pw_dir);
    }
    // Try login name
    const char *loginName = getlogin();
    if (loginName && QString(loginName) != "root") {
        struct passwd *pw = getpwnam(loginName);
        if (pw && pw->pw_dir) return QString(pw->pw_dir);
    }
    // Finally fallback to QDir
    return QDir::homePath();
}
#include "customimagewizard.h"
// #include "boardsupportdialog.h" // Not included in slimmed version
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QButtonGroup>
#include <QDate>
#include <iostream>

CustomImageWizard::CustomImageWizard(QWidget *parent)
    : QWidget(parent)
    , m_buildProcess(nullptr)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentDownload(nullptr)
    , m_imageBuilder(new ImageBuilder(this))
    , m_isBuilding(false)
    , m_currentStep(StepNone)
{
    setupUI();
    validateInputs();
    
    // Connect ImageBuilder signals
    connect(m_imageBuilder, &ImageBuilder::buildStarted, this, [this]() {
        m_isBuilding = true;
        m_buildButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_buildStepsGroup->setVisible(true);
        m_buildProgress->setVisible(true);
        emit switchToStatusTab();
    });
    
    connect(m_imageBuilder, &ImageBuilder::buildProgress, this, [this](int percentage, const QString &description) {
        m_buildProgress->setValue(percentage);
        m_statusLabel->setText(description);
    });
    
    connect(m_imageBuilder, &ImageBuilder::buildStepChanged, this, [this](const QString &step, const QString &description) {
        updateBuildStepDisplay(step, description);
    });
    
    connect(m_imageBuilder, &ImageBuilder::buildLogMessage, this, &CustomImageWizard::logMessage);
    
    connect(m_imageBuilder, &ImageBuilder::buildCompleted, this, [this](bool success, const QString &message) {
        m_isBuilding = false;
        m_buildButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_buildProgress->setVisible(false);
        
        if (success) {
            m_statusLabel->setText("Build completed successfully!");
            QMessageBox::information(this, "Build Complete", message);
        } else {
            m_statusLabel->setText("Build failed: " + message);
            QMessageBox::warning(this, "Build Failed", message);
        }
        
        emit buildFinished(success);
    });
    
    connect(m_imageBuilder, &ImageBuilder::buildError, this, [this](const QString &error) {
        logMessage("ERROR: " + error);
    });
}

CustomImageWizard::~CustomImageWizard()
{
    if (m_buildProcess) {
        m_buildProcess->terminate();
        if (!m_buildProcess->waitForFinished(5000)) {
            m_buildProcess->kill();
        }
        delete m_buildProcess;
    }
    if (m_currentDownload) {
        m_currentDownload->abort();
        m_currentDownload->deleteLater();
    }
}

void CustomImageWizard::setGithubToken(const QString &token)
{
    m_githubToken = token;
}

void CustomImageWizard::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    
    // Title
    QLabel *title = new QLabel("Orange Pi 5 Plus - Custom Ubuntu Image Builder");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #ff6600; margin: 5px;");
    mainLayout->addWidget(title);
    
    // Create horizontal layout for source selection and kernel build
    QHBoxLayout *sourceKernelLayout = new QHBoxLayout();
    
    // Live Image Creator Group (left half)
    m_sourceGroup = new QGroupBox("Live Image Creator");
    QVBoxLayout *sourceLayout = new QVBoxLayout(m_sourceGroup);
    
    // Provider and Branch selection at the top (for Live Image base system)
    QHBoxLayout *liveImageProviderLayout = new QHBoxLayout();
    liveImageProviderLayout->addWidget(new QLabel("Provider:"));
    
    // Note: m_liveImageProviderCombo is not in header, using m_branchCombo for live image branches
    QComboBox *liveImageProviderCombo = new QComboBox();
    liveImageProviderCombo->addItem("Rockchip");
    liveImageProviderCombo->addItem("Armbian");
    liveImageProviderCombo->addItem("Joshua-Riek");
    liveImageProviderCombo->setMaximumWidth(120);
    liveImageProviderLayout->addWidget(liveImageProviderCombo);
    
    liveImageProviderLayout->addWidget(new QLabel("Branch:"));
    // Use m_branchCombo as the live image branch combo
    m_branchCombo = new QComboBox();
    m_branchCombo->setMaximumWidth(150);
    m_refreshBranchesButton = new QPushButton("Refresh");
    m_refreshBranchesButton->setMaximumWidth(80);
    liveImageProviderLayout->addWidget(m_branchCombo);
    liveImageProviderLayout->addWidget(m_refreshBranchesButton);
    liveImageProviderLayout->addStretch();
    sourceLayout->addLayout(liveImageProviderLayout);
    
    // Custom git location for live image
    m_customLiveImageCheck = new QCheckBox("Download from custom location");
    sourceLayout->addWidget(m_customLiveImageCheck);
    m_customLiveImageEdit = new QLineEdit();
    m_customLiveImageEdit->setPlaceholderText("git clone -b kernel-5.10 --recursive https://github.com/armbian/linux-rockchip.git");
    m_customLiveImageEdit->setVisible(false);
    sourceLayout->addWidget(m_customLiveImageEdit);
    
    // Checkboxes for source type
    m_localSourceCheck = new QCheckBox("Use local source code");
    m_kernelOnlyCheck = new QCheckBox("Build kernel packages only");
    sourceLayout->addWidget(m_localSourceCheck);
    sourceLayout->addWidget(m_kernelOnlyCheck);
    
    // Board Support Package options (not in header, create as local widgets)
    QCheckBox *includeBoardSupportCheck = new QCheckBox("Include Orange Pi 5 Plus board support");
    includeBoardSupportCheck->setChecked(true);
    sourceLayout->addWidget(includeBoardSupportCheck);
    
    QPushButton *selectBoardSupportButton = new QPushButton("Configure Board Package");
    selectBoardSupportButton->setMaximumWidth(180);
    selectBoardSupportButton->setEnabled(false);
    selectBoardSupportButton->setVisible(false);
    sourceLayout->addWidget(selectBoardSupportButton);
    
    // Connect board support widgets
    connect(includeBoardSupportCheck, &QCheckBox::toggled, [selectBoardSupportButton](bool checked) {
        selectBoardSupportButton->setEnabled(checked);
        selectBoardSupportButton->setVisible(checked);
    });
    
    // Stacked widget for different source options
    m_sourceStack = new QStackedWidget();
    
    // Page 0: Default download page
    QWidget *downloadPage = new QWidget();
    QVBoxLayout *downloadLayout = new QVBoxLayout(downloadPage);
    downloadLayout->setContentsMargins(0, 0, 0, 0);
    
    
    m_downloadProgress = new QProgressBar();
    m_downloadProgress->setVisible(false);
    m_downloadStatusLabel = new QLabel("");
    m_downloadStatusLabel->setStyleSheet("color: #666;");
    
    downloadLayout->addWidget(m_downloadStatusLabel);
    downloadLayout->addWidget(m_downloadProgress);
    
    // Page 1: Local source page
    QWidget *localSourcePage = new QWidget();
    QHBoxLayout *localLayout = new QHBoxLayout(localSourcePage);
    localLayout->setContentsMargins(0, 0, 0, 0);
    m_localSourceEdit = new QLineEdit();
    m_localSourceEdit->setPlaceholderText("/path/to/ubuntu-rockchip");
    m_browseLocalButton = new QPushButton("Browse...");
    m_browseLocalButton->setMaximumWidth(100);
    localLayout->addWidget(new QLabel("Path:"));
    localLayout->addWidget(m_localSourceEdit);
    localLayout->addWidget(m_browseLocalButton);
    
    // Page 2: Custom kernel page
    QWidget *customKernelPage = new QWidget();
    QVBoxLayout *customLayout = new QVBoxLayout(customKernelPage);
    customLayout->setContentsMargins(0, 0, 0, 0);
    
    // Note: These widgets aren't in the header, so they're local to this function
    QLabel *infoLabel = new QLabel("Configure custom kernel source location");
    customLayout->addWidget(infoLabel);
    customLayout->addStretch();
    
    m_sourceStack->addWidget(downloadPage);
    m_sourceStack->addWidget(localSourcePage);
    m_sourceStack->addWidget(customKernelPage);
    m_sourceStack->setCurrentIndex(0);
    
    sourceLayout->addWidget(m_sourceStack);
    
    // Kernel Build Group (right half)
    m_kernelBuildGroup = new QGroupBox("Kernel Build Options");
    QVBoxLayout *kernelBuildLayout = new QVBoxLayout(m_kernelBuildGroup);
    m_kernelBuildGroup->setVisible(false); // Hidden by default
    
    // Rockchip kernel source selection (locked to Rockchip only)
    QLabel *rockchipLabel = new QLabel("Rockchip Linux Kernel");
    rockchipLabel->setStyleSheet("font-weight: bold; color: #ff6600;");
    kernelBuildLayout->addWidget(rockchipLabel);
    
    // Branch selection for Rockchip (no provider dropdown - locked to Rockchip)
    QHBoxLayout *rockchipBranchLayout = new QHBoxLayout();
    rockchipBranchLayout->addWidget(new QLabel("Branch:"));
    m_rockchipBranchCombo = new QComboBox();
    m_rockchipBranchCombo->setMaximumWidth(200);
    m_refreshRockchipBranchesButton = new QPushButton("Refresh");
    m_refreshRockchipBranchesButton->setMaximumWidth(80);
    rockchipBranchLayout->addWidget(m_rockchipBranchCombo);
    rockchipBranchLayout->addWidget(m_refreshRockchipBranchesButton);
    rockchipBranchLayout->addStretch();
    kernelBuildLayout->addLayout(rockchipBranchLayout);
    
    // Custom kernel git location
    m_customKernelCheck = new QCheckBox("Download kernel from custom location");
    kernelBuildLayout->addWidget(m_customKernelCheck);
    m_customKernelEdit = new QLineEdit();
    m_customKernelEdit->setPlaceholderText("git clone -b kernel-5.10 --recursive https://github.com/armbian/linux-rockchip.git");
    m_customKernelEdit->setVisible(false);
    kernelBuildLayout->addWidget(m_customKernelEdit);
    
    // Local kernel source option
    m_localKernelCheck = new QCheckBox("Use local kernel source");
    kernelBuildLayout->addWidget(m_localKernelCheck);
    m_localKernelSourceEdit = new QLineEdit();
    m_localKernelSourceEdit->setPlaceholderText("/path/to/kernel/source");
    m_localKernelSourceEdit->setVisible(false);
    m_browseLocalKernelButton = new QPushButton("Browse...");
    m_browseLocalKernelButton->setMaximumWidth(100);
    m_browseLocalKernelButton->setVisible(false);
    QHBoxLayout *localKernelLayout = new QHBoxLayout();
    localKernelLayout->addWidget(m_localKernelSourceEdit);
    localKernelLayout->addWidget(m_browseLocalKernelButton);
    kernelBuildLayout->addLayout(localKernelLayout);
    
    // Custom kernel options
    QLabel *optionsLabel = new QLabel("Kernel Options:");
    optionsLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    kernelBuildLayout->addWidget(optionsLabel);
    
    m_enableF2fsCheck = new QCheckBox("Enable F2FS filesystem support");
    m_enableF2fsCheck->setChecked(true);
    kernelBuildLayout->addWidget(m_enableF2fsCheck);
    
    m_enableCompressionCheck = new QCheckBox("Enable kernel compression (LZ4/ZSTD)");
    m_enableCompressionCheck->setChecked(true);
    kernelBuildLayout->addWidget(m_enableCompressionCheck);
    
    m_enableDebugCheck = new QCheckBox("Enable debug symbols");
    kernelBuildLayout->addWidget(m_enableDebugCheck);
    
    kernelBuildLayout->addStretch();
    
    // Stack for kernel source options
    m_kernelSourceStack = new QStackedWidget();
    
    // Create checkboxes for kernel source type (replacing radio buttons)
    QCheckBox *kernelOnlineSourceCheck = new QCheckBox("Download kernel source");
    QCheckBox *kernelLocalSourceCheck = new QCheckBox("Use local kernel source");
    kernelOnlineSourceCheck->setChecked(true);
    
    // Add checkboxes to kernel build layout
    QHBoxLayout *kernelSourceTypeLayout = new QHBoxLayout();
    kernelSourceTypeLayout->addWidget(kernelOnlineSourceCheck);
    kernelSourceTypeLayout->addWidget(kernelLocalSourceCheck);
    kernelSourceTypeLayout->addStretch();
    kernelBuildLayout->addLayout(kernelSourceTypeLayout);
    
    // Initialize the radio button members as null (they exist in header but we're using checkboxes)
    m_kernelLocalSourceRadio = nullptr;
    m_kernelOnlineSourceRadio = nullptr;
    
    // Add kernel patches controls
    m_applyKernelPatchesCheck = new QCheckBox("Apply Joshua Riek's kernel patches");
    m_selectKernelPatchesButton = new QPushButton("Select Patches...");
    m_selectKernelPatchesButton->setEnabled(false);
    
    QHBoxLayout *kernelPatchLayout = new QHBoxLayout();
    kernelPatchLayout->addWidget(m_applyKernelPatchesCheck);
    kernelPatchLayout->addWidget(m_selectKernelPatchesButton);
    kernelPatchLayout->addStretch();
    kernelBuildLayout->addLayout(kernelPatchLayout);
    
    connect(m_applyKernelPatchesCheck, &QCheckBox::toggled, m_selectKernelPatchesButton, &QPushButton::setEnabled);
    
    // Initialize kernel source stack pages
    QWidget *kernelOnlinePage = new QWidget();
    QVBoxLayout *kernelOnlineLayout = new QVBoxLayout(kernelOnlinePage);
    
    
    kernelOnlineLayout->addStretch();
    
    QWidget *kernelLocalPage = new QWidget();
    QHBoxLayout *kernelLocalLayout = new QHBoxLayout(kernelLocalPage);
    QLineEdit *kernelLocalSourceEdit = new QLineEdit();
    QPushButton *browseKernelLocalButton = new QPushButton("Browse...");
    kernelLocalLayout->addWidget(kernelLocalSourceEdit);
    kernelLocalLayout->addWidget(browseKernelLocalButton);
    
    m_kernelSourceStack->addWidget(kernelOnlinePage);
    m_kernelSourceStack->addWidget(kernelLocalPage);
    kernelBuildLayout->addWidget(m_kernelSourceStack);
    
    // Connect checkboxes to stack widget (ensure only one is checked at a time)
    connect(kernelOnlineSourceCheck, &QCheckBox::toggled, this, [this, kernelLocalSourceCheck](bool checked) {
        if (checked) {
            kernelLocalSourceCheck->setChecked(false);
            m_kernelSourceStack->setCurrentIndex(0);
        }
    });
    connect(kernelLocalSourceCheck, &QCheckBox::toggled, this, [this, kernelOnlineSourceCheck](bool checked) {
        if (checked) {
            kernelOnlineSourceCheck->setChecked(false);
            m_kernelSourceStack->setCurrentIndex(1);
        }
    });
    
    // Add widgets to the horizontal split layout
    sourceKernelLayout->addWidget(m_sourceGroup);
    sourceKernelLayout->addWidget(m_kernelBuildGroup);
    
    // Add the split layout to main layout
    mainLayout->addLayout(sourceKernelLayout);
    
    // Add main patches controls (these are in the header but weren't added)
    m_applyMainPatchesCheck = new QCheckBox("Apply kernel patches");
    m_selectMainPatchesButton = new QPushButton("Select Patches...");
    m_selectMainPatchesButton->setEnabled(false);
    
    QHBoxLayout *mainPatchLayout = new QHBoxLayout();
    mainPatchLayout->addWidget(m_applyMainPatchesCheck);
    mainPatchLayout->addWidget(m_selectMainPatchesButton);
    mainPatchLayout->addStretch();
    mainLayout->addLayout(mainPatchLayout);
    
    connect(m_applyMainPatchesCheck, &QCheckBox::toggled, m_selectMainPatchesButton, &QPushButton::setEnabled);
    
    // Combined Build Configuration & Options
    QGroupBox *configGroup = new QGroupBox("Build Configuration & Options");
    QGridLayout *configLayout = new QGridLayout(configGroup);
    
    // Left side - Configuration
    QLabel *suiteLabel = new QLabel("Ubuntu Suite:");
    m_suiteCombo = new QComboBox();
    m_suiteCombo->setMaximumWidth(150);
    m_suiteCombo->addItem("jammy (22.04 LTS)");
    m_suiteCombo->addItem("noble (24.04 LTS)");
    m_suiteCombo->addItem("oracular (24.10)");
    m_suiteCombo->addItem("plucky (25.04)");
    
    QLabel *flavorLabel = new QLabel("Flavor:");
    m_flavorCombo = new QComboBox();
    m_flavorCombo->setMaximumWidth(150);
    m_flavorCombo->addItem("desktop");
    m_flavorCombo->addItem("server");
    
    QLabel *partitionLabel = new QLabel("Filesystem:");
    m_partitionTypeCombo = new QComboBox();
    m_partitionTypeCombo->setMaximumWidth(150);
    m_partitionTypeCombo->addItem("ext4");
    m_partitionTypeCombo->addItem("f2fs");
    
    configLayout->addWidget(suiteLabel, 0, 0);
    configLayout->addWidget(m_suiteCombo, 0, 1);
    configLayout->addWidget(flavorLabel, 1, 0);
    configLayout->addWidget(m_flavorCombo, 1, 1);
    configLayout->addWidget(partitionLabel, 2, 0);
    configLayout->addWidget(m_partitionTypeCombo, 2, 1);
    
    // Right side - Options
    m_cleanBuildCheck = new QCheckBox("Clean build");
    m_verboseCheck = new QCheckBox("Verbose output");
    m_includeWifiCheck = new QCheckBox("Include WiFi drivers");
    m_includeBluetoothCheck = new QCheckBox("Include Bluetooth");
    m_includeGpuDriversCheck = new QCheckBox("Include GPU drivers");
    
    m_includeWifiCheck->setChecked(true);
    m_includeBluetoothCheck->setChecked(true);
    m_includeGpuDriversCheck->setChecked(true);
    
    configLayout->addWidget(m_cleanBuildCheck, 0, 2);
    configLayout->addWidget(m_verboseCheck, 0, 3);
    configLayout->addWidget(m_includeWifiCheck, 1, 2);
    configLayout->addWidget(m_includeBluetoothCheck, 1, 3);
    configLayout->addWidget(m_includeGpuDriversCheck, 2, 2);
    
    // Add column stretch
    configLayout->setColumnStretch(1, 1);
    configLayout->setColumnStretch(3, 1);
    configLayout->setColumnStretch(4, 2);
    
    mainLayout->addWidget(configGroup);
    
    // Output Directory
    QGroupBox *outputGroup = new QGroupBox("Output Directory");
    QHBoxLayout *outputLayout = new QHBoxLayout(outputGroup);
    
    m_outputDirEdit = new QLineEdit();
    m_outputDirEdit->setText(invokingUserHome() + "/tweaker/images");
    m_browseDirButton = new QPushButton("Browse...");
    m_browseDirButton->setMaximumWidth(100);
    
    outputLayout->addWidget(m_outputDirEdit);
    outputLayout->addWidget(m_browseDirButton);
    
    mainLayout->addWidget(outputGroup);
    
    // Build Steps Progress
    m_buildStepsGroup = new QGroupBox("Build Progress");
    QVBoxLayout *stepsLayout = new QVBoxLayout(m_buildStepsGroup);
    
    m_stepKernelLabel = new QLabel("• Kernel: Not started");
    m_stepUBootLabel = new QLabel("• U-Boot: Not started");
    m_stepRootfsLabel = new QLabel("• Root filesystem: Not started");
    m_stepImageLabel = new QLabel("• Disk image: Not started");
    
    stepsLayout->addWidget(m_stepKernelLabel);
    stepsLayout->addWidget(m_stepUBootLabel);
    stepsLayout->addWidget(m_stepRootfsLabel);
    stepsLayout->addWidget(m_stepImageLabel);
    
    m_buildStepsGroup->setVisible(false);
    mainLayout->addWidget(m_buildStepsGroup);
    
    // Build Controls
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    m_buildButton = new QPushButton("Start Build");
    m_buildButton->setMinimumHeight(35);
    m_buildButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    
    m_stopButton = new QPushButton("Stop Build");
    m_stopButton->setMinimumHeight(35);
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; }");
    
    controlLayout->addWidget(m_buildButton);
    controlLayout->addWidget(m_stopButton);
    
    mainLayout->addLayout(controlLayout);
    
    // Overall progress
    m_buildProgress = new QProgressBar();
    m_buildProgress->setVisible(false);
    mainLayout->addWidget(m_buildProgress);
    
    // Status
    m_statusLabel = new QLabel("Ready to build Orange Pi 5 Plus image");
    m_statusLabel->setStyleSheet("QLabel { color: #666; padding: 5px; }");
    mainLayout->addWidget(m_statusLabel);
    
    // Add stretch at bottom to push everything up
    mainLayout->addStretch();
    
    // Connect signals for checkboxes
    connect(m_localSourceCheck, &QCheckBox::toggled, [this](bool checked) {
        if (checked) {
            m_sourceStack->setCurrentIndex(1);
            m_kernelOnlyCheck->setChecked(false);
        } else {
            m_sourceStack->setCurrentIndex(0);
        }
        validateInputs();
    });
    
    connect(m_kernelOnlyCheck, &QCheckBox::toggled, [this](bool checked) {
        if (checked) {
            m_localSourceCheck->setChecked(false);
        }
        m_kernelBuildGroup->setVisible(checked);
        validateInputs();
    });
    
    
    connect(m_customLiveImageCheck, &QCheckBox::toggled, [this](bool checked) {
        m_customLiveImageEdit->setVisible(checked);
    });
    
    connect(m_customKernelCheck, &QCheckBox::toggled, [this](bool checked) {
        m_customKernelEdit->setVisible(checked);
    });
    
    connect(m_localKernelCheck, &QCheckBox::toggled, [this](bool checked) {
        m_localKernelSourceEdit->setVisible(checked);
        m_browseLocalKernelButton->setVisible(checked);
    });
    
    // Connect board support button (moved to local widget connections above)
    connect(selectBoardSupportButton, &QPushButton::clicked, [this]() {
        // For now, just show a message since BoardSupportDialog interface is different
        emit logMessage("Board support configuration dialog would open here");
        // TODO: Implement proper board support dialog interface
    });
    
    // Connect other UI elements
    connect(m_browseLocalButton, &QPushButton::clicked, this, &CustomImageWizard::onBrowseLocalSourceClicked);
    // Note: m_browseKernelButton doesn't exist in header, connect handled elsewhere
    connect(m_browseLocalKernelButton, &QPushButton::clicked, this, &CustomImageWizard::onBrowseLocalKernelSourceClicked);
    connect(m_browseDirButton, &QPushButton::clicked, this, &CustomImageWizard::onBrowseOutputDirClicked);
    connect(m_refreshBranchesButton, &QPushButton::clicked, this, &CustomImageWizard::onRefreshBranchesClicked);
    // Connect patch selection buttons
    connect(m_selectKernelPatchesButton, &QPushButton::clicked, this, [this]() {
        emit logMessage("Kernel patch selection dialog would open here");
        // TODO: Implement patch selection dialog
    });
    
    connect(m_selectMainPatchesButton, &QPushButton::clicked, this, [this]() {
        emit logMessage("Main patch selection dialog would open here");
        // TODO: Implement patch selection dialog
    });
    connect(m_refreshRockchipBranchesButton, &QPushButton::clicked, this, &CustomImageWizard::onRefreshRockchipBranchesClicked);
    connect(m_buildButton, &QPushButton::clicked, this, &CustomImageWizard::onBuildClicked);
    connect(m_stopButton, &QPushButton::clicked, [this]() {
        if (m_imageBuilder && m_imageBuilder->isBuilding()) {
            m_imageBuilder->cancelBuild();
            m_statusLabel->setText("Build cancelled by user");
        } else if (m_buildProcess) {
            m_buildProcess->terminate();
            m_statusLabel->setText("Build cancelled by user");
        }
    });
    
    connect(m_localSourceEdit, &QLineEdit::textChanged, this, &CustomImageWizard::validateInputs);
    // Connect local kernel edit widgets to validation (widgets are local now)
    connect(m_localKernelSourceEdit, &QLineEdit::textChanged, this, &CustomImageWizard::validateInputs);
    connect(m_customLiveImageEdit, &QLineEdit::textChanged, this, &CustomImageWizard::validateInputs);
    connect(m_customKernelEdit, &QLineEdit::textChanged, this, &CustomImageWizard::validateInputs);
    
    // Load default branches
    QTimer::singleShot(100, this, &CustomImageWizard::onRefreshBranchesClicked);
    QTimer::singleShot(200, this, &CustomImageWizard::onRefreshRockchipBranchesClicked);
}

void CustomImageWizard::onBrowseLocalSourceClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        "Select Ubuntu Rockchip Source Directory", 
        invokingUserHome(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!dir.isEmpty()) {
        m_localSourceEdit->setText(dir);
    }
}

void CustomImageWizard::onBrowseKernelSourceClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        "Select Kernel Source Directory", 
        invokingUserHome(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!dir.isEmpty()) {
        // The kernel source edit is now local to setupUI
        emit logMessage("Selected kernel source: " + dir);
    }
}

void CustomImageWizard::onBrowseLocalKernelSourceClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        "Select Local Kernel Source Directory", 
        invokingUserHome(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!dir.isEmpty()) {
        m_localKernelSourceEdit->setText(dir);
    }
}

void CustomImageWizard::onBrowseOutputDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        "Select Output Directory", 
        m_outputDirEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!dir.isEmpty()) {
        m_outputDirEdit->setText(dir);
    }
}

void CustomImageWizard::onRefreshBranchesClicked()
{
    m_branchCombo->clear();
    m_branchCombo->addItem("Fetching branches...");
    m_refreshBranchesButton->setEnabled(false);
    
    // Fetch branches from Joshua Riek's livecd-rootfs repository
    QNetworkRequest request(QUrl("https://api.github.com/repos/Joshua-Riek/livecd-rootfs/branches"));
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    if (!m_githubToken.isEmpty()) {
        QByteArray auth = "token " + m_githubToken.toUtf8();
        request.setRawHeader("Authorization", auth);
    }
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        m_branchCombo->clear();
        m_refreshBranchesButton->setEnabled(true);
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray branches = doc.array();
            
            for (const QJsonValue &value : branches) {
                QJsonObject branch = value.toObject();
                QString name = branch["name"].toString();
                if (!name.isEmpty()) {
                    m_branchCombo->addItem(name);
                }
            }
            
            // Select main branch by default
            int mainIndex = m_branchCombo->findText("main");
            if (mainIndex >= 0) {
                m_branchCombo->setCurrentIndex(mainIndex);
            }
            
            m_downloadStatusLabel->setText("Branches loaded successfully");
        } else {
            m_branchCombo->addItem("main");
            m_branchCombo->addItem("develop");
            m_downloadStatusLabel->setText("Failed to fetch branches, using defaults");
        }
        
        reply->deleteLater();
    });
}


void CustomImageWizard::onRefreshRockchipBranchesClicked()
{
    fetchRockchipBranches();
}

void CustomImageWizard::fetchRockchipBranches()
{
    m_rockchipBranchCombo->clear();
    m_rockchipBranchCombo->addItem("Loading branches...");
    m_refreshRockchipBranchesButton->setEnabled(false);
    
    // Create network request to fetch branches from Rockchip Linux kernel repo
    QNetworkRequest request(QUrl("https://api.github.com/repos/rockchip-linux/kernel/branches"));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Arm-Pi-Tweaker/1.0");
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();
        m_refreshRockchipBranchesButton->setEnabled(true);
        
        if (reply->error() != QNetworkReply::NoError) {
            m_rockchipBranchCombo->clear();
            m_rockchipBranchCombo->addItem("Error loading branches");
            logMessage("Failed to fetch Rockchip kernel branches: " + reply->errorString());
            return;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray branches = doc.array();
        
        m_rockchipBranchCombo->clear();
        
        // Add common/recommended branches first
        QStringList priorityBranches = {"develop-5.10", "stable-5.10", "linux-5.10.y", "develop-6.1", "stable-6.1"};
        QStringList allBranches;
        
        // Collect all branch names
        for (const QJsonValue &value : branches) {
            QJsonObject branch = value.toObject();
            QString branchName = branch["name"].toString();
            if (!branchName.isEmpty()) {
                allBranches.append(branchName);
            }
        }
        
        // Add priority branches first (if they exist)
        for (const QString &priorityBranch : priorityBranches) {
            if (allBranches.contains(priorityBranch)) {
                m_rockchipBranchCombo->addItem(priorityBranch);
                allBranches.removeAll(priorityBranch);
            }
        }
        
        // Add separator if we have both priority and other branches
        if (m_rockchipBranchCombo->count() > 0 && !allBranches.isEmpty()) {
            m_rockchipBranchCombo->insertSeparator(m_rockchipBranchCombo->count());
        }
        
        // Add remaining branches sorted alphabetically
        allBranches.sort();
        for (const QString &branchName : allBranches) {
            m_rockchipBranchCombo->addItem(branchName);
        }
        
        // Set default selection to develop-5.10 if available, otherwise first item
        int developIndex = m_rockchipBranchCombo->findText("develop-5.10");
        if (developIndex >= 0) {
            m_rockchipBranchCombo->setCurrentIndex(developIndex);
        }
        
        logMessage(QString("Loaded %1 Rockchip kernel branches").arg(branches.size()));
    });
}

void CustomImageWizard::onDownloadSourceClicked()
{
    QString branch = m_branchCombo->currentText();
    if (branch.isEmpty() || branch == "Fetching branches...") {
        QMessageBox::warning(this, "No Branch Selected", "Please select a branch to download.");
        return;
    }
    
    m_downloadProgress->setVisible(true);
    m_downloadProgress->setRange(0, 0); // Indeterminate
    m_downloadStatusLabel->setText("Downloading required repositories...");

    // Prompt user for download location
    QString baseSrcDir = QFileDialog::getExistingDirectory(this,
        "Select Download Location", invokingUserHome(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (baseSrcDir.isEmpty()) {
        m_downloadStatusLabel->setText("Download canceled by user.");
        m_downloadProgress->setVisible(false);
        return;
    }
    
    // Set custom download location in ImageBuilder
    m_imageBuilder->setDownloadLocation(baseSrcDir);
    
    // Set up source dirs
    m_sourceDir = baseSrcDir + "/livecd-rootfs";
    m_kernelDir = baseSrcDir + "/linux-rockchip";
    QDir().mkpath(baseSrcDir);
    
    // Start with livecd-rootfs repository
    m_currentStep = StepDownloading;
    m_downloadStatusLabel->setText("Cloning livecd-rootfs repository...");
    
    // Get the suite name for branch mapping
    QString suite = m_suiteCombo->currentText().split(" ").first();
    QString livecdBranch = getLivecdRootfsBranch(suite);
    cloneRepository("https://github.com/Joshua-Riek/livecd-rootfs.git", livecdBranch, m_sourceDir);
}

void CustomImageWizard::cloneRepository(const QString &url, const QString &branch, const QString &targetDir)
{
    // If targetDir already contains source, skip clone to avoid loop
    QDir dir(targetDir);
    if (dir.exists()) {
        QStringList contents = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        if (!contents.isEmpty()) {
            emit logMessage(QString("Source already present at %1; skipping clone.").arg(targetDir));
            // Advance logic without re-cloning
            if (m_currentStep == StepDownloading) {
                // Skip livecd-rootfs, proceed to download kernel
                m_currentStep = StepDownloadingKernel;
                m_downloadStatusLabel->setText("Cloning linux-rockchip kernel repository...");
                QString suite = m_suiteCombo->currentText().split(" ").first();
                cloneRepository("https://github.com/Joshua-Riek/linux-rockchip.git", suite, m_kernelDir);
            } else if (m_currentStep == StepDownloadingKernel) {
                // Both repos present, finish download phase
                onDownloadFinished();
            }
            return;
        }
    } else {
        // Ensure parent path exists before cloning
        QFileInfo fi(targetDir);
        QDir().mkpath(fi.path());
    }
    
    QString command = QString("git clone --depth 1 --branch '%1' '%2' '%3'")
                        .arg(branch)
                        .arg(url)
                        .arg(targetDir);
    
    executeCommand(command, invokingUserHome());
}

void CustomImageWizard::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        m_downloadProgress->setRange(0, 100);
        m_downloadProgress->setValue((bytesReceived * 100) / bytesTotal);
        
        QString status = QString("Downloading: %1 MB / %2 MB")
                            .arg(bytesReceived / 1048576.0, 0, 'f', 1)
                            .arg(bytesTotal / 1048576.0, 0, 'f', 1);
        m_downloadStatusLabel->setText(status);
    }
}

void CustomImageWizard::onDownloadFinished()
{
    m_downloadProgress->setVisible(false);
    
    if (m_currentStep == StepDownloadingKernel) {
        m_downloadStatusLabel->setText("All repositories downloaded successfully!");
        m_currentStep = StepNone;
        validateInputs();
    }
}

void CustomImageWizard::validateInputs()
{
    bool valid = false;
    
    if (m_kernelOnlyCheck->isChecked()) {
        // Kernel-only build validation
        valid = true; // Kernel-only mode can always proceed
    } else if (m_localSourceCheck->isChecked()) {
        valid = !m_localSourceEdit->text().isEmpty() && QDir(m_localSourceEdit->text()).exists();
    } else {
        // Default download option - check if source already downloaded
        valid = !m_sourceDir.isEmpty() && !m_kernelDir.isEmpty() && 
                QDir(m_sourceDir).exists() && QDir(m_kernelDir).exists();
    }
    
    m_buildButton->setEnabled(valid && !m_isBuilding);
}

void CustomImageWizard::onBuildClicked()
{
    // Configure ImageBuilder
    ImageBuilder::BuildConfiguration config;
    
    // Set suite and flavor
    QString suiteText = m_suiteCombo->currentText().split(" ").first();
    config.suite = ImageBuilder::stringToSuite(suiteText);
    config.flavor = ImageBuilder::stringToFlavor(m_flavorCombo->currentText());
    config.partitionType = ImageBuilder::stringToPartitionType(m_partitionTypeCombo->currentText());
    
    // Set directories
    config.outputDir = m_outputDirEdit->text();
    
    // Set build options
    config.cleanBuild = m_cleanBuildCheck->isChecked();
    config.verboseOutput = m_verboseCheck->isChecked();
    config.includeWifi = m_includeWifiCheck->isChecked();
    config.includeBluetooth = m_includeBluetoothCheck->isChecked();
    config.includeGpuDrivers = m_includeGpuDriversCheck->isChecked();
    
    // Board support is always enabled for Orange Pi 5+
    // The ImageBuilder will handle board support internally
    
    // Determine kernel source type and build mode
    if (m_kernelOnlyCheck->isChecked()) {
        config.buildMode = ImageBuilder::KernelOnly;
        config.kernelSource = ImageBuilder::RemoteKernel;
    } else if (m_localSourceCheck->isChecked()) {
        config.buildMode = ImageBuilder::FullBuild;
        config.kernelSource = ImageBuilder::LocalKernel;
        config.localKernelPath = m_localSourceEdit->text();
    } else {
        // Download sources (default)
        config.buildMode = ImageBuilder::FullBuild;
        config.kernelSource = ImageBuilder::RemoteKernel;
    }
    
    // Set configuration and start build
    m_imageBuilder->setConfiguration(config);
    
    if (config.buildMode == ImageBuilder::KernelOnly) {
        m_imageBuilder->startKernelOnlyBuild();
    } else {
        m_imageBuilder->startBuild();
    }
}

void CustomImageWizard::startBuildProcess()
{
    updateBuildSteps();
    buildKernel();
}

void CustomImageWizard::updateBuildSteps()
{
    QString notStarted = "Not started";
    QString inProgress = "<span style='color: #FFA500;'>In progress...</span>";
    QString completed = "<span style='color: #00FF00;'>✓ Completed</span>";
    QString failed = "<span style='color: #FF0000;'>✗ Failed</span>";
    
    switch (m_currentStep) {
        case StepKernel:
            m_stepKernelLabel->setText("• Kernel: " + inProgress);
            break;
        case StepUBoot:
            m_stepKernelLabel->setText("• Kernel: " + completed);
            m_stepUBootLabel->setText("• U-Boot: " + inProgress);
            m_buildProgress->setValue(25);
            break;
        case StepRootfs:
            m_stepKernelLabel->setText("• Kernel: " + completed);
            m_stepUBootLabel->setText("• U-Boot: " + completed);
            m_stepRootfsLabel->setText("• Root filesystem: " + inProgress);
            m_buildProgress->setValue(50);
            break;
        case StepImage:
            m_stepKernelLabel->setText("• Kernel: " + completed);
            m_stepUBootLabel->setText("• U-Boot: " + completed);
            m_stepRootfsLabel->setText("• Root filesystem: " + completed);
            m_stepImageLabel->setText("• Disk image: " + inProgress);
            m_buildProgress->setValue(75);
            break;
        default:
            break;
    }
}

void CustomImageWizard::buildKernel()
{
    m_currentStep = StepKernel;
    updateBuildSteps();
    
    emit logMessage("\n=== Building Kernel ===");
    m_statusLabel->setText("Building kernel for Orange Pi 5 Plus...");
    
    // Implementation details for kernel build
    // This would contain the actual build commands
}

void CustomImageWizard::buildUBoot()
{
    m_currentStep = StepUBoot;
    updateBuildSteps();
    
    emit logMessage("\n=== Building U-Boot ===");
    m_statusLabel->setText("Building U-Boot for Orange Pi 5 Plus...");
    
    // Implementation details for U-Boot build
}

void CustomImageWizard::buildRootfs()
{
    m_currentStep = StepRootfs;
    updateBuildSteps();
    
    emit logMessage("\n=== Building Root Filesystem ===");
    m_statusLabel->setText("Creating root filesystem...");
    
    // Implementation details for rootfs build
}

void CustomImageWizard::createImage()
{
    m_currentStep = StepImage;
    updateBuildSteps();
    
    emit logMessage("\n=== Creating Disk Image ===");
    m_statusLabel->setText("Creating disk image...");
    
    // Implementation details for image creation
}

void CustomImageWizard::executeCommand(const QString &command, const QString &workDir)
{
    if (!m_buildProcess) {
        m_buildProcess = new QProcess(this);
        m_buildProcess->setProcessChannelMode(QProcess::MergedChannels);
        
        connect(m_buildProcess, &QProcess::readyRead, this, &CustomImageWizard::onProcessOutput);
        connect(m_buildProcess, &QProcess::errorOccurred, this, &CustomImageWizard::onProcessError);
        connect(m_buildProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &CustomImageWizard::onProcessFinished);
    }
    
    if (!workDir.isEmpty()) {
        m_buildProcess->setWorkingDirectory(workDir);
    }
    
    emit logMessage("$ " + command);
    emit logMessage(""); // Empty line for readability
    m_buildProcess->start("bash", QStringList() << "-c" << command);
}

void CustomImageWizard::onProcessOutput()
{
    if (!m_buildProcess) return;
    
    QString output = m_buildProcess->readAll();
    
    // Send output to status tab line by line for better formatting
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        emit logMessage(line);
    }
}

void CustomImageWizard::onProcessError()
{
    QString error = "Process error: ";
    if (m_buildProcess) {
        switch (m_buildProcess->error()) {
            case QProcess::FailedToStart:
                error += "Failed to start";
                break;
            case QProcess::Crashed:
                error += "Process crashed";
                break;
            case QProcess::Timedout:
                error += "Process timed out";
                break;
            default:
                error += "Unknown error";
        }
    }
    
    emit logMessage(error);
    m_statusLabel->setText(error);
}

void CustomImageWizard::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString("\nProcess failed with exit code: %1").arg(exitCode));
        m_statusLabel->setText("Build failed!");
        m_isBuilding = false;
        m_buildButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_buildProgress->setVisible(false);
        emit buildFinished(false);
        
        // Update failed step
        QString failed = "<span style='color: #FF0000;'>✗ Failed</span>";
        switch (m_currentStep) {
            case StepKernel:
                m_stepKernelLabel->setText("• Kernel: " + failed);
                break;
            case StepUBoot:
                m_stepUBootLabel->setText("• U-Boot: " + failed);
                break;
            case StepRootfs:
                m_stepRootfsLabel->setText("• Root filesystem: " + failed);
                break;
            case StepImage:
                m_stepImageLabel->setText("• Disk image: " + failed);
                break;
            default:
                break;
        }
        
        return;
    }
    
    // Continue to next step
    switch (m_currentStep) {
        case StepDownloading: {
            // Now clone the kernel repository
            m_currentStep = StepDownloadingKernel;
            m_downloadStatusLabel->setText("Cloning linux-rockchip kernel repository...");
            // Use the same branch name as suite for kernel repository
            QString suite = m_suiteCombo->currentText().split(" ").first();
            cloneRepository("https://github.com/Joshua-Riek/linux-rockchip.git", suite, m_kernelDir);
            break;
        }
        case StepDownloadingKernel:
            onDownloadFinished();
            break;
        case StepKernel:
            buildUBoot();
            break;
        case StepUBoot:
            buildRootfs();
            break;
        case StepRootfs:
            createImage();
            break;
        case StepImage:
            m_buildProgress->setValue(100);
            m_stepImageLabel->setText("• Disk image: <span style='color: #00FF00;'>✓ Completed</span>");
            emit logMessage("\n=====================================");
            emit logMessage("=== Build Complete! ===");
            emit logMessage("=====================================");
            emit logMessage("Image saved to: " + m_outputDir);
            emit logMessage("=====================================\n");
            m_statusLabel->setText("Build completed successfully!");
            m_isBuilding = false;
            m_buildButton->setEnabled(true);
            m_stopButton->setEnabled(false);
            emit buildFinished(true);
            
            QMessageBox::information(this, "Build Complete", 
                QString("Orange Pi 5 Plus image built successfully!\n\nOutput: %1").arg(m_outputDir));
            break;
        default:
            break;
    }
}

void CustomImageWizard::onSourceTypeChanged()
{
    validateInputs();
}

QString CustomImageWizard::getLivecdRootfsBranch(const QString &suite)
{
    // Map suite to correct livecd-rootfs branch based on analysis
    if (suite == "noble") return "main";
    if (suite == "plucky") return "upstream";
    if (suite == "jammy") return "jammy";
    if (suite == "oracular") return "oracular";
    return "main"; // default fallback
}

void CustomImageWizard::updateBuildStepDisplay(const QString &step, const QString &description)
{
    QString inProgress = "<span style='color: #FFA500;'>In progress... (" + description + ")</span>";
    QString completed = "<span style='color: #00FF00;'>✓ Completed</span>";
    
    // Update current step
    if (step.contains("Kernel", Qt::CaseInsensitive)) {
        m_stepKernelLabel->setText("• Kernel: " + inProgress);
    } else if (step.contains("U-Boot", Qt::CaseInsensitive)) {
        m_stepKernelLabel->setText("• Kernel: " + completed);
        m_stepUBootLabel->setText("• U-Boot: " + inProgress);
    } else if (step.contains("Rootfs", Qt::CaseInsensitive)) {
        m_stepKernelLabel->setText("• Kernel: " + completed);
        m_stepUBootLabel->setText("• U-Boot: " + completed);
        m_stepRootfsLabel->setText("• Root filesystem: " + inProgress);
    } else if (step.contains("Image", Qt::CaseInsensitive)) {
        m_stepKernelLabel->setText("• Kernel: " + completed);
        m_stepUBootLabel->setText("• U-Boot: " + completed);
        m_stepRootfsLabel->setText("• Root filesystem: " + completed);
        m_stepImageLabel->setText("• Disk image: " + inProgress);
    }
}

void CustomImageWizard::fetchBranches()
{
    // Already implemented in onRefreshBranchesClicked
}

void CustomImageWizard::downloadSource()
{
    // Implementation for downloading source
}