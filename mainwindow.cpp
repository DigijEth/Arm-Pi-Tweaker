#include "mainwindow.h"
#include "widgets/upgradewidget.h"
#include "systemmanager.h"
#include "gpumanager.h"
#include "kernelmanager.h"
#include "storagemanager.h"
#include "widgets/customimagewizard.h"
#include "widgets/uefitab.h"
#include "widgets/rockytab.h"
#include "widgets/welcometab.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QFrame>
#include <QFont>
#include <QDialogButtonBox>
#include <QDialog>
#include <QLineEdit>
#include <QResizeEvent>
#include <QScreen>
#include <QButtonGroup>
#include <QDebug>
#include <QCheckBox>
#include <QTabBar>
#include <QTabWidget>
#include <QRadioButton>
#include <QComboBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_welcomeTab(nullptr)
    , m_upgradeWidget(nullptr)
    , m_statusOutput(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_gpuManager(nullptr)
    , m_kernelManager(nullptr)
    , m_storageManager(nullptr)
    , m_customImageWizard(nullptr)
    , m_uefiTabWidget(nullptr)
    , m_systemManager(nullptr)
    , m_rockyTab(nullptr)
{
    std::cout << "DEBUG: MainWindow constructor started" << std::endl;
    setWindowTitle("Arm-Pi Tweaker - Orange Pi 5+ Optimization Tool");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    std::cout << "DEBUG: Creating SystemManager" << std::endl;
    m_systemManager = new SystemManager(this);
    
    std::cout << "DEBUG: Calling setupUI" << std::endl;
    setupUI();
    std::cout << "DEBUG: setupUI completed" << std::endl;
    
    std::cout << "DEBUG: Calling setupMenuBar" << std::endl;
    setupMenuBar();
    std::cout << "DEBUG: setupMenuBar completed" << std::endl;
    
    // Apply initial dynamic sizing
    std::cout << "DEBUG: Calling updateDynamicSizes" << std::endl;
    updateDynamicSizes();
    std::cout << "DEBUG: MainWindow constructor completed" << std::endl;
}

// Slot stubs for ImageBuilder signals (no-op, handled in CustomImageWizard)
void MainWindow::handleRequestDownloadDir()
{
}

void MainWindow::handleRequestBuildConfirmation(const QString &kernelPath)
{
    Q_UNUSED(kernelPath);
}

MainWindow::~MainWindow()
{
    // Disconnect all signals to prevent crashes during destruction
    if (m_customImageWizard) {
        disconnect(m_customImageWizard, nullptr, this, nullptr);
    }
}

void MainWindow::showSettings()
{
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("ARM Pi Tweaker Settings");
    settingsDialog.setMinimumSize(720, 600);
    settingsDialog.resize(864, 720);
    settingsDialog.setSizeGripEnabled(true);
    settingsDialog.setStyleSheet("background-color: #DCDCDC;");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&settingsDialog);
    
    // Title
    QLabel *titleLabel = new QLabel("⚙️ ARM Pi Tweaker Settings");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #000000; margin: 15px; padding: 10px; background-color: #E8E8E8; border-radius: 8px;");
    mainLayout->addWidget(titleLabel);
    
    // Tab widget for different settings categories
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 2px solid #000000; background-color: #F0F0F0; }"
        "QTabWidget::tab-bar { alignment: center; }"
        "QTabBar::tab { background-color: #D0D0D0; color: #000000; padding: 8px 16px; margin: 2px; border: 1px solid #000000; }"
        "QTabBar::tab:selected { background-color: #F0F0F0; font-weight: bold; }"
        "QTabBar::tab:hover { background-color: #E0E0E0; }"
    );
    
    // ===== GENERAL SETTINGS TAB =====
    QWidget *generalPage = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalPage);
    
    // Default Directories Group
    QGroupBox *dirGroup = new QGroupBox("📁 Default Directories");
    dirGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 15px; background-color: #F8F8F8; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *dirLayout = new QVBoxLayout(dirGroup);
    
    // Kernel Directory Setting
    QLabel *kernelDirLabel = new QLabel("Default Kernel Directory:");
    kernelDirLabel->setStyleSheet("color: #000000; font-weight: bold;");
    dirLayout->addWidget(kernelDirLabel);
    
    QHBoxLayout *kernelDirEditLayout = new QHBoxLayout();
    QString kernelDir = m_kernelManager ? m_kernelManager->getKernelDirectory() : "/usr/src";
    QLineEdit *kernelDirEdit = new QLineEdit(kernelDir);
    kernelDirEdit->setStyleSheet("background-color: #FFFFFF; color: #000000; border: 1px solid #000000; padding: 5px;");
    kernelDirEditLayout->addWidget(kernelDirEdit);
    
    QPushButton *browseDirButton = new QPushButton("📁 Browse");
    browseDirButton->setStyleSheet("QPushButton { background-color: #E0E0E0; color: #000000; border: 2px solid #000000; padding: 5px 10px; } QPushButton:hover { background-color: #D0D0D0; }");
    connect(browseDirButton, &QPushButton::clicked, [&kernelDirEdit, this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Default Kernel Directory", kernelDirEdit->text());
        if (!dir.isEmpty()) {
            kernelDirEdit->setText(dir);
        }
    });
    kernelDirEditLayout->addWidget(browseDirButton);
    dirLayout->addLayout(kernelDirEditLayout);
    
    generalLayout->addWidget(dirGroup);
    generalLayout->addStretch();
    tabWidget->addTab(generalPage, "General");
    
    // ===== SECURITY SETTINGS TAB =====
    QWidget *securityPage = new QWidget();
    QVBoxLayout *securityLayout = new QVBoxLayout(securityPage);
    
    // Remote Connection Security Group
    QGroupBox *remoteGroup = new QGroupBox("🔒 Remote Connection Security");
    remoteGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 15px; background-color: #F8F8F8; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *remoteLayout = new QVBoxLayout(remoteGroup);
    
    // Enable Remote Features checkbox
    QCheckBox *enableRemoteCheck = new QCheckBox("Enable Remote Features");
    enableRemoteCheck->setStyleSheet("color: #000000; font-weight: bold; margin: 5px;");
    enableRemoteCheck->setChecked(false);
    remoteLayout->addWidget(enableRemoteCheck);
    
    // Encryption settings (initially disabled)
    QLabel *encryptionLabel = new QLabel("Encryption Method:");
    encryptionLabel->setStyleSheet("color: #000000; margin-left: 20px;");
    encryptionLabel->setEnabled(false);
    remoteLayout->addWidget(encryptionLabel);
    
    QRadioButton *aesBtn = new QRadioButton("AES-256-GCM (Recommended)");
    aesBtn->setStyleSheet("color: #000000; margin-left: 30px;");
    aesBtn->setEnabled(false);
    aesBtn->setChecked(true);
    remoteLayout->addWidget(aesBtn);
    
    QRadioButton *chachaBtn = new QRadioButton("ChaCha20-Poly1305");
    chachaBtn->setStyleSheet("color: #000000; margin-left: 30px;");
    chachaBtn->setEnabled(false);
    remoteLayout->addWidget(chachaBtn);
    
    // Connect enable/disable logic
    connect(enableRemoteCheck, &QCheckBox::toggled, [=](bool checked) {
        encryptionLabel->setEnabled(checked);
        aesBtn->setEnabled(checked);
        chachaBtn->setEnabled(checked);
    });
    
    securityLayout->addWidget(remoteGroup);
    securityLayout->addStretch();
    tabWidget->addTab(securityPage, "Security");
    
    // ===== API SETTINGS TAB =====
    QWidget *apiPage = new QWidget();
    QVBoxLayout *apiLayout = new QVBoxLayout(apiPage);
    
    // GitHub API Token Group
    QGroupBox *githubGroup = new QGroupBox("🐙 GitHub API Configuration");
    githubGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 15px; background-color: #F8F8F8; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *githubLayout = new QVBoxLayout(githubGroup);
    
    // GitHub Token input
    QLabel *tokenLabel = new QLabel("GitHub API Token:");
    tokenLabel->setStyleSheet("color: #000000; font-weight: bold;");
    githubLayout->addWidget(tokenLabel);
    
    QLineEdit *tokenEdit = new QLineEdit();
    tokenEdit->setEchoMode(QLineEdit::Password);
    tokenEdit->setPlaceholderText("Enter your GitHub personal access token");
    tokenEdit->setStyleSheet("background-color: #FFFFFF; color: #000000; border: 1px solid #000000; padding: 5px;");
    githubLayout->addWidget(tokenEdit);
    
    // Custom API settings
    QCheckBox *customApiCheck = new QCheckBox("Use Custom API Settings");
    customApiCheck->setStyleSheet("color: #000000; font-weight: bold; margin: 10px 0;");
    githubLayout->addWidget(customApiCheck);
    
    // .env file selection (initially disabled)
    QHBoxLayout *envLayout = new QHBoxLayout();
    QLabel *envLabel = new QLabel("Environment File (.env):");
    envLabel->setStyleSheet("color: #000000; margin-left: 20px;");
    envLabel->setEnabled(false);
    envLayout->addWidget(envLabel);
    
    QLineEdit *envPathEdit = new QLineEdit();
    envPathEdit->setPlaceholderText("Select .env file for custom API configuration");
    envPathEdit->setStyleSheet("background-color: #FFFFFF; color: #000000; border: 1px solid #000000; padding: 5px;");
    envPathEdit->setEnabled(false);
    envLayout->addWidget(envPathEdit);
    
    QPushButton *browseEnvButton = new QPushButton("📄 Browse");
    browseEnvButton->setStyleSheet("QPushButton { background-color: #E0E0E0; color: #000000; border: 2px solid #000000; padding: 5px 10px; } QPushButton:hover { background-color: #D0D0D0; }");
    browseEnvButton->setEnabled(false);
    connect(browseEnvButton, &QPushButton::clicked, [&envPathEdit, this]() {
        QString envFile = QFileDialog::getOpenFileName(this, "Select Environment File", "", "Environment Files (*.env);;All Files (*)");
        if (!envFile.isEmpty()) {
            envPathEdit->setText(envFile);
        }
    });
    envLayout->addWidget(browseEnvButton);
    
    githubLayout->addLayout(envLayout);
    
    // Connect custom API checkbox logic
    connect(customApiCheck, &QCheckBox::toggled, [=](bool checked) {
        envLabel->setEnabled(checked);
        envPathEdit->setEnabled(checked);
        browseEnvButton->setEnabled(checked);
        tokenEdit->setEnabled(!checked);
        tokenLabel->setEnabled(!checked);
    });
    
    apiLayout->addWidget(githubGroup);
    apiLayout->addStretch();
    tabWidget->addTab(apiPage, "API Tokens");
    
    mainLayout->addWidget(tabWidget);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    buttonBox->setStyleSheet(
        "QPushButton { background-color: #E0E0E0; color: #000000; border: 2px solid #000000; padding: 8px 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #D0D0D0; }"
        "QPushButton:pressed { background-color: #C0C0C0; }"
    );
    
    connect(buttonBox, &QDialogButtonBox::accepted, [&settingsDialog, &kernelDirEdit, &tokenEdit, &envPathEdit, &customApiCheck, this]() {
        // Save settings
        if (m_kernelManager) {
            m_kernelManager->setKernelDirectory(kernelDirEdit->text());
        }
        
        // Save GitHub token settings
        if (customApiCheck->isChecked() && !envPathEdit->text().isEmpty()) {
            // TODO: Implement .env file loading
            QMessageBox::information(&settingsDialog, "Custom API Settings", 
                "Custom .env file settings will be implemented in the next version.\n\n"
                "Selected file: " + envPathEdit->text());
        } else if (!tokenEdit->text().isEmpty()) {
            m_githubToken = tokenEdit->text();
        }
        
        settingsDialog.accept();
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, &settingsDialog, &QDialog::reject);
    
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, [&kernelDirEdit, &tokenEdit, this]() {
        // Apply settings without closing dialog
        if (m_kernelManager) {
            m_kernelManager->setKernelDirectory(kernelDirEdit->text());
        }
        if (!tokenEdit->text().isEmpty()) {
            m_githubToken = tokenEdit->text();
        }
        QMessageBox::information(this, "Settings Applied", "Settings have been applied successfully!");
    });
    
    mainLayout->addWidget(buttonBox);
    
    settingsDialog.exec();
}

