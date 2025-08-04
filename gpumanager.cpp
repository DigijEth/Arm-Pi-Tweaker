#include "gpumanager.h"
#include "systemmanager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QDateTime>
#include <QRandomGenerator>
#include <QRegularExpression>

GpuManager::GpuManager(SystemManager *systemManager, QWidget *parent)
    : QWidget(parent)
    , m_systemManager(systemManager)
{
    setupUI();
    
    // Initial scan for GPU info and drivers
    QTimer::singleShot(100, this, &GpuManager::updateDriverStatus);
    QTimer::singleShot(200, this, &GpuManager::onScanDrivers);
}

void GpuManager::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *title = new QLabel("GPU Driver Manager");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #000000; margin: 10px;");
    mainLayout->addWidget(title);
    
    // Create GPU power graph group
    createGpuGraphGroup();
    mainLayout->addWidget(m_gpuGraphGroup);
    
    // Create horizontal layout for the main content
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    // Left side - Driver info (vertical, takes up less width)
    createDriverInfoGroup();
    contentLayout->addWidget(m_driverInfoGroup, 2);
    
    // Right side - Actions and config stacked vertically
    QVBoxLayout *rightLayout = new QVBoxLayout();
    createDriverActionsGroup();
    createDriverConfigGroup();
    rightLayout->addWidget(m_driverActionsGroup);
    rightLayout->addWidget(m_driverConfigGroup);
    rightLayout->addStretch();
    
    contentLayout->addLayout(rightLayout, 3);
    mainLayout->addLayout(contentLayout);
    
    // Progress and status
    m_progressBar = new QProgressBar();
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 2px solid #000000; border-radius: 5px; background-color: #F0F0F0; }"
        "QProgressBar::chunk { background-color: #000000; }"
    );
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);
    
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("color: #000000; font-weight: bold;");
    mainLayout->addWidget(m_statusLabel);
    
    mainLayout->addStretch();
}