void MainWindow::setupUI()
{
    std::cout << "DEBUG: setupUI() started" << std::endl;
    
    std::cout << "DEBUG: Creating QTabWidget" << std::endl;
    m_tabWidget = new QTabWidget(this);
    
    std::cout << "DEBUG: Setting central widget" << std::endl;
    setCentralWidget(m_tabWidget);
    
    std::cout << "DEBUG: Calling setupTabs" << std::endl;
    setupTabs();
    std::cout << "DEBUG: setupTabs completed" << std::endl;
    
    // Status bar
    std::cout << "DEBUG: Setting status bar message" << std::endl;
    statusBar()->showMessage("Ready - Orange Pi 5+ Tweaker");
    
    // Set initial tab
    std::cout << "DEBUG: Setting current tab index" << std::endl;
    m_tabWidget->setCurrentIndex(0);
    
    std::cout << "DEBUG: setupUI() completed" << std::endl;
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // Settings Menu
    QMenu *settingsMenu = menuBar->addMenu("&Settings");
    QAction *systemTweaksAction = settingsMenu->addAction("System &Tweaks");
    QAction *preferencesAction = settingsMenu->addAction("&Preferences");
    settingsMenu->addSeparator();
    QAction *exitAction = settingsMenu->addAction("E&xit");
    
    connect(systemTweaksAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(7); });
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::showSettings);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // Tools Menu (All widgets accessible here)
    QMenu *toolsMenu = menuBar->addMenu("&Tools");
    QAction *welcomeAction = toolsMenu->addAction("&Welcome");
    QAction *upgradeAction = toolsMenu->addAction("Ubuntu &Upgrade");
    QAction *imageEditorAction = toolsMenu->addAction("&Image Builder");
    QAction *kernelManagerAction = toolsMenu->addAction("&Kernel Manager");
    QAction *gpuManagerAction = toolsMenu->addAction("&GPU Manager");
    QAction *storageManagerAction = toolsMenu->addAction("&Storage Manager");
    QAction *uefiManagerAction = toolsMenu->addAction("UE&FI Manager");
    QAction *rockyAction = toolsMenu->addAction("&Rocky AI");
    
    connect(welcomeAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(0); });
    connect(upgradeAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(1); });
    connect(imageEditorAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(2); });
    connect(kernelManagerAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(3); });
    connect(gpuManagerAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(4); });
    connect(storageManagerAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(5); });
    connect(uefiManagerAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(6); });
    connect(rockyAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(9); });
    
    // View Menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    QAction *hideTabsAction = viewMenu->addAction("&Hide Tabs");
    hideTabsAction->setCheckable(true);
    hideTabsAction->setChecked(false);
    viewMenu->addSeparator();
    QAction *statusLogAction = viewMenu->addAction("Status &Log");
    QAction *refreshAction = viewMenu->addAction("&Refresh");
    
    connect(hideTabsAction, &QAction::toggled, this, &MainWindow::toggleTabsVisibility);
    connect(statusLogAction, &QAction::triggered, [this]() { m_tabWidget->setCurrentIndex(8); });
    
    // Help Menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("&About");
    QAction *docsAction = helpMenu->addAction("&Documentation");
    
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupTabs()
{
    std::cout << "DEBUG: setupTabs() started" << std::endl;
    
    std::cout << "DEBUG: Setting up Welcome tab" << std::endl;
    setupWelcomeTab();
    
    std::cout << "DEBUG: Setting up Upgrade tab" << std::endl;
    setupUpgradeTab();
    
    std::cout << "DEBUG: Setting up Image Editor tab" << std::endl;
    setupImageEditorTab();
    
    std::cout << "DEBUG: Setting up Kernel Manager tab" << std::endl;
    setupKernelManagerTab();
    
    std::cout << "DEBUG: Setting up GPU Manager tab" << std::endl;
    setupGpuManagerTab();
    
    std::cout << "DEBUG: Setting up Storage tab" << std::endl;
    setupStorageTab();
    
    std::cout << "DEBUG: Setting up UEFI tab" << std::endl;
    setupUefiTab();
    
    std::cout << "DEBUG: Setting up Rocky tab" << std::endl;
    setupRockyTab();
    
    std::cout << "DEBUG: Setting up System Tweaks tab" << std::endl;
    setupSystemTweaksTab();
    
    std::cout << "DEBUG: Setting up Status tab" << std::endl;
    setupStatusTab();
    
    std::cout << "DEBUG: setupTabs() completed" << std::endl;
}