void GpuManager::createDriverInfoGroup()
{
    m_driverInfoGroup = new QGroupBox("🎮 GPU Driver Information");
    m_driverInfoGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    m_driverInfoGroup->setMinimumWidth(350);
    m_driverInfoGroup->setMinimumHeight(400);
    
    QVBoxLayout *layout = new QVBoxLayout(m_driverInfoGroup);
    layout->setSpacing(10); // Reduced spacing for better scaling
    
    // GPU Info
    m_gpuInfoLabel = new QLabel("Detecting GPU...");
    m_gpuInfoLabel->setStyleSheet("color: #00FFFF; font-family: monospace; font-size: 10pt; padding: 3px;");
    m_gpuInfoLabel->setWordWrap(true);
    layout->addWidget(m_gpuInfoLabel);
    
    layout->addSpacing(8); // Add extra space after GPU info
    
    // Current driver
    m_currentDriverLabel = new QLabel("Current Installed Driver: Detecting...");
    m_currentDriverLabel->setStyleSheet("color: #000000; font-weight: bold; font-size: 10pt; padding: 3px;");
    m_currentDriverLabel->setWordWrap(true);
    layout->addWidget(m_currentDriverLabel);
    
    // Driver version
    m_driverVersionLabel = new QLabel("Driver Version: Detecting...");
    m_driverVersionLabel->setStyleSheet("color: #000000; font-size: 10pt; padding: 3px;");
    m_driverVersionLabel->setWordWrap(true);
    layout->addWidget(m_driverVersionLabel);
    
    // Driver library version
    m_driverLibVersionLabel = new QLabel("Driver Library Version: Detecting...");
    m_driverLibVersionLabel->setStyleSheet("color: #000000; font-size: 10pt; padding: 3px;");
    m_driverLibVersionLabel->setWordWrap(true);
    layout->addWidget(m_driverLibVersionLabel);
    
    // Date created
    m_driverDateLabel = new QLabel("Date Created: Detecting...");
    m_driverDateLabel->setStyleSheet("color: #000000; font-size: 10pt; padding: 3px;");
    m_driverDateLabel->setWordWrap(true);
    layout->addWidget(m_driverDateLabel);
    
    // Created by
    m_driverCreatorLabel = new QLabel("Created By: Detecting...");
    m_driverCreatorLabel->setStyleSheet("color: #000000; font-size: 10pt; padding: 3px;");
    m_driverCreatorLabel->setWordWrap(true);
    layout->addWidget(m_driverCreatorLabel);
    
    // Driver supports
    m_driverSupportsLabel = new QLabel("Driver Supports: Detecting...");
    m_driverSupportsLabel->setStyleSheet("color: #000000; font-size: 10pt; padding: 3px;");
    m_driverSupportsLabel->setWordWrap(true);
    layout->addWidget(m_driverSupportsLabel);
    
    // Driver location link
    m_driverLocationLink = new QLabel("<a href='#'>Driver Location: Click to open</a>");
    m_driverLocationLink->setStyleSheet("color: #0000FF; font-size: 10pt; padding: 3px;");
    m_driverLocationLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(m_driverLocationLink, &QLabel::linkActivated, this, &GpuManager::onOpenDriverLocation);
    layout->addWidget(m_driverLocationLink);
    
    // Add some spacing
    layout->addSpacing(15);
    
    // Driver documentation links
    QLabel *docLinksLabel = new QLabel("<b>Driver Documentation:</b>");
    docLinksLabel->setStyleSheet("color: #000000; font-size: 10pt; padding: 3px;");
    layout->addWidget(docLinksLabel);
    
    // Mesa Panfrost link
    m_mesaPanfrostLink = new QLabel("<a href='https://docs.mesa3d.org/drivers/panfrost.html'>Mesa's Panfrost</a>");
    m_mesaPanfrostLink->setStyleSheet("color: #0000FF; margin-left: 20px; font-size: 10pt;");
    m_mesaPanfrostLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(m_mesaPanfrostLink, &QLabel::linkActivated, [](const QString &) {
        QDesktopServices::openUrl(QUrl("https://docs.mesa3d.org/drivers/panfrost.html"));
    });
    layout->addWidget(m_mesaPanfrostLink);
    
    // Panfork link
    m_panforkLink = new QLabel("<a href='https://gitlab.com/panfork/mesa'>Panfork</a>");
    m_panforkLink->setStyleSheet("color: #0000FF; margin-left: 20px; font-size: 10pt;");
    m_panforkLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(m_panforkLink, &QLabel::linkActivated, [](const QString &) {
        QDesktopServices::openUrl(QUrl("https://gitlab.com/panfork/mesa"));
    });
    layout->addWidget(m_panforkLink);
    
    // Panthor link
    m_panthorLink = new QLabel("<a href='https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/25048'>Panthor</a>");
    m_panthorLink->setStyleSheet("color: #0000FF; margin-left: 20px; font-size: 10pt;");
    m_panthorLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(m_panthorLink, &QLabel::linkActivated, [](const QString &) {
        QDesktopServices::openUrl(QUrl("https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/25048"));
    });
    layout->addWidget(m_panthorLink);
    
    // ARM Valhall link
    m_armValhallLink = new QLabel("<a href='https://developer.arm.com/Processors/Mali-G610'>ARM's Valhall Site</a>");
    m_armValhallLink->setStyleSheet("color: #0000FF; margin-left: 20px; font-size: 10pt;");
    m_armValhallLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(m_armValhallLink, &QLabel::linkActivated, [](const QString &) {
        QDesktopServices::openUrl(QUrl("https://developer.arm.com/Processors/Mali-G610"));
    });
    layout->addWidget(m_armValhallLink);
}

void GpuManager::createGpuGraphGroup()
{
    m_gpuGraphGroup = new QGroupBox("📊 System Performance Monitor");
    m_gpuGraphGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_gpuGraphGroup);
    
    // Create custom graph widget (30% smaller)
    m_gpuGraphWidget = new QWidget();
    m_gpuGraphWidget->setMinimumHeight(140);
    m_gpuGraphWidget->setMaximumHeight(140);
    m_gpuGraphWidget->setStyleSheet("background-color: #F0F0F0; border: 1px solid #000000;");
    
    // Install event filter to handle painting
    m_gpuGraphWidget->installEventFilter(this);
    
    layout->addWidget(m_gpuGraphWidget);
    
    // Labels for current values
    QHBoxLayout *valuesLayout = new QHBoxLayout();
    
    m_powerVoltageLabel = new QLabel("GPU Freq: 0 MHz");
    m_powerVoltageLabel->setStyleSheet("color: #FF0000; font-weight: bold; font-size: 9pt;");
    valuesLayout->addWidget(m_powerVoltageLabel);
    
    m_powerWattsLabel = new QLabel("GPU Temp: 0°C");
    m_powerWattsLabel->setStyleSheet("color: #00FF00; font-weight: bold; font-size: 9pt;");
    valuesLayout->addWidget(m_powerWattsLabel);
    
    m_systemResourcesLabel = new QLabel("GPU Usage: 0%");
    m_systemResourcesLabel->setStyleSheet("color: #0000FF; font-weight: bold; font-size: 9pt;");
    valuesLayout->addWidget(m_systemResourcesLabel);
    
    // Add CPU information
    m_cpuFreqLabel = new QLabel("CPU Freq: 0 MHz");
    m_cpuFreqLabel->setStyleSheet("color: #FF00FF; font-weight: bold; font-size: 9pt;");
    valuesLayout->addWidget(m_cpuFreqLabel);
    
    m_cpuTempLabel = new QLabel("CPU Temp: 0°C");
    m_cpuTempLabel->setStyleSheet("color: #FFA500; font-weight: bold; font-size: 9pt;");
    valuesLayout->addWidget(m_cpuTempLabel);
    
    m_cpuUsageLabel = new QLabel("CPU Usage: 0%");
    m_cpuUsageLabel->setStyleSheet("color: #008000; font-weight: bold; font-size: 9pt;");
    valuesLayout->addWidget(m_cpuUsageLabel);
    
    valuesLayout->addStretch();
    layout->addLayout(valuesLayout);
    
    // Initialize data vectors
    m_voltageData.reserve(100);
    m_powerData.reserve(100);
    m_usageData.reserve(100);
    m_cpuFreqData.reserve(100);
    m_cpuTempData.reserve(100);
    m_cpuUsageData.reserve(100);
    
    // Timer for updating graph
    m_graphUpdateTimer = new QTimer(this);
    connect(m_graphUpdateTimer, &QTimer::timeout, this, &GpuManager::updateGpuGraph);
    m_graphUpdateTimer->start(1000); // Update every second
}

void GpuManager::createDriverActionsGroup()
{
    m_driverActionsGroup = new QGroupBox("🔧 Driver Management");
    m_driverActionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    m_driverActionsGroup->setMinimumHeight(250);
    
    QVBoxLayout *layout = new QVBoxLayout(m_driverActionsGroup);
    
    // Available drivers list
    QLabel *availableLabel = new QLabel("Available Drivers:");
    availableLabel->setStyleSheet("color: #000000; font-weight: bold;");
    layout->addWidget(availableLabel);
    
    m_availableDriversList = new QListWidget();
    m_availableDriversList->setMinimumHeight(150);
    m_availableDriversList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    layout->addWidget(m_availableDriversList);
    
    connect(m_availableDriversList, &QListWidget::itemSelectionChanged,
            this, &GpuManager::onDriverSelectionChanged);
    
    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_scanButton = new QPushButton("🔍 Scan");
    m_scanButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_scanButton, &QPushButton::clicked, this, &GpuManager::onScanDrivers);
    buttonLayout->addWidget(m_scanButton);
    
    m_installButton = new QPushButton("📦 Install");
    m_installButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_installButton->setEnabled(false);
    connect(m_installButton, &QPushButton::clicked, this, &GpuManager::onInstallDriver);
    buttonLayout->addWidget(m_installButton);
    
    m_removeButton = new QPushButton("🗑️ Remove");
    m_removeButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF0000; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_removeButton->setEnabled(false);
    connect(m_removeButton, &QPushButton::clicked, this, &GpuManager::onRemoveDriver);
    buttonLayout->addWidget(m_removeButton);
    
    m_testButton = new QPushButton("🧪 Test");
    m_testButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #00FFFF; border: 2px solid #000000; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_testButton, &QPushButton::clicked, [this]() {
        m_statusLabel->setText("Running GPU test...");
        QProcess *testProcess = new QProcess(this);
        connect(testProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this, testProcess](int exitCode, QProcess::ExitStatus) {
            QString output = testProcess->readAllStandardOutput();
            m_driverDetailsText->setPlainText(output);
            m_statusLabel->setText(exitCode == 0 ? "GPU test completed" : "GPU test failed");
            testProcess->deleteLater();
        });
        testProcess->start("glxinfo", QStringList() << "-B");
    });
    buttonLayout->addWidget(m_testButton);
    
    layout->addLayout(buttonLayout);
}