void MainWindow::setupWelcomeTab()
{
    std::cout << "DEBUG: setupWelcomeTab() started" << std::endl;
    
    m_welcomeTab = new WelcomeTab();
    m_tabWidget->addTab(m_welcomeTab, "🏠 Welcome");
    
    std::cout << "DEBUG: setupWelcomeTab() completed" << std::endl;
}

void MainWindow::setupUpgradeTab()
{
    std::cout << "DEBUG: setupUpgradeTab() started" << std::endl;
    
    std::cout << "DEBUG: Creating upgrade tab widget" << std::endl;
    m_upgradeTab = new QWidget();
    
    std::cout << "DEBUG: Adding tab to tabWidget" << std::endl;
    m_tabWidget->addTab(m_upgradeTab, "🔄 Ubuntu Upgrade");
    
    std::cout << "DEBUG: Creating layout for upgrade tab" << std::endl;
    QVBoxLayout *layout = new QVBoxLayout(m_upgradeTab);
    
    // Title
    std::cout << "DEBUG: Creating title label" << std::endl;
    QLabel *title = new QLabel("Ubuntu Upgrade Mode");
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #000000; margin: 10px;");
    
    std::cout << "DEBUG: Adding title to layout" << std::endl;
    layout->addWidget(title);
    
    // Description
    std::cout << "DEBUG: Creating description label" << std::endl;
    QLabel *description = new QLabel(
        "Upgrade Ubuntu 22.04 to 24.10 with Orange Pi 5+ drivers and kernel support.\n"
        "First extract the correct drivers from the mounted upgrade.img, then run the upgrade."
    );
    description->setWordWrap(true);
    description->setStyleSheet("margin: 5px; padding: 10px;");
    
    std::cout << "DEBUG: Adding description to layout" << std::endl;
    layout->addWidget(description);
    
    // Create upgrade widget
    std::cout << "DEBUG: Creating UpgradeWidget" << std::endl;
    m_upgradeWidget = new UpgradeWidget(this);
    
    std::cout << "DEBUG: Adding UpgradeWidget to layout" << std::endl;
    layout->addWidget(m_upgradeWidget);
    
    std::cout << "DEBUG: setupUpgradeTab() completed" << std::endl;
    
    // Connect signals
    connect(m_upgradeWidget, &UpgradeWidget::extractDriversRequested, this, &MainWindow::onExtractDrivers);
    connect(m_upgradeWidget, &UpgradeWidget::runUpgradeRequested, this, &MainWindow::onRunUpgrade);
    connect(m_upgradeWidget, &UpgradeWidget::patchSystemRequested, this, &MainWindow::onPatchSystem);
    connect(m_upgradeWidget, &UpgradeWidget::rollbackRequested, this, &MainWindow::onRollbackUpgrade);
    
    // Connect system manager signals to upgrade widget
    connect(m_systemManager, &SystemManager::progressUpdated, m_upgradeWidget, &UpgradeWidget::updateProgress);
    connect(m_systemManager, &SystemManager::statusUpdated, m_upgradeWidget, &UpgradeWidget::updateStatus);
    connect(m_systemManager, &SystemManager::operationCompleted, [this](bool success, const QString &message) {
        m_upgradeWidget->setButtonsEnabled(true);
        statusBar()->showMessage(success ? "Operation completed successfully" : "Operation failed");
    });
}