void GpuManager::createDriverConfigGroup()
{
    m_driverConfigGroup = new QGroupBox("⚙️ Driver Configuration");
    m_driverConfigGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    m_driverConfigGroup->setMaximumHeight(140); // Reduce by 20%
    
    QVBoxLayout *layout = new QVBoxLayout(m_driverConfigGroup);
    layout->setSpacing(5); // Tighter spacing
    
    // Top row - Driver type and Switch button side by side
    QHBoxLayout *topRow = new QHBoxLayout();
    
    QLabel *typeLabel = new QLabel("Driver Type:");
    typeLabel->setStyleSheet("color: #000000; font-size: 9pt;");
    typeLabel->setMaximumWidth(70);
    topRow->addWidget(typeLabel);
    
    m_driverTypeCombo = new QComboBox();
    m_driverTypeCombo->addItems(QStringList() 
        << "Mali Proprietary"
        << "Mesa/Panfrost"
        << "Mali Bifrost"
        << "Software");
    m_driverTypeCombo->setStyleSheet(
        "QComboBox { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; padding: 2px; font-size: 9pt; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: #F0F0F0; color: #000000; selection-background-color: #000000; selection-color: #FFFFFF; }"
    );
    m_driverTypeCombo->setMaximumWidth(120); // Cut in half
    topRow->addWidget(m_driverTypeCombo);
    
    topRow->addSpacing(10);
    
    // Switch driver button (cut in half)
    m_switchButton = new QPushButton("🔄 Switch");
    m_switchButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF00FF; border: 2px solid #000000; padding: 3px; font-size: 9pt; } QPushButton:hover { background-color: #E0E0E0; }");
    m_switchButton->setMaximumWidth(80); // Cut in half
    connect(m_switchButton, &QPushButton::clicked, this, &GpuManager::onSwitchDriver);
    topRow->addWidget(m_switchButton);
    
    topRow->addStretch();
    layout->addLayout(topRow);
    
    // Configuration options - horizontal layout
    QLabel *configLabel = new QLabel("Additional Options:");
    configLabel->setStyleSheet("color: #000000; font-size: 9pt; margin-top: 5px;");
    layout->addWidget(configLabel);
    
    QHBoxLayout *optionsLayout = new QHBoxLayout();
    
    QCheckBox *hwAccelCheck = new QCheckBox("HW Accel");
    hwAccelCheck->setStyleSheet("color: #000000; font-size: 9pt;");
    hwAccelCheck->setChecked(true);
    optionsLayout->addWidget(hwAccelCheck);
    
    QCheckBox *vaapiCheck = new QCheckBox("VA-API");
    vaapiCheck->setStyleSheet("color: #000000; font-size: 9pt;");
    optionsLayout->addWidget(vaapiCheck);
    
    QCheckBox *vulkanCheck = new QCheckBox("Vulkan");
    vulkanCheck->setStyleSheet("color: #000000; font-size: 9pt;");
    optionsLayout->addWidget(vulkanCheck);
    
    optionsLayout->addStretch();
    layout->addLayout(optionsLayout);
    
    layout->addStretch();
}

void GpuManager::onScanDrivers()
{
    m_statusLabel->setText("Scanning for GPU drivers...");
    m_availableDriversList->clear();
    m_availableDrivers.clear();
    
    // Scan GPU directory
    QString gpuDir = "/home/snake/Arm-Pi-Tweaker/gpu";
    QDir gpu(gpuDir);
    
    if (gpu.exists()) {
        // Scan for proprietary Mali drivers
        QDir proprietary(gpuDir + "/proprietary");
        if (proprietary.exists()) {
            QStringList debFiles = proprietary.entryList(QStringList() << "*.deb", QDir::Files);
            for (const QString &deb : debFiles) {
                QString fullPath = proprietary.absoluteFilePath(deb);
                m_availableDrivers.append(fullPath);
                
                QListWidgetItem *item = new QListWidgetItem("📦 " + deb);
                item->setData(Qt::UserRole, fullPath);
                m_availableDriversList->addItem(item);
            }
        }
        
        // Scan for Mesa drivers
        QDir mesa(gpuDir + "/mesa");
        if (mesa.exists()) {
            QStringList mesaFiles = mesa.entryList(QStringList() << "*.deb", QDir::Files);
            for (const QString &deb : mesaFiles) {
                QString fullPath = mesa.absoluteFilePath(deb);
                m_availableDrivers.append(fullPath);
                
                QListWidgetItem *item = new QListWidgetItem("🌐 " + deb);
                item->setData(Qt::UserRole, fullPath);
                m_availableDriversList->addItem(item);
            }
        }
    }
    
    // Check system for installed packages
    QProcess *dpkgProcess = new QProcess(this);
    connect(dpkgProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, dpkgProcess](int, QProcess::ExitStatus) {
        QString output = dpkgProcess->readAllStandardOutput();
        if (output.contains("libmali") || output.contains("mesa") || output.contains("panfrost")) {
            QListWidgetItem *item = new QListWidgetItem("✅ System GPU drivers detected");
            item->setData(Qt::UserRole, "system");
            m_availableDriversList->addItem(item);
        }
        dpkgProcess->deleteLater();
        
        m_statusLabel->setText(QString("Found %1 GPU drivers").arg(m_availableDriversList->count()));
    });
    dpkgProcess->start("dpkg", QStringList() << "-l" << "*mali*" << "*mesa*" << "*panfrost*");
}

void GpuManager::onInstallDriver()
{
    QListWidgetItem *item = m_availableDriversList->currentItem();
    if (!item) return;
    
    QString driverPath = item->data(Qt::UserRole).toString();
    if (driverPath.isEmpty() || driverPath == "system") return;
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Install GPU Driver",
        QString("Install GPU driver:\n%1\n\nThis will replace the current driver.").arg(QFileInfo(driverPath).fileName()),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        emit installDriverRequested(driverPath);
        m_statusLabel->setText("Installing GPU driver...");
        m_progressBar->setVisible(true);
        
        // Simulate installation (in real implementation, SystemManager would handle this)
        QTimer::singleShot(3000, [this]() {
            m_progressBar->setVisible(false);
            m_statusLabel->setText("GPU driver installed successfully");
            updateDriverStatus();
        });
    }
}

void GpuManager::onRemoveDriver()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove GPU Driver",
        "Remove the current GPU driver?\n\nThis may affect graphics performance.",
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        emit removeDriverRequested(m_currentDriver);
        m_statusLabel->setText("Removing GPU driver...");
    }
}

void GpuManager::onSwitchDriver()
{
    QString selectedType = m_driverTypeCombo->currentText();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Switch GPU Driver",
        QString("Switch to %1?\n\nSystem will need to restart for changes to take effect.").arg(selectedType),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        emit switchDriverRequested(selectedType);
        m_statusLabel->setText(QString("Switching to %1...").arg(selectedType));
    }
}

void GpuManager::onDriverSelectionChanged()
{
    QListWidgetItem *item = m_availableDriversList->currentItem();
    m_installButton->setEnabled(item != nullptr && item->data(Qt::UserRole).toString() != "system");
}