void MainWindow::setupImageEditorTab()
{
    m_imageEditorTab = new QWidget();
    m_tabWidget->addTab(m_imageEditorTab, "🖼️ Image Builder");
    
    QVBoxLayout *layout = new QVBoxLayout(m_imageEditorTab);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create and add the custom image wizard
    m_customImageWizard = new CustomImageWizard(m_imageEditorTab);
    layout->addWidget(m_customImageWizard);
    
    // Connect signals from the wizard
    connect(m_customImageWizard, &CustomImageWizard::logMessage, this, &MainWindow::onImageBuilderLogMessage);
    connect(m_customImageWizard, &CustomImageWizard::switchToStatusTab, this, &MainWindow::onSwitchToStatusTab);
    
    connect(m_customImageWizard, &CustomImageWizard::buildFinished, [this](bool success) {
        if (success) {
            statusBar()->showMessage("Image build completed successfully!", 10000);
        } else {
            statusBar()->showMessage("Image build failed. Check the log for details.", 10000);
        }
    });
}

void MainWindow::setupKernelManagerTab()
{
    m_kernelManagerTab = new QWidget();
    m_tabWidget->addTab(m_kernelManagerTab, "🐧 Kernel Manager");
    
    QVBoxLayout *layout = new QVBoxLayout(m_kernelManagerTab);
    
    // Create the actual Kernel Manager widget
    m_kernelManager = new KernelManager(m_systemManager, this);
    layout->addWidget(m_kernelManager);
}

void MainWindow::setupUefiTab()
{
    m_uefiTab = new QWidget();
    m_tabWidget->addTab(m_uefiTab, "🔧 UEFI Manager");
    
    QVBoxLayout *layout = new QVBoxLayout(m_uefiTab);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create the UEFI tab widget
    m_uefiTabWidget = new UefiTab(m_uefiTab);
    layout->addWidget(m_uefiTabWidget);
}

void MainWindow::setupSystemTweaksTab()
{
    m_systemTweaksTab = new QWidget();
    m_tabWidget->addTab(m_systemTweaksTab, "⚙️ System Tweaks");
    
    QVBoxLayout *layout = new QVBoxLayout(m_systemTweaksTab);
    
    QLabel *title = new QLabel("System Tweaks & Configuration");
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #000000; margin: 10px;");
    layout->addWidget(title);
    
    QLabel *placeholder = new QLabel("System Tweaks functionality will be implemented here.");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #666; font-size: 14px; margin: 50px;");
    layout->addWidget(placeholder);
}

void MainWindow::setupRockyTab()
{
    std::cout << "DEBUG: setupRockyTab() started" << std::endl;
    
    m_rockyTab = new RockyTab();
    m_tabWidget->addTab(m_rockyTab, "🤖 Rocky");
    
    std::cout << "DEBUG: setupRockyTab() completed" << std::endl;
}