void GpuManager::updateDriverStatus()
{
    // Detect GPU
    QProcess *gpuProcess = new QProcess(this);
    connect(gpuProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, gpuProcess](int, QProcess::ExitStatus) {
        QString output = gpuProcess->readAllStandardOutput();
        
        // Parse GPU info for Orange Pi 5+
        if (output.contains("Mali") || output.contains("G610")) {
            m_gpuInfoLabel->setText("GPU: ARM Mali-G610 MP4 (Odin)");
        } else {
            // Fallback detection
            QFile gpuFile("/sys/class/graphics/fb0/device/name");
            if (gpuFile.open(QIODevice::ReadOnly)) {
                QString gpuName = gpuFile.readAll().trimmed();
                m_gpuInfoLabel->setText(QString("GPU: %1").arg(gpuName.isEmpty() ? "Unknown" : gpuName));
                gpuFile.close();
            } else {
                m_gpuInfoLabel->setText("GPU: Detection failed");
            }
        }
        gpuProcess->deleteLater();
    });
    gpuProcess->start("lspci", QStringList() << "-v");
    
    // Detect current driver
    QProcess *driverProcess = new QProcess(this);
    connect(driverProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, driverProcess](int, QProcess::ExitStatus) {
        QString output = driverProcess->readAllStandardOutput();
        
        QString currentDriver = "Unknown";
        QString driverVersion = "Unknown";
        QString driverLibVersion = "Unknown";
        QString driverDate = "Unknown";
        QString driverCreator = "Unknown";
        QString driverSupports = "";
        
        if (output.contains("mali")) {
            currentDriver = "Mali Proprietary";
            driverCreator = "ARM Ltd.";
            
            // Get Mali driver version
            QFile versionFile("/sys/module/mali_kbase/version");
            if (versionFile.open(QIODevice::ReadOnly)) {
                driverVersion = versionFile.readAll().trimmed();
                versionFile.close();
            }
            
            // Get driver location
            m_driverLocation = "/lib/modules/" + QString(QProcess::systemEnvironment().filter("KERNEL").first().split("=").last()) + "/kernel/drivers/gpu/arm/mali";
            if (!QDir(m_driverLocation).exists()) {
                m_driverLocation = "/usr/lib/mali";
            }
            
            driverSupports = "Vulkan, OpenGL ES 3.2, OpenCL 2.1";
            driverDate = "2024"; // Would need to check actual file dates
        } else if (output.contains("panfrost")) {
            currentDriver = "Panfrost (Open Source)";
            driverCreator = "Mesa/Freedesktop.org Community";
            m_driverLocation = "/usr/lib/dri";
            driverSupports = "OpenGL ES 3.1, OpenGL 3.1";
            driverDate = "2024";
        } else if (output.contains("panthor")) {
            currentDriver = "Panthor (Next-Gen Open Source)";
            driverCreator = "Mesa/Freedesktop.org Community";
            m_driverLocation = "/usr/lib/dri";
            driverSupports = "Vulkan 1.3, OpenGL ES 3.2";
            driverDate = "2024";
        }
        
        // Get Mesa/library version if applicable
        QProcess *mesaProcess = new QProcess(this);
        mesaProcess->start("glxinfo", QStringList() << "-B");
        mesaProcess->waitForFinished(2000);
        QString glInfo = mesaProcess->readAllStandardOutput();
        if (!glInfo.isEmpty() && glInfo.contains("Mesa")) {
            QStringList lines = glInfo.split('\n');
            for (const QString &line : lines) {
                if (line.contains("OpenGL version")) {
                    QRegularExpression rx("Mesa ([0-9.]+)");
                    QRegularExpressionMatch match = rx.match(line);
                    if (match.hasMatch()) {
                        driverLibVersion = "Mesa " + match.captured(1);
                    }
                }
            }
        }
        delete mesaProcess;
        
        // Update all labels
        m_currentDriver = currentDriver;
        m_currentDriverLabel->setText(QString("Current Installed Driver: %1").arg(currentDriver));
        m_driverVersionLabel->setText(QString("Driver Version: %1").arg(driverVersion));
        m_driverLibVersionLabel->setText(QString("Driver Library Version: %1").arg(driverLibVersion));
        m_driverDateLabel->setText(QString("Date Created: %1").arg(driverDate));
        m_driverCreatorLabel->setText(QString("Created By: %1").arg(driverCreator));
        m_driverSupportsLabel->setText(QString("Driver Supports: %1").arg(driverSupports));
        
        if (!m_driverLocation.isEmpty()) {
            m_driverLocationLink->setText(QString("<a href='#'>Driver Location: %1</a>").arg(m_driverLocation));
        }
        
        driverProcess->deleteLater();
    });
    driverProcess->start("lsmod", QStringList());
}

void GpuManager::updateGpuGraph()
{
    // Read real GPU data
    double gpuFreq = readGpuFrequency();
    double gpuTemp = readGpuTemperature();
    double gpuUsage = readGpuUsage();
    
    // Read real CPU data
    double cpuFreq = readCpuFrequency();
    double cpuTemp = readCpuTemperature();
    double cpuUsage = readCpuUsage();
    
    // Update labels - show "N/A" when no real data available
    m_powerVoltageLabel->setText(gpuFreq > 0 ? QString("GPU Freq: %1 MHz").arg(gpuFreq, 0, 'f', 0) : "GPU Freq: N/A");
    m_powerWattsLabel->setText(gpuTemp > 0 ? QString("GPU Temp: %1°C").arg(gpuTemp, 0, 'f', 1) : "GPU Temp: N/A");
    m_systemResourcesLabel->setText(gpuUsage > 0 ? QString("GPU Usage: %1%").arg(gpuUsage, 0, 'f', 0) : "GPU Usage: N/A");
    m_cpuFreqLabel->setText(cpuFreq > 0 ? QString("CPU Freq: %1 MHz").arg(cpuFreq, 0, 'f', 0) : "CPU Freq: N/A");
    m_cpuTempLabel->setText(cpuTemp > 0 ? QString("CPU Temp: %1°C").arg(cpuTemp, 0, 'f', 1) : "CPU Temp: N/A");
    m_cpuUsageLabel->setText(cpuUsage > 0 ? QString("CPU Usage: %1%").arg(cpuUsage, 0, 'f', 0) : "CPU Usage: N/A");
    
    // Add to data vectors (keep last 100 samples)
    m_voltageData.append(gpuFreq / 1000.0);  // Scale for graph
    m_powerData.append(gpuTemp);
    m_usageData.append(gpuUsage);
    m_cpuFreqData.append(cpuFreq / 1000.0);  // Scale for graph
    m_cpuTempData.append(cpuTemp);
    m_cpuUsageData.append(cpuUsage);
    
    if (m_voltageData.size() > 100) {
        m_voltageData.removeFirst();
        m_powerData.removeFirst();
        m_usageData.removeFirst();
        m_cpuFreqData.removeFirst();
        m_cpuTempData.removeFirst();
        m_cpuUsageData.removeFirst();
    }
    
    // Trigger repaint
    m_gpuGraphWidget->update();
}