void MainWindow::setupStatusTab()
{
    m_statusTab = new QWidget();
    m_tabWidget->addTab(m_statusTab, "📋 Status Log");
    
    QVBoxLayout *layout = new QVBoxLayout(m_statusTab);
    
    QLabel *title = new QLabel("System Status & Logs");
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #000000; margin: 10px;");
    layout->addWidget(title);
    
    m_statusOutput = new QTextEdit();
    m_statusOutput->setReadOnly(true);
    m_statusOutput->setFont(QFont("monospace", 9));
    m_statusOutput->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #ffffff; }");
    layout->addWidget(m_statusOutput);
    
    // Add clear button
    QPushButton *clearButton = new QPushButton("Clear Log");
    clearButton->setMaximumWidth(100);
    connect(clearButton, &QPushButton::clicked, [this]() {
        m_statusOutput->clear();
    });
    layout->addWidget(clearButton, 0, Qt::AlignRight);
}

void MainWindow::onExtractDrivers()
{
    statusBar()->showMessage("Extracting Orange Pi 5+ drivers and kernel...");
    m_upgradeWidget->setButtonsEnabled(false);
    m_systemManager->extractDrivers();
}

void MainWindow::onRunUpgrade()
{
    statusBar()->showMessage("Running Ubuntu upgrade to 24.10...");
    m_upgradeWidget->setButtonsEnabled(false);
    m_systemManager->runUbuntuUpgrade();
}

void MainWindow::onPatchSystem()
{
    statusBar()->showMessage("Patching system with Orange Pi 5+ support...");
    m_upgradeWidget->setButtonsEnabled(false);
    m_systemManager->patchSystem();
}

void MainWindow::onRollbackUpgrade()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Rollback",
        "Are you sure you want to rollback the upgrade? This will restore the previous system state.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        statusBar()->showMessage("Rolling back upgrade...");
        m_upgradeWidget->setButtonsEnabled(false);
        m_systemManager->rollbackUpgrade();
    }
}

void MainWindow::setupGpuManagerTab()
{
    m_gpuManagerTab = new QWidget();
    m_tabWidget->addTab(m_gpuManagerTab, "🎮 GPU Manager");
    
    QVBoxLayout *layout = new QVBoxLayout(m_gpuManagerTab);
    
    // Create the actual GPU Manager widget
    m_gpuManager = new GpuManager(m_systemManager, this);
    layout->addWidget(m_gpuManager);
    
    // Connect GPU Manager signals to SystemManager
    connect(m_gpuManager, &GpuManager::installDriverRequested, 
            m_systemManager, &SystemManager::installGpuDriver);
    connect(m_gpuManager, &GpuManager::removeDriverRequested, 
            m_systemManager, &SystemManager::removeGpuDriver);
    connect(m_gpuManager, &GpuManager::switchDriverRequested, 
            m_systemManager, &SystemManager::switchGpuDriver);
}

void MainWindow::setupStorageTab()
{
    m_storageTab = new QWidget();
    m_tabWidget->addTab(m_storageTab, "💾 Storage Manager");
    
    QVBoxLayout *layout = new QVBoxLayout(m_storageTab);
    
    // Create the actual Storage Manager widget
    m_storageManager = new StorageManager(m_systemManager, this);
    layout->addWidget(m_storageManager);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About Arm-Pi Tweaker",
        "<h3>Arm-Pi Tweaker v0.1.0</h3>"
        "<p>Advanced GUI tool for Orange Pi 5+ optimization and Linux image editing.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Ubuntu distribution upgrade (22.04 → 24.10)</li>"
        "<li>Kernel management and optimization</li>"
        "<li>Hardware acceleration configuration</li>"
        "<li>Live Linux image editing</li>"
        "</ul>"
        "<p>© 2024 Setec Labs</p>"
    );
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateDynamicSizes();
}

void MainWindow::updateDynamicSizes()
{
    // Update font sizes for all tabs based on window size
    if (m_upgradeWidget) {
        m_upgradeWidget->setStyleSheet(QString(
            "QLabel { font-size: %1pt; }"
            "QPushButton { font-size: %2pt; }"
            "QGroupBox { font-size: %2pt; }"
        ).arg(calculateFontSize(10)).arg(calculateFontSize(9)));
    }
    
    // Update GPU Manager font sizes
    if (m_gpuManager) {
        m_gpuManager->setStyleSheet(QString(
            "QLabel { font-size: %1pt; }"
            "QPushButton { font-size: %2pt; }"
            "QGroupBox { font-size: %2pt; }"
        ).arg(calculateFontSize(10)).arg(calculateFontSize(9)));
    }
    
    // Update tab widget font
    m_tabWidget->setStyleSheet(QString(
        "QTabBar::tab { font-size: %1pt; }"
    ).arg(calculateFontSize(10)));
}

int MainWindow::calculateFontSize(int baseSize)
{
    // Get current window size
    QSize windowSize = size();
    
    // Get screen size for reference
    QScreen *screen = QApplication::primaryScreen();
    QSize screenSize = screen->size();
    
    // Calculate scaling factor based on window width (minimum 800, reference 1200)
    double scaleFactor = qBound(0.8, (double)windowSize.width() / 1200.0, 1.5);
    
    // Apply additional scaling for very large screens
    if (screenSize.width() > 2560) {
        scaleFactor *= 1.2;
    }
    
    return qRound(baseSize * scaleFactor);
}

void MainWindow::onImageBuilderLogMessage(const QString &message)
{
    if (m_statusOutput) {
        m_statusOutput->append(message);
        
        // Auto-scroll to bottom
        QTextCursor cursor = m_statusOutput->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_statusOutput->setTextCursor(cursor);
    }
}

void MainWindow::onSwitchToStatusTab()
{
    // Find the status tab index and switch to it
    for (int i = 0; i < m_tabWidget->count(); i++) {
        if (m_tabWidget->widget(i) == m_statusTab) {
            m_tabWidget->setCurrentIndex(i);
            break;
        }
    }
    
    // Clear previous log content
    if (m_statusOutput) {
        m_statusOutput->clear();
    }
}

void MainWindow::toggleTabsVisibility(bool visible)
{
    if (m_tabWidget) {
        m_tabWidget->tabBar()->setVisible(!visible);
    }
}

void MainWindow::showAllWidgetsMenu()
{
    // This is handled by the Tools menu
}