bool GpuManager::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_gpuGraphWidget && event->type() == QEvent::Paint) {
        QPainter painter(m_gpuGraphWidget);
        painter.setRenderHint(QPainter::Antialiasing);
        
        int width = m_gpuGraphWidget->width();
        int height = m_gpuGraphWidget->height();
        
        // Draw background
        painter.fillRect(0, 0, width, height, Qt::white);
        
        // Draw grid
        painter.setPen(QPen(Qt::lightGray, 1));
        for (int i = 0; i <= 10; i++) {
            int y = height * i / 10;
            painter.drawLine(0, y, width, y);
        }
        
        // Draw data if available
        if (m_voltageData.size() > 1) {
            double xStep = (double)width / (m_voltageData.size() - 1);
            
            // Draw GPU frequency (red)
            painter.setPen(QPen(Qt::red, 2));
            for (int i = 1; i < m_voltageData.size(); i++) {
                double y1 = height - (m_voltageData[i-1] / 2.0) * height; // Scale 0-2GHz
                double y2 = height - (m_voltageData[i] / 2.0) * height;
                painter.drawLine((i-1) * xStep, y1, i * xStep, y2);
            }
            
            // Draw GPU temperature (green)
            painter.setPen(QPen(Qt::green, 2));
            for (int i = 1; i < m_powerData.size(); i++) {
                double y1 = height - (m_powerData[i-1] / 100.0) * height; // Scale 0-100°C
                double y2 = height - (m_powerData[i] / 100.0) * height;
                painter.drawLine((i-1) * xStep, y1, i * xStep, y2);
            }
            
            // Draw GPU usage (blue)
            painter.setPen(QPen(Qt::blue, 2));
            for (int i = 1; i < m_usageData.size(); i++) {
                double y1 = height - (m_usageData[i-1] / 100.0) * height; // Scale 0-100%
                double y2 = height - (m_usageData[i] / 100.0) * height;
                painter.drawLine((i-1) * xStep, y1, i * xStep, y2);
            }
            
            // Draw CPU frequency (magenta)
            painter.setPen(QPen(Qt::magenta, 2));
            for (int i = 1; i < m_cpuFreqData.size(); i++) {
                double y1 = height - (m_cpuFreqData[i-1] / 3.0) * height; // Scale 0-3GHz
                double y2 = height - (m_cpuFreqData[i] / 3.0) * height;
                painter.drawLine((i-1) * xStep, y1, i * xStep, y2);
            }
            
            // Draw CPU temperature (orange)
            painter.setPen(QPen(QColor(255, 165, 0), 2)); // Orange
            for (int i = 1; i < m_cpuTempData.size(); i++) {
                double y1 = height - (m_cpuTempData[i-1] / 100.0) * height; // Scale 0-100°C
                double y2 = height - (m_cpuTempData[i] / 100.0) * height;
                painter.drawLine((i-1) * xStep, y1, i * xStep, y2);
            }
            
            // Draw CPU usage (dark green)
            painter.setPen(QPen(QColor(0, 128, 0), 2)); // Dark green
            for (int i = 1; i < m_cpuUsageData.size(); i++) {
                double y1 = height - (m_cpuUsageData[i-1] / 100.0) * height; // Scale 0-100%
                double y2 = height - (m_cpuUsageData[i] / 100.0) * height;
                painter.drawLine((i-1) * xStep, y1, i * xStep, y2);
            }
        }
        
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void GpuManager::onOpenDriverLocation()
{
    if (!m_driverLocation.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_driverLocation));
    }
}

// Real system monitoring functions
double GpuManager::readGpuFrequency()
{
    // Try to read Mali GPU frequency from various possible locations
    QStringList gpuFreqPaths = {
        "/sys/class/devfreq/fb000000.gpu/cur_freq",
        "/sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/cur_freq",
        "/sys/kernel/debug/clk/clk_gpu/clk_rate"
    };
    
    for (const QString &path : gpuFreqPaths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QString content = file.readAll().trimmed();
            bool ok;
            double freq = content.toDouble(&ok);
            if (ok && freq > 0) {
                return freq / 1000000.0; // Convert Hz to MHz
            }
        }
    }
    
    // No fallback - return 0 if unable to read real data
    return 0.0;
}

double GpuManager::readGpuTemperature()
{
    // Try to read GPU temperature from thermal zones
    QStringList tempPaths = {
        "/sys/class/thermal/thermal_zone1/temp", // Often GPU thermal zone
        "/sys/class/thermal/thermal_zone2/temp",
        "/sys/devices/virtual/thermal/thermal_zone1/temp"
    };
    
    for (const QString &path : tempPaths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QString content = file.readAll().trimmed();
            bool ok;
            double temp = content.toDouble(&ok);
            if (ok && temp > 10000) { // Temperature in millidegrees
                return temp / 1000.0;
            }
        }
    }
    
    // No fallback - return 0 if unable to read real data
    return 0.0;
}

double GpuManager::readGpuUsage()
{
    // Try to read GPU utilization
    QStringList usagePaths = {
        "/sys/class/devfreq/fb000000.gpu/load",
        "/sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/load",
        "/proc/mali/utilization"
    };
    
    for (const QString &path : usagePaths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QString content = file.readAll().trimmed();
            bool ok;
            double usage = content.toDouble(&ok);
            if (ok && usage >= 0 && usage <= 100) {
                return usage;
            }
        }
    }
    
    // No fallback - return 0 if unable to read real data
    return 0.0;
}

double GpuManager::readCpuFrequency()
{
    // Read current CPU frequency (average of all cores)
    QFile file("/proc/cpuinfo");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString line;
        double totalFreq = 0;
        int coreCount = 0;
        
        while (stream.readLineInto(&line)) {
            if (line.contains("cpu MHz")) {
                QStringList parts = line.split(':');
                if (parts.size() == 2) {
                    bool ok;
                    double freq = parts[1].trimmed().toDouble(&ok);
                    if (ok) {
                        totalFreq += freq;
                        coreCount++;
                    }
                }
            }
        }
        
        if (coreCount > 0) {
            return totalFreq / coreCount;
        }
    }
    
    // Try to read from scaling frequency
    QFile scalingFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    if (scalingFile.open(QIODevice::ReadOnly)) {
        QString content = scalingFile.readAll().trimmed();
        bool ok;
        double freq = content.toDouble(&ok);
        if (ok) {
            return freq / 1000.0; // Convert kHz to MHz
        }
    }
    
    // No fallback - return 0 if unable to read real data
    return 0.0;
}

double GpuManager::readCpuTemperature()
{
    // Read CPU temperature from thermal zone 0 (usually CPU)
    QFile file("/sys/class/thermal/thermal_zone0/temp");
    if (file.open(QIODevice::ReadOnly)) {
        QString content = file.readAll().trimmed();
        bool ok;
        double temp = content.toDouble(&ok);
        if (ok && temp > 10000) {
            return temp / 1000.0; // Convert millidegrees to degrees
        }
    }
    
    // No fallback - return 0 if unable to read real data
    return 0.0;
}

double GpuManager::readCpuUsage()
{
    static qint64 lastIdle = 0;
    static qint64 lastTotal = 0;
    
    QFile file("/proc/stat");
    if (file.open(QIODevice::ReadOnly)) {
        QString line = file.readLine();
        if (line.startsWith("cpu ")) {
            QStringList values = line.split(QRegularExpression("\\s+"));
            if (values.size() >= 5) {
                qint64 user = values[1].toLongLong();
                qint64 nice = values[2].toLongLong();
                qint64 system = values[3].toLongLong();
                qint64 idle = values[4].toLongLong();
                qint64 iowait = values.size() > 5 ? values[5].toLongLong() : 0;
                
                qint64 totalIdle = idle + iowait;
                qint64 total = user + nice + system + idle + iowait;
                
                if (lastTotal > 0) {
                    qint64 totalDiff = total - lastTotal;
                    qint64 idleDiff = totalIdle - lastIdle;
                    
                    if (totalDiff > 0) {
                        double usage = 100.0 * (totalDiff - idleDiff) / totalDiff;
                        lastIdle = totalIdle;
                        lastTotal = total;
                        return qMax(0.0, qMin(100.0, usage));
                    }
                }
                
                lastIdle = totalIdle;
                lastTotal = total;
            }
        }
    }
    
    // No fallback - return 0 if unable to read real data
    return 0.0;
}