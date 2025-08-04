#include "kernelmanager.h"
#include "systemmanager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSplitter>
#include <QDialog>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QRadioButton>
#include <QFileInfo>
#include <QDateTime>

KernelManager::KernelManager(SystemManager *systemManager, QWidget *parent)
    : QWidget(parent)
    , m_systemManager(systemManager)
{
    // Initialize kernel directory to ~/tweaker/kernel
    m_kernelDirectory = QDir::homePath() + "/tweaker/kernel";
    
    setupUI();
    
    // Initial refresh of kernel data
    QTimer::singleShot(100, this, &KernelManager::onRefreshKernels);
    QTimer::singleShot(200, this, &KernelManager::onRefreshModules);
}

void KernelManager::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *title = new QLabel("Kernel Manager");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: #000000; margin: 10px;");
    mainLayout->addWidget(title);
    
    // Create tab widget for different kernel management functions
    m_tabWidget = new QTabWidget();
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 2px solid #000000; background-color: #DCDCDC; }"
        "QTabBar::tab { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; padding: 8px; }"
        "QTabBar::tab:selected { background-color: #000000; color: #FFFFFF; }"
    );
    
    createKernelManagementTab();
    createPatchingTab();
    createLiveConfigTab();
    createModuleManagementTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Status label
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("color: #000000; font-weight: bold; margin: 5px;");
    mainLayout->addWidget(m_statusLabel);
}

void KernelManager::createKernelManagementTab()
{
    QWidget *tab = new QWidget();
    m_tabWidget->addTab(tab, "🐧 Kernel Management");
    
    QHBoxLayout *layout = new QHBoxLayout(tab);
    
    // Left side - Kernel list and details
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    m_kernelListGroup = new QGroupBox("Installed Kernels");
    m_kernelListGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *kernelListLayout = new QVBoxLayout(m_kernelListGroup);
    
    m_currentKernelLabel = new QLabel("Current: Detecting...");
    m_currentKernelLabel->setStyleSheet("color: #000000; font-weight: bold;");
    kernelListLayout->addWidget(m_currentKernelLabel);
    
    m_defaultKernelLabel = new QLabel("Default: Detecting...");
    m_defaultKernelLabel->setStyleSheet("color: #000000;");
    kernelListLayout->addWidget(m_defaultKernelLabel);
    
    // Add kernel directory controls
    QLabel *kernelDirLabel = new QLabel("Kernel Directory:");
    kernelDirLabel->setStyleSheet("color: #000000; margin-top: 10px;");
    kernelListLayout->addWidget(kernelDirLabel);
    
    QHBoxLayout *kernelDirLayout = new QHBoxLayout();
    m_kernelDirectoryEdit = new QLineEdit(m_kernelDirectory);
    m_kernelDirectoryEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    connect(m_kernelDirectoryEdit, &QLineEdit::returnPressed, this, &KernelManager::onRefreshKernels);
    kernelDirLayout->addWidget(m_kernelDirectoryEdit);
    
    m_browseKernelDirButton = new QPushButton("📁 Browse");
    m_browseKernelDirButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 4px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_browseKernelDirButton, &QPushButton::clicked, this, &KernelManager::onBrowseKernelDirectory);
    kernelDirLayout->addWidget(m_browseKernelDirButton);
    
    kernelListLayout->addLayout(kernelDirLayout);
    
    m_copyCurrentKernelButton = new QPushButton("📋 Copy Current Kernel");
    m_copyCurrentKernelButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; margin-top: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_copyCurrentKernelButton, &QPushButton::clicked, this, &KernelManager::onCopyCurrentKernel);
    kernelListLayout->addWidget(m_copyCurrentKernelButton);
    
    m_backupKernelButton = new QPushButton("💾 Back Up Kernel");
    m_backupKernelButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; margin-top: 5px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_backupKernelButton->setEnabled(false);
    connect(m_backupKernelButton, &QPushButton::clicked, this, &KernelManager::onBackupKernel);
    kernelListLayout->addWidget(m_backupKernelButton);
    
    m_kernelList = new QListWidget();
    m_kernelList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    connect(m_kernelList, &QListWidget::itemSelectionChanged,
            this, &KernelManager::onKernelSelectionChanged);
    kernelListLayout->addWidget(m_kernelList);
    
    leftLayout->addWidget(m_kernelListGroup);
    
    // Kernel details
    m_kernelDetailsGroup = new QGroupBox("Kernel Details");
    m_kernelDetailsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *detailsLayout = new QVBoxLayout(m_kernelDetailsGroup);
    m_kernelDetailsText = new QTextEdit();
    m_kernelDetailsText->setReadOnly(true);
    m_kernelDetailsText->setMaximumHeight(150);
    m_kernelDetailsText->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    detailsLayout->addWidget(m_kernelDetailsText);
    
    leftLayout->addWidget(m_kernelDetailsGroup);
    
    // Right side - Actions
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    m_kernelActionsGroup = new QGroupBox("Kernel Actions");
    m_kernelActionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *actionsLayout = new QVBoxLayout(m_kernelActionsGroup);
    
    m_refreshButton = new QPushButton("🔄 Refresh Kernels");
    m_refreshButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_refreshButton, &QPushButton::clicked, this, &KernelManager::onRefreshKernels);
    actionsLayout->addWidget(m_refreshButton);
    
    m_setDefaultButton = new QPushButton("⭐ Set as Default");
    m_setDefaultButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_setDefaultButton->setEnabled(false);
    connect(m_setDefaultButton, &QPushButton::clicked, this, &KernelManager::onSetDefaultKernel);
    actionsLayout->addWidget(m_setDefaultButton);
    
    m_removeButton = new QPushButton("🗑️ Remove Kernel");
    m_removeButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_removeButton->setEnabled(false);
    connect(m_removeButton, &QPushButton::clicked, this, &KernelManager::onRemoveKernel);
    actionsLayout->addWidget(m_removeButton);
    
    m_updateInitramfsButton = new QPushButton("🔧 Update Initramfs");
    m_updateInitramfsButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_updateInitramfsButton, &QPushButton::clicked, this, &KernelManager::onUpdateInitramfs);
    actionsLayout->addWidget(m_updateInitramfsButton);
    
    m_updateGrubButton = new QPushButton("🥾 Update GRUB");
    m_updateGrubButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_updateGrubButton, &QPushButton::clicked, this, &KernelManager::onUpdateGrub);
    actionsLayout->addWidget(m_updateGrubButton);
    
    m_viewConfigButton = new QPushButton("📄 View Config");
    m_viewConfigButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_viewConfigButton->setEnabled(false);
    connect(m_viewConfigButton, &QPushButton::clicked, this, &KernelManager::onViewKernelConfig);
    actionsLayout->addWidget(m_viewConfigButton);
    
    m_installToDeviceButton = new QPushButton("💾 Install to Other Device");
    m_installToDeviceButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_installToDeviceButton->setEnabled(false);
    connect(m_installToDeviceButton, &QPushButton::clicked, this, &KernelManager::onInstallKernelToDevice);
    actionsLayout->addWidget(m_installToDeviceButton);
    
    m_updateGrubOnDeviceButton = new QPushButton("🥾 Update GRUB on Device");
    m_updateGrubOnDeviceButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_updateGrubOnDeviceButton, &QPushButton::clicked, this, &KernelManager::onUpdateGrubOnDevice);
    actionsLayout->addWidget(m_updateGrubOnDeviceButton);
    
    // Install new kernel section
    actionsLayout->addWidget(new QLabel("Install New Kernel:"));
    
    m_availableKernelsCombo = new QComboBox();
    m_availableKernelsCombo->setStyleSheet(
        "QComboBox { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; padding: 3px; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: #F0F0F0; color: #000000; }"
    );
    actionsLayout->addWidget(m_availableKernelsCombo);
    
    m_installKernelButton = new QPushButton("📦 Install Kernel");
    m_installKernelButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_installKernelButton, &QPushButton::clicked, this, &KernelManager::onInstallKernel);
    actionsLayout->addWidget(m_installKernelButton);
    
    // Joshua's Fixes button
    actionsLayout->addWidget(new QLabel(""));  // Add spacing
    m_joshuaFixesButton = new QPushButton("🔧 Joshua's Fixes");
    m_joshuaFixesButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_joshuaFixesButton, &QPushButton::clicked, this, &KernelManager::onShowJoshuaFixes);
    actionsLayout->addWidget(m_joshuaFixesButton);
    
    actionsLayout->addStretch();
    rightLayout->addWidget(m_kernelActionsGroup);
    
    layout->addLayout(leftLayout, 2);
    layout->addLayout(rightLayout, 1);
}

void KernelManager::createPatchingTab()
{
    QWidget *tab = new QWidget();
    m_tabWidget->addTab(tab, "🩹 Kernel Patching");
    
    QHBoxLayout *layout = new QHBoxLayout(tab);
    
    // Left side - Available patches
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    m_patchListGroup = new QGroupBox("Available Patches");
    m_patchListGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *patchListLayout = new QVBoxLayout(m_patchListGroup);
    
    m_patchList = new QListWidget();
    m_patchList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    patchListLayout->addWidget(m_patchList);
    
    leftLayout->addWidget(m_patchListGroup);
    
    // Applied patches
    QGroupBox *appliedGroup = new QGroupBox("Applied Patches");
    appliedGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *appliedLayout = new QVBoxLayout(appliedGroup);
    m_appliedPatchesList = new QListWidget();
    m_appliedPatchesList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #00FF00; color: #000000; }"
    );
    appliedLayout->addWidget(m_appliedPatchesList);
    
    leftLayout->addWidget(appliedGroup);
    
    // Right side - Patch actions and preview
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    m_patchActionsGroup = new QGroupBox("Patch Actions");
    m_patchActionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *patchActionsLayout = new QVBoxLayout(m_patchActionsGroup);
    
    m_loadPatchButton = new QPushButton("📁 Load Patch File");
    m_loadPatchButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_loadPatchButton, &QPushButton::clicked, this, &KernelManager::onLoadPatchFile);
    patchActionsLayout->addWidget(m_loadPatchButton);
    
    m_applyPatchButton = new QPushButton("✅ Apply Patch");
    m_applyPatchButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_applyPatchButton, &QPushButton::clicked, this, &KernelManager::onApplyPatch);
    patchActionsLayout->addWidget(m_applyPatchButton);
    
    m_revertPatchButton = new QPushButton("❌ Revert Patch");
    m_revertPatchButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF0000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_revertPatchButton, &QPushButton::clicked, this, &KernelManager::onRevertPatch);
    patchActionsLayout->addWidget(m_revertPatchButton);
    
    m_createPatchButton = new QPushButton("🔧 Create Patch");
    m_createPatchButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #00FFFF; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_createPatchButton, &QPushButton::clicked, this, &KernelManager::onCreatePatch);
    patchActionsLayout->addWidget(m_createPatchButton);
    
    rightLayout->addWidget(m_patchActionsGroup);
    
    // Patch preview
    QGroupBox *previewGroup = new QGroupBox("Patch Preview");
    previewGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    m_patchPreviewText = new QTextEdit();
    m_patchPreviewText->setReadOnly(true);
    m_patchPreviewText->setFont(QFont("monospace"));
    m_patchPreviewText->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    previewLayout->addWidget(m_patchPreviewText);
    
    rightLayout->addWidget(previewGroup);
    
    layout->addLayout(leftLayout, 1);
    layout->addLayout(rightLayout, 1);
}

void KernelManager::createLiveConfigTab()
{
    QWidget *tab = new QWidget();
    m_tabWidget->addTab(tab, "⚡ Live Configuration");
    
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    // Kernel parameters section
    m_kernelParamsGroup = new QGroupBox("Kernel Parameters");
    m_kernelParamsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *paramsLayout = new QVBoxLayout(m_kernelParamsGroup);
    
    QHBoxLayout *paramInputLayout = new QHBoxLayout();
    paramInputLayout->addWidget(new QLabel("Parameter:"));
    m_paramNameEdit = new QLineEdit();
    m_paramNameEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    paramInputLayout->addWidget(m_paramNameEdit);
    
    paramInputLayout->addWidget(new QLabel("Value:"));
    m_paramValueEdit = new QLineEdit();
    m_paramValueEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    paramInputLayout->addWidget(m_paramValueEdit);
    
    m_applyParamButton = new QPushButton("Apply");
    m_applyParamButton->setStyleSheet("QPushButton { background-color: #000000; color: #39FF14; border: 2px solid #00FF00; padding: 5px; } QPushButton:hover { background-color: #001100; }");
    connect(m_applyParamButton, &QPushButton::clicked, this, &KernelManager::onApplyKernelParameter);
    paramInputLayout->addWidget(m_applyParamButton);
    
    paramsLayout->addLayout(paramInputLayout);
    
    m_kernelParamsList = new QListWidget();
    m_kernelParamsList->setMaximumHeight(150);
    m_kernelParamsList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    paramsLayout->addWidget(m_kernelParamsList);
    
    layout->addWidget(m_kernelParamsGroup);
    
    // Boot parameters section
    m_bootParamsGroup = new QGroupBox("Boot Parameters");
    m_bootParamsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *bootLayout = new QVBoxLayout(m_bootParamsGroup);
    
    m_bootParamsEdit = new QTextEdit();
    m_bootParamsEdit->setMaximumHeight(100);
    m_bootParamsEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    bootLayout->addWidget(m_bootParamsEdit);
    
    m_updateBootParamsButton = new QPushButton("Update Boot Parameters");
    m_updateBootParamsButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_updateBootParamsButton, &QPushButton::clicked, this, &KernelManager::onUpdateBootParameters);
    bootLayout->addWidget(m_updateBootParamsButton);
    
    layout->addWidget(m_bootParamsGroup);
    
    // Kernel config editing
    m_configOptionsGroup = new QGroupBox("Kernel Configuration");
    m_configOptionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *configLayout = new QVBoxLayout(m_configOptionsGroup);
    
    m_configOptionsList = new QListWidget();
    m_configOptionsList->setMaximumHeight(150);
    m_configOptionsList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    configLayout->addWidget(m_configOptionsList);
    
    m_configEditor = new QTextEdit();
    m_configEditor->setFont(QFont("monospace"));
    m_configEditor->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    configLayout->addWidget(m_configEditor);
    
    m_saveConfigButton = new QPushButton("Save Configuration");
    m_saveConfigButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_saveConfigButton, &QPushButton::clicked, this, &KernelManager::onSaveKernelConfig);
    configLayout->addWidget(m_saveConfigButton);
    
    layout->addWidget(m_configOptionsGroup);
}

void KernelManager::createModuleManagementTab()
{
    QWidget *tab = new QWidget();
    m_tabWidget->addTab(tab, "📦 Module Management");
    
    QHBoxLayout *layout = new QHBoxLayout(tab);
    
    // Left side - Loaded modules
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    m_loadedModulesGroup = new QGroupBox("Loaded Modules");
    m_loadedModulesGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *loadedLayout = new QVBoxLayout(m_loadedModulesGroup);
    
    m_loadedModulesList = new QListWidget();
    m_loadedModulesList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #00FF00; color: #000000; }"
    );
    connect(m_loadedModulesList, &QListWidget::itemSelectionChanged,
            this, &KernelManager::onModuleSelectionChanged);
    loadedLayout->addWidget(m_loadedModulesList);
    
    leftLayout->addWidget(m_loadedModulesGroup);
    
    // Right side - Available modules and actions
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    m_availableModulesGroup = new QGroupBox("Available Modules");
    m_availableModulesGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *availableLayout = new QVBoxLayout(m_availableModulesGroup);
    
    // Search box
    m_moduleSearchEdit = new QLineEdit();
    m_moduleSearchEdit->setPlaceholderText("Search modules...");
    m_moduleSearchEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    availableLayout->addWidget(m_moduleSearchEdit);
    
    m_availableModulesList = new QListWidget();
    m_availableModulesList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    availableLayout->addWidget(m_availableModulesList);
    
    rightLayout->addWidget(m_availableModulesGroup);
    
    // Module actions
    m_moduleActionsGroup = new QGroupBox("Module Actions");
    m_moduleActionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *actionsLayout = new QVBoxLayout(m_moduleActionsGroup);
    
    m_loadModuleButton = new QPushButton("📥 Load Module");
    m_loadModuleButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_loadModuleButton, &QPushButton::clicked, this, &KernelManager::onLoadModule);
    actionsLayout->addWidget(m_loadModuleButton);
    
    m_unloadModuleButton = new QPushButton("📤 Unload Module");
    m_unloadModuleButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF0000; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_unloadModuleButton, &QPushButton::clicked, this, &KernelManager::onUnloadModule);
    actionsLayout->addWidget(m_unloadModuleButton);
    
    m_blacklistModuleButton = new QPushButton("🚫 Blacklist Module");
    m_blacklistModuleButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FF00FF; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_blacklistModuleButton, &QPushButton::clicked, this, &KernelManager::onBlacklistModule);
    actionsLayout->addWidget(m_blacklistModuleButton);
    
    m_refreshModulesButton = new QPushButton("🔄 Refresh");
    m_refreshModulesButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #00FFFF; border: 2px solid #000000; padding: 8px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(m_refreshModulesButton, &QPushButton::clicked, this, &KernelManager::onRefreshModules);
    actionsLayout->addWidget(m_refreshModulesButton);
    
    rightLayout->addWidget(m_moduleActionsGroup);
    
    // Module information
    QGroupBox *infoGroup = new QGroupBox("Module Information");
    infoGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    m_moduleInfoText = new QTextEdit();
    m_moduleInfoText->setReadOnly(true);
    m_moduleInfoText->setMaximumHeight(150);
    m_moduleInfoText->setFont(QFont("monospace"));
    m_moduleInfoText->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    infoLayout->addWidget(m_moduleInfoText);
    
    rightLayout->addWidget(infoGroup);
    
    layout->addLayout(leftLayout, 1);
    layout->addLayout(rightLayout, 1);
}

// Implementation of slots would continue here with the actual functionality
// For brevity, I'll show a few key implementations:

void KernelManager::onRefreshKernels()
{
    m_statusLabel->setText("Scanning for installed kernels...");
    m_kernelList->clear();
    m_installedKernels.clear();
    
    // Get current kernel
    QProcess *currentProcess = new QProcess(this);
    connect(currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, currentProcess](int, QProcess::ExitStatus) {
        m_currentKernel = currentProcess->readAllStandardOutput().trimmed();
        m_currentKernelLabel->setText(QString("Current: %1").arg(m_currentKernel));
        currentProcess->deleteLater();
    });
    currentProcess->start("uname", QStringList() << "-r");
    
    // Scan for installed kernels in /boot
    QDir bootDir("/boot");
    QStringList installedKernelFiles = bootDir.entryList(QStringList() << "vmlinuz-*", QDir::Files);
    QStringList installedVersions;
    
    for (const QString &kernel : installedKernelFiles) {
        QString version = kernel.mid(8); // Remove "vmlinuz-"
        installedVersions.append(version);
        m_installedKernels.append(version);
    }
    
    // Scan for kernels in the tweaker directory
    QString kernelDir = m_kernelDirectoryEdit->text().trimmed();
    if (kernelDir.isEmpty()) {
        kernelDir = m_kernelDirectory;
    }
    
    QDir tweakerDir(kernelDir);
    if (!tweakerDir.exists()) {
        QDir().mkpath(kernelDir);
    }
    
    QStringList tweakerKernelFiles = tweakerDir.entryList(QStringList() << "vmlinuz-*", QDir::Files);
    QStringList allVersions = installedVersions;
    
    // Add tweaker kernels that aren't already installed
    for (const QString &kernel : tweakerKernelFiles) {
        QString version = kernel.mid(8); // Remove "vmlinuz-"
        if (!allVersions.contains(version)) {
            allVersions.append(version);
        }
    }
    
    // Add all kernels to the list
    for (const QString &version : allVersions) {
        QString displayText = "🐧 " + version;
        if (installedVersions.contains(version)) {
            displayText += " (Installed)";
        }
        
        QListWidgetItem *item = new QListWidgetItem(displayText);
        if (version == m_currentKernel) {
            item->setBackground(QBrush(QColor(0, 0, 0, 50)));
        }
        m_kernelList->addItem(item);
    }
    
    m_statusLabel->setText(QString("Found %1 installed kernels").arg(m_installedKernels.size()));
}

void KernelManager::onRefreshModules()
{
    m_statusLabel->setText("Scanning loaded modules...");
    m_loadedModulesList->clear();
    m_availableModulesList->clear();
    
    // Get loaded modules
    QProcess *lsmodProcess = new QProcess(this);
    connect(lsmodProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, lsmodProcess](int, QProcess::ExitStatus) {
        QString output = lsmodProcess->readAllStandardOutput();
        QStringList lines = output.split('\n');
        
        m_loadedModules.clear();
        for (int i = 1; i < lines.size(); ++i) { // Skip header
            QString line = lines[i].trimmed();
            if (!line.isEmpty()) {
                QString moduleName = line.split(' ').first();
                m_loadedModules.append(moduleName);
                m_loadedModulesList->addItem("✅ " + moduleName);
            }
        }
        
        lsmodProcess->deleteLater();
        m_statusLabel->setText(QString("Found %1 loaded modules").arg(m_loadedModules.size()));
    });
    lsmodProcess->start("lsmod", QStringList());
    
    // Get available modules (this is simplified - real implementation would scan /lib/modules)
    QDir modulesDir(QString("/lib/modules/%1").arg(m_currentKernel));
    if (modulesDir.exists()) {
        // Simplified: just show some common module directories
        QStringList dirs = modulesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &dir : dirs) {
            m_availableModulesList->addItem("📦 " + dir);
        }
    }
}

void KernelManager::onKernelSelectionChanged()
{
    QListWidgetItem *item = m_kernelList->currentItem();
    if (!item) {
        m_setDefaultButton->setEnabled(false);
        m_removeButton->setEnabled(false);
        m_viewConfigButton->setEnabled(false);
        m_installToDeviceButton->setEnabled(false);
        m_backupKernelButton->setEnabled(false);
        return;
    }
    
    QString kernelVersion = cleanKernelVersion(item->text());
    
    m_setDefaultButton->setEnabled(kernelVersion != m_currentKernel);
    m_removeButton->setEnabled(kernelVersion != m_currentKernel);
    m_viewConfigButton->setEnabled(true);
    m_installToDeviceButton->setEnabled(true);
    m_backupKernelButton->setEnabled(true);
    
    // Show kernel details
    QString details = QString("Kernel: %1\n").arg(kernelVersion);
    
    // Check for config file
    QString configPath = QString("/boot/config-%1").arg(kernelVersion);
    if (QFile::exists(configPath)) {
        details += "Configuration: Available\n";
    }
    
    // Check for initramfs
    QString initramfsPath = QString("/boot/initrd.img-%1").arg(kernelVersion);
    if (QFile::exists(initramfsPath)) {
        QFileInfo initramfsInfo(initramfsPath);
        details += QString("Initramfs: %1 MB\n").arg(initramfsInfo.size() / 1024 / 1024);
    }
    
    m_kernelDetailsText->setPlainText(details);
}

void KernelManager::onSetDefaultKernel()
{
    QListWidgetItem *item = m_kernelList->currentItem();
    if (!item) return;
    
    QString kernelVersion = cleanKernelVersion(item->text());
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Set Default Kernel",
        QString("Set %1 as the default kernel?\n\nThis will update GRUB configuration.").arg(kernelVersion),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        emit setDefaultKernelRequested(kernelVersion);
        m_statusLabel->setText(QString("Setting %1 as default kernel...").arg(kernelVersion));
    }
}

void KernelManager::onRemoveKernel()
{
    QListWidgetItem *item = m_kernelList->currentItem();
    if (!item) return;
    
    QString kernelVersion = cleanKernelVersion(item->text());
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove Kernel",
        QString("Remove kernel %1?\n\nThis will delete the kernel and its modules.").arg(kernelVersion),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        emit removeKernelRequested(kernelVersion);
        m_statusLabel->setText(QString("Removing kernel %1...").arg(kernelVersion));
    }
}

void KernelManager::onUpdateInitramfs()
{
    emit updateInitramfsRequested("all");
    m_statusLabel->setText("Updating initramfs for all kernels...");
}

void KernelManager::onUpdateGrub()
{
    m_statusLabel->setText("Updating GRUB configuration...");
    // This would trigger the SystemManager to update GRUB
}

void KernelManager::onViewKernelConfig()
{
    QListWidgetItem *item = m_kernelList->currentItem();
    if (!item) return;
    
    QString kernelVersion = cleanKernelVersion(item->text());
    QString configPath = QString("/boot/config-%1").arg(kernelVersion);
    
    QFile configFile(configPath);
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = configFile.readAll();
        m_configEditor->setPlainText(content);
        m_tabWidget->setCurrentIndex(2); // Switch to Live Configuration tab
    } else {
        QMessageBox::warning(this, "Error", QString("Could not read kernel config: %1").arg(configPath));
    }
}

// Add placeholder implementations for the other slots
void KernelManager::onInstallKernel() { /* Implementation */ }

void KernelManager::onInstallKernelToDevice()
{
    QListWidgetItem *item = m_kernelList->currentItem();
    if (!item) return;
    
    QString originalText = item->text();
    QString kernelVersion = cleanKernelVersion(originalText);
    bool isInstalledKernel = originalText.contains("(Installed)");
    
    // Create a dialog for device selection
    QDialog dialog(this);
    dialog.setWindowTitle("Install Kernel to Other Device");
    dialog.setFixedSize(600, 500);
    dialog.setStyleSheet("background-color: #DCDCDC;");
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Target Device for Kernel Installation");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(titleLabel);
    
    // Info label
    QLabel *infoLabel = new QLabel(QString("Installing kernel %1 to another device.\n"
                                          "This is useful for repairing systems from a live image.").arg(kernelVersion));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #000000; margin: 10px; padding: 10px; background-color: #F0F0F0; border: 1px solid #000000;");
    dialogLayout->addWidget(infoLabel);
    
    // Device list
    QGroupBox *deviceGroup = new QGroupBox("Available Block Devices");
    deviceGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *deviceLayout = new QVBoxLayout(deviceGroup);
    
    QListWidget *deviceList = new QListWidget();
    deviceList->setStyleSheet(
        "QListWidget { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; }"
        "QListWidget::item:selected { background-color: #000000; color: #FFFFFF; }"
    );
    
    // Scan for block devices
    m_statusLabel->setText("Scanning for block devices...");
    
    QProcess *lsblkProcess = new QProcess(this);
    lsblkProcess->start("lsblk", QStringList() << "-J" << "-o" << "NAME,SIZE,TYPE,MOUNTPOINT,MODEL");
    lsblkProcess->waitForFinished();
    
    QString lsblkOutput = lsblkProcess->readAllStandardOutput();
    
    // Parse lsblk output (simplified for now - in real implementation would use JSON parser)
    QDir devDir("/dev");
    QStringList blockDevices = devDir.entryList(QStringList() << "sd*" << "nvme*" << "mmcblk*", QDir::System);
    
    // Filter to only show main devices (not partitions)
    QStringList mainDevices;
    for (const QString &device : blockDevices) {
        // Skip partition entries
    if (!device.contains(QRegularExpression("[0-9]$")) || device.startsWith("mmcblk")) {
            if (device.startsWith("mmcblk") && device.contains("p")) continue;
            mainDevices.append(device);
        }
    }
    
    // Add devices to list with details
    for (const QString &device : mainDevices) {
        QString devicePath = "/dev/" + device;
        
        // Get device info
        QProcess *deviceInfoProcess = new QProcess(this);
        deviceInfoProcess->start("lsblk", QStringList() << "-n" << "-o" << "SIZE,MODEL" << devicePath);
        deviceInfoProcess->waitForFinished();
        QString deviceInfo = deviceInfoProcess->readAllStandardOutput().trimmed();
        
        QString listEntry = QString("💾 %1 - %2").arg(devicePath).arg(deviceInfo);
        
        // Check if device is mounted (safety check)
        QProcess *mountProcess = new QProcess(this);
        mountProcess->start("findmnt", QStringList() << "-n" << "-o" << "TARGET" << devicePath);
        mountProcess->waitForFinished();
        QString mountPoint = mountProcess->readAllStandardOutput().trimmed();
        
        if (!mountPoint.isEmpty()) {
            listEntry += QString(" [MOUNTED at %1]").arg(mountPoint);
        }
        
        QListWidgetItem *deviceItem = new QListWidgetItem(listEntry);
        deviceItem->setData(Qt::UserRole, devicePath);
        
        // Mark mounted devices with different color
        if (!mountPoint.isEmpty()) {
            deviceItem->setForeground(QBrush(QColor(255, 0, 0)));
        }
        
        deviceList->addItem(deviceItem);
    }
    
    deviceLayout->addWidget(deviceList);
    dialogLayout->addWidget(deviceGroup);
    
    // Options group
    QGroupBox *optionsGroup = new QGroupBox("Installation Options");
    optionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    
    QCheckBox *mountRootCheckbox = new QCheckBox("Mount root partition automatically");
    mountRootCheckbox->setChecked(true);
    mountRootCheckbox->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(mountRootCheckbox);
    
    // Add custom mount point option
    QHBoxLayout *mountPointLayout = new QHBoxLayout();
    QLabel *mountPointLabel = new QLabel("Custom mount point (if already mounted):");
    mountPointLabel->setStyleSheet("color: #000000;");
    mountPointLayout->addWidget(mountPointLabel);
    
    QLineEdit *customMountPointEdit = new QLineEdit();
    customMountPointEdit->setPlaceholderText("/mnt/target");
    customMountPointEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    customMountPointEdit->setEnabled(!mountRootCheckbox->isChecked());
    mountPointLayout->addWidget(customMountPointEdit);
    
    optionsLayout->addLayout(mountPointLayout);
    
    // Connect checkbox to enable/disable custom mount point
    connect(mountRootCheckbox, &QCheckBox::toggled, [customMountPointEdit](bool checked) {
        customMountPointEdit->setEnabled(!checked);
        if (checked) {
            customMountPointEdit->clear();
        }
    });
    
    QCheckBox *updateGrubCheckbox = new QCheckBox("Update GRUB configuration");
    updateGrubCheckbox->setChecked(true);
    updateGrubCheckbox->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(updateGrubCheckbox);
    
    QCheckBox *copyModulesCheckbox = new QCheckBox("Copy kernel modules");
    copyModulesCheckbox->setChecked(true);
    copyModulesCheckbox->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(copyModulesCheckbox);
    
    dialogLayout->addWidget(optionsGroup);
    
    // Warning label
    QLabel *warningLabel = new QLabel("⚠️ WARNING: This operation will modify the target device.\n"
                                     "Make sure you have selected the correct device!");
    warningLabel->setStyleSheet("color: #FF0000; font-weight: bold; margin: 10px;");
    warningLabel->setAlignment(Qt::AlignCenter);
    dialogLayout->addWidget(warningLabel);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Install Kernel");
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    buttonBox->setStyleSheet(
        "QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; }"
        "QPushButton:hover { background-color: #E0E0E0; }"
    );
    
    // Enable OK button only when device is selected
    connect(deviceList, &QListWidget::itemSelectionChanged, [buttonBox, deviceList]() {
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(deviceList->currentItem() != nullptr);
    });
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    dialogLayout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        QListWidgetItem *selectedDevice = deviceList->currentItem();
        if (selectedDevice) {
            QString devicePath = selectedDevice->data(Qt::UserRole).toString();
            
            // Build confirmation message based on selected options
            QString confirmMessage = QString("Are you sure you want to install kernel %1 to device %2?\n\n"
                                           "This operation will:\n").arg(kernelVersion).arg(devicePath);
            
            if (mountRootCheckbox->isChecked()) {
                confirmMessage += "• Mount the device's root partition\n";
            } else if (!customMountPointEdit->text().trimmed().isEmpty()) {
                confirmMessage += QString("• Use custom mount point: %1\n").arg(customMountPointEdit->text().trimmed());
            }
            
            confirmMessage += "• Copy kernel files to /boot\n";
            confirmMessage += "• Update initramfs\n";
            
            if (updateGrubCheckbox->isChecked()) {
                confirmMessage += "• Update GRUB configuration\n";
            }
            
            if (copyModulesCheckbox->isChecked()) {
                confirmMessage += "• Copy kernel modules\n";
            }
            
            confirmMessage += "\nThis may take several minutes.";
            
            // Confirm with user
            QMessageBox::StandardButton confirm = QMessageBox::question(this, 
                "Confirm Kernel Installation",
                confirmMessage,
                QMessageBox::Yes | QMessageBox::No);
                
            if (confirm == QMessageBox::Yes) {
                // Create progress dialog
                QProgressDialog progress("Installing kernel to device...", "Cancel", 0, 100, this);
                progress.setWindowModality(Qt::WindowModal);
                progress.setWindowTitle("Kernel Installation Progress");
                progress.setAutoClose(true);
                progress.setValue(0);
                
                // Perform the installation
                QString customMountPoint = customMountPointEdit->text().trimmed();
                performKernelInstallation(kernelVersion, devicePath, 
                                        mountRootCheckbox->isChecked(),
                                        updateGrubCheckbox->isChecked(),
                                        copyModulesCheckbox->isChecked(),
                                        customMountPoint,
                                        isInstalledKernel,
                                        &progress);
            }
        }
    }
    
    m_statusLabel->setText("Ready");
}

void KernelManager::performKernelInstallation(const QString &kernelVersion, 
                                             const QString &devicePath,
                                             bool mountRoot,
                                             bool updateGrub,
                                             bool copyModules,
                                             const QString &customMountPoint,
                                             bool isInstalledKernel,
                                             QProgressDialog *progress)
{
    QString mountPoint = customMountPoint.isEmpty() ? "/mnt/kernel-install-target" : customMountPoint;
    bool success = true;
    QString errorMessage;
    bool shouldUnmount = mountRoot && customMountPoint.isEmpty();
    
    // Step 1: Create mount point (only if not using custom mount point)
    progress->setLabelText("Preparing mount point...");
    progress->setValue(10);
    if (customMountPoint.isEmpty()) {
        QProcess::execute("mkdir", QStringList() << "-p" << mountPoint);
    }
    
    if (mountRoot) {
        // Step 2: Find and mount root partition
        progress->setLabelText("Finding root partition...");
        progress->setValue(20);
        
        // Try to find root partition (simplified - real implementation would be more sophisticated)
        QString rootPartition;
        QProcess *partitionsProcess = new QProcess(this);
        partitionsProcess->start("lsblk", QStringList() << "-n" << "-o" << "NAME,FSTYPE" << devicePath);
        partitionsProcess->waitForFinished();
        
        QStringList partitions = QString(partitionsProcess->readAllStandardOutput()).split('\n');
        for (const QString &partition : partitions) {
            if (partition.contains("ext4") || partition.contains("btrfs")) {
                rootPartition = devicePath + partition.split(' ').first().trimmed().mid(2);
                break;
            }
        }
        
        if (rootPartition.isEmpty()) {
            // Try first partition as fallback
            rootPartition = devicePath + "1";
        }
        
        // Mount the partition
        progress->setLabelText(QString("Mounting %1...").arg(rootPartition));
        progress->setValue(30);
        
        QProcess *mountProcess = new QProcess(this);
        mountProcess->start("mount", QStringList() << rootPartition << mountPoint);
        mountProcess->waitForFinished();
        
        if (mountProcess->exitCode() != 0) {
            errorMessage = "Failed to mount root partition: " + mountProcess->readAllStandardError();
            success = false;
        }
    }
    
    if (success) {
        // Step 3: Copy kernel files
        progress->setLabelText("Copying kernel files...");
        progress->setValue(40);
        
        QString kernelSource;
        if (isInstalledKernel) {
            // Copy from live system locations
            kernelSource = QString("/boot/vmlinuz-%1").arg(kernelVersion);
        } else {
            // Copy from tweaker directory
            QString kernelDir = m_kernelDirectoryEdit->text().trimmed();
            if (kernelDir.isEmpty()) {
                kernelDir = m_kernelDirectory;
            }
            kernelSource = QString("%1/vmlinuz-%2").arg(kernelDir).arg(kernelVersion);
        }
        QString kernelDest = QString("%1/boot/vmlinuz-%2").arg(mountPoint).arg(kernelVersion);
        
        // Ensure /boot exists on target
        QProcess::execute("mkdir", QStringList() << "-p" << mountPoint + "/boot");
        
        // Copy kernel file using rsync (handles bind mounts and same-file scenarios)
        QProcess *copyKernelProcess = new QProcess(this);
        copyKernelProcess->start("rsync", QStringList() << "-av" << "--update" << kernelSource << kernelDest);
        copyKernelProcess->waitForFinished();
        
        if (copyKernelProcess->exitCode() != 0) {
            errorMessage = "Failed to copy kernel: " + QString(copyKernelProcess->readAllStandardError());
            success = false;
        }
        
        // Copy initramfs
        if (success) {
            progress->setLabelText("Copying initramfs...");
            progress->setValue(50);
            
            QString initramfsSource;
            if (isInstalledKernel) {
                initramfsSource = QString("/boot/initrd.img-%1").arg(kernelVersion);
            } else {
                QString kernelDir = m_kernelDirectoryEdit->text().trimmed();
                if (kernelDir.isEmpty()) {
                    kernelDir = m_kernelDirectory;
                }
                initramfsSource = QString("%1/initrd.img-%2").arg(kernelDir).arg(kernelVersion);
            }
            QString initramfsDest = QString("%1/boot/initrd.img-%2").arg(mountPoint).arg(kernelVersion);
            
            // Copy initramfs using rsync
            QProcess *copyInitramfsProcess = new QProcess(this);
            copyInitramfsProcess->start("rsync", QStringList() << "-av" << "--update" << initramfsSource << initramfsDest);
            copyInitramfsProcess->waitForFinished();
            
            if (copyInitramfsProcess->exitCode() != 0) {
                // Try to generate initramfs if copy failed
                progress->setLabelText("Generating initramfs...");
                QProcess *mkinitramfsProcess = new QProcess(this);
                mkinitramfsProcess->start("chroot", QStringList() << mountPoint << "update-initramfs" << "-c" << "-k" << kernelVersion);
                mkinitramfsProcess->waitForFinished();
            }
        }
        
        // Copy kernel config
        if (success) {
            QString configSource;
            if (isInstalledKernel) {
                configSource = QString("/boot/config-%1").arg(kernelVersion);
            } else {
                QString kernelDir = m_kernelDirectoryEdit->text().trimmed();
                if (kernelDir.isEmpty()) {
                    kernelDir = m_kernelDirectory;
                }
                configSource = QString("%1/config-%2").arg(kernelDir).arg(kernelVersion);
            }
            QString configDest = QString("%1/boot/config-%2").arg(mountPoint).arg(kernelVersion);
            
            // Copy config using rsync
            QProcess::execute("rsync", QStringList() << "-av" << "--update" << configSource << configDest);
        }
        
        // Copy System.map
        if (success) {
            QString sysmapSource;
            if (isInstalledKernel) {
                sysmapSource = QString("/boot/System.map-%1").arg(kernelVersion);
            } else {
                QString kernelDir = m_kernelDirectoryEdit->text().trimmed();
                if (kernelDir.isEmpty()) {
                    kernelDir = m_kernelDirectory;
                }
                sysmapSource = QString("%1/System.map-%2").arg(kernelDir).arg(kernelVersion);
            }
            QString sysmapDest = QString("%1/boot/System.map-%2").arg(mountPoint).arg(kernelVersion);
            
            // Copy System.map using rsync
            QProcess::execute("rsync", QStringList() << "-av" << "--update" << sysmapSource << sysmapDest);
        }
    }
    
    if (success && copyModules) {
        // Step 4: Copy kernel modules
        progress->setLabelText("Copying kernel modules...");
        progress->setValue(60);
        
        QString modulesSource = QString("/lib/modules/%1").arg(kernelVersion);
        QString modulesDest = QString("%1/lib/modules/%2").arg(mountPoint).arg(kernelVersion);
        
        // Ensure modules directory exists
        QProcess::execute("mkdir", QStringList() << "-p" << QString("%1/lib/modules/").arg(mountPoint));
        
        // Copy modules using rsync (handles bind mounts and overwrites)
        QProcess *copyModulesProcess = new QProcess(this);
        copyModulesProcess->start("rsync", QStringList() << "-av" << "--update" << "--delete" << modulesSource + "/" << QString("%1/lib/modules/%2/").arg(mountPoint).arg(kernelVersion));
        copyModulesProcess->waitForFinished();
        
        if (copyModulesProcess->exitCode() != 0) {
            errorMessage = "Failed to copy kernel modules: " + QString(copyModulesProcess->readAllStandardError());
            success = false;
        }
    }
    
    if (success && updateGrub) {
        // Step 5: Update GRUB configuration
        progress->setLabelText("Updating GRUB configuration...");
        progress->setValue(80);
        
        // Mount necessary filesystems for chroot
        QProcess::execute("mount", QStringList() << "--bind" << "/dev" << mountPoint + "/dev");
        QProcess::execute("mount", QStringList() << "--bind" << "/proc" << mountPoint + "/proc");
        QProcess::execute("mount", QStringList() << "--bind" << "/sys" << mountPoint + "/sys");
        
        // Update GRUB in chroot
        QProcess *grubProcess = new QProcess(this);
        grubProcess->start("chroot", QStringList() << mountPoint << "update-grub");
        grubProcess->waitForFinished();
        
        if (grubProcess->exitCode() != 0) {
            // Try grub-mkconfig as fallback
            QProcess *grubMkconfigProcess = new QProcess(this);
            grubMkconfigProcess->start("chroot", QStringList() << mountPoint << "grub-mkconfig" << "-o" << "/boot/grub/grub.cfg");
            grubMkconfigProcess->waitForFinished();
        }
        
        // Unmount bind mounts
        QProcess::execute("umount", QStringList() << mountPoint + "/sys");
        QProcess::execute("umount", QStringList() << mountPoint + "/proc");
        QProcess::execute("umount", QStringList() << mountPoint + "/dev");
    }
    
    // Step 6: Cleanup
    progress->setLabelText("Cleaning up...");
    progress->setValue(90);
    
    if (shouldUnmount) {
        QProcess::execute("umount", QStringList() << mountPoint);
    }
    
    progress->setValue(100);
    
    // Show result
    if (success) {
        QMessageBox::information(this, "Installation Complete",
            QString("Successfully installed kernel %1 to device %2\n\n"
                   "The target system should now be able to boot with the new kernel.")
            .arg(kernelVersion).arg(devicePath));
        
        m_statusLabel->setText(QString("Kernel %1 installed to %2").arg(kernelVersion).arg(devicePath));
    } else {
        QMessageBox::critical(this, "Installation Failed",
            QString("Failed to install kernel to device.\n\nError: %1").arg(errorMessage));
        
        m_statusLabel->setText("Kernel installation failed");
    }
}

void KernelManager::onUpdateGrubOnDevice()
{
    // Create a dialog for device/mount point selection
    QDialog dialog(this);
    dialog.setWindowTitle("Update GRUB on Device");
    dialog.setFixedSize(600, 400);
    dialog.setStyleSheet("background-color: #DCDCDC;");
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    
    // Title
    QLabel *titleLabel = new QLabel("Update GRUB Configuration on External Device");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(titleLabel);
    
    // Info label
    QLabel *infoLabel = new QLabel("This will update the GRUB bootloader configuration on another device.\n"
                                  "Useful for fixing boot issues without reinstalling the kernel.");
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #000000; margin: 10px; padding: 10px; background-color: #F0F0F0; border: 1px solid #000000;");
    dialogLayout->addWidget(infoLabel);
    
    // Options group
    QGroupBox *optionsGroup = new QGroupBox("Target Options");
    optionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    
    // Radio buttons for mount type
    QRadioButton *autoMountRadio = new QRadioButton("Select device to mount automatically");
    autoMountRadio->setChecked(true);
    autoMountRadio->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(autoMountRadio);
    
    QRadioButton *customMountRadio = new QRadioButton("Specify custom mount point (already mounted)");
    customMountRadio->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(customMountRadio);
    
    // Device selection (for auto mount)
    QComboBox *deviceCombo = new QComboBox();
    deviceCombo->setStyleSheet(
        "QComboBox { background-color: #F0F0F0; color: #000000; border: 1px solid #000000; padding: 3px; margin-left: 20px; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: #F0F0F0; color: #000000; }"
    );
    
    // Scan for block devices
    QDir devDir("/dev");
    QStringList blockDevices = devDir.entryList(QStringList() << "sd*" << "nvme*" << "mmcblk*", QDir::System);
    for (const QString &device : blockDevices) {
    if (!device.contains(QRegularExpression("[0-9]$")) || device.startsWith("mmcblk")) {
            if (device.startsWith("mmcblk") && device.contains("p")) continue;
            deviceCombo->addItem("/dev/" + device);
        }
    }
    optionsLayout->addWidget(deviceCombo);
    
    // Custom mount point input
    QHBoxLayout *mountPointLayout = new QHBoxLayout();
    QLabel *mountPointLabel = new QLabel("Mount point:");
    mountPointLabel->setStyleSheet("color: #000000; margin-left: 20px;");
    mountPointLayout->addWidget(mountPointLabel);
    
    QLineEdit *mountPointEdit = new QLineEdit();
    mountPointEdit->setPlaceholderText("/mnt/target");
    mountPointEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    mountPointEdit->setEnabled(false);
    mountPointLayout->addWidget(mountPointEdit);
    
    optionsLayout->addLayout(mountPointLayout);
    
    // Connect radio buttons
    connect(autoMountRadio, &QRadioButton::toggled, [deviceCombo, mountPointEdit](bool checked) {
        deviceCombo->setEnabled(checked);
        mountPointEdit->setEnabled(!checked);
        if (!checked) {
            mountPointEdit->setFocus();
        }
    });
    
    dialogLayout->addWidget(optionsGroup);
    
    // Additional options
    QCheckBox *updateInitramfsCheckbox = new QCheckBox("Also update initramfs");
    updateInitramfsCheckbox->setChecked(true);
    updateInitramfsCheckbox->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(updateInitramfsCheckbox);
    
    // Warning label
    QLabel *warningLabel = new QLabel("⚠️ Make sure the target device/mount point contains a valid Linux installation!");
    warningLabel->setStyleSheet("color: #FF0000; font-weight: bold; margin: 10px;");
    warningLabel->setAlignment(Qt::AlignCenter);
    dialogLayout->addWidget(warningLabel);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Update GRUB");
    buttonBox->setStyleSheet(
        "QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; }"
        "QPushButton:hover { background-color: #E0E0E0; }"
    );
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    dialogLayout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString mountPoint;
        QString devicePath;
        bool needsMount = false;
        
        if (autoMountRadio->isChecked()) {
            devicePath = deviceCombo->currentText();
            mountPoint = "/mnt/grub-update-target";
            needsMount = true;
        } else {
            mountPoint = mountPointEdit->text().trimmed();
            if (mountPoint.isEmpty()) {
                QMessageBox::warning(this, "Invalid Mount Point", "Please specify a valid mount point.");
                return;
            }
        }
        
        // Confirm with user
        QString targetInfo = needsMount ? QString("device %1").arg(devicePath) : QString("mount point %1").arg(mountPoint);
        QMessageBox::StandardButton confirm = QMessageBox::question(this, 
            "Confirm GRUB Update",
            QString("Are you sure you want to update GRUB on %1?\n\n"
                   "This operation will:\n"
                   "• %2\n"
                   "• Mount necessary filesystems (/dev, /proc, /sys)\n"
                   "• Run update-grub in chroot environment\n"
                   "%3")
                   .arg(targetInfo)
                   .arg(needsMount ? "Mount the device's root partition" : "Use the existing mount")
                   .arg(updateInitramfsCheckbox->isChecked() ? "• Update initramfs for all kernels\n" : ""),
            QMessageBox::Yes | QMessageBox::No);
            
        if (confirm == QMessageBox::Yes) {
            // Create progress dialog
            QProgressDialog progress("Updating GRUB...", "Cancel", 0, 100, this);
            progress.setWindowModality(Qt::WindowModal);
            progress.setWindowTitle("GRUB Update Progress");
            progress.setAutoClose(true);
            progress.setValue(0);
            
            // Perform the GRUB update
            performGrubUpdate(mountPoint, devicePath, needsMount, 
                            updateInitramfsCheckbox->isChecked(), &progress);
        }
    }
}

void KernelManager::performGrubUpdate(const QString &mountPoint,
                                     const QString &devicePath,
                                     bool needsMount,
                                     bool updateInitramfs,
                                     QProgressDialog *progress)
{
    bool success = true;
    QString errorMessage;
    QString actualMountPoint = mountPoint;
    
    // Step 1: Mount if needed
    if (needsMount) {
        progress->setLabelText("Creating mount point...");
        progress->setValue(10);
        QProcess::execute("mkdir", QStringList() << "-p" << mountPoint);
        
        progress->setLabelText("Finding root partition...");
        progress->setValue(20);
        
        // Try to find root partition
        QString rootPartition;
        QProcess *partitionsProcess = new QProcess(this);
        partitionsProcess->start("lsblk", QStringList() << "-n" << "-o" << "NAME,FSTYPE" << devicePath);
        partitionsProcess->waitForFinished();
        
        QStringList partitions = QString(partitionsProcess->readAllStandardOutput()).split('\n');
        for (const QString &partition : partitions) {
            if (partition.contains("ext4") || partition.contains("btrfs")) {
                rootPartition = devicePath + partition.split(' ').first().trimmed().mid(2);
                break;
            }
        }
        
        if (rootPartition.isEmpty()) {
            rootPartition = devicePath + "1";
        }
        
        progress->setLabelText(QString("Mounting %1...").arg(rootPartition));
        progress->setValue(30);
        
        QProcess *mountProcess = new QProcess(this);
        mountProcess->start("mount", QStringList() << rootPartition << mountPoint);
        mountProcess->waitForFinished();
        
        if (mountProcess->exitCode() != 0) {
            errorMessage = "Failed to mount root partition: " + mountProcess->readAllStandardError();
            success = false;
        }
    } else {
        progress->setValue(30);
    }
    
    if (success) {
        // Step 2: Mount necessary filesystems for chroot
        progress->setLabelText("Mounting system filesystems...");
        progress->setValue(40);
        
        QProcess::execute("mount", QStringList() << "--bind" << "/dev" << actualMountPoint + "/dev");
        QProcess::execute("mount", QStringList() << "--bind" << "/proc" << actualMountPoint + "/proc");
        QProcess::execute("mount", QStringList() << "--bind" << "/sys" << actualMountPoint + "/sys");
        
        // Step 3: Update initramfs if requested
        if (updateInitramfs) {
            progress->setLabelText("Updating initramfs...");
            progress->setValue(60);
            
            QProcess *initramfsProcess = new QProcess(this);
            initramfsProcess->start("chroot", QStringList() << actualMountPoint << "update-initramfs" << "-u" << "-k" << "all");
            initramfsProcess->waitForFinished();
            
            if (initramfsProcess->exitCode() != 0) {
                m_statusLabel->setText("Warning: initramfs update had issues");
            }
        }
        
        // Step 4: Update GRUB
        progress->setLabelText("Updating GRUB configuration...");
        progress->setValue(80);
        
        QProcess *grubProcess = new QProcess(this);
        grubProcess->start("chroot", QStringList() << actualMountPoint << "update-grub");
        grubProcess->waitForFinished();
        
        if (grubProcess->exitCode() != 0) {
            // Try grub-mkconfig as fallback
            QProcess *grubMkconfigProcess = new QProcess(this);
            grubMkconfigProcess->start("chroot", QStringList() << actualMountPoint << "grub-mkconfig" << "-o" << "/boot/grub/grub.cfg");
            grubMkconfigProcess->waitForFinished();
            
            if (grubMkconfigProcess->exitCode() != 0) {
                errorMessage = "Failed to update GRUB: " + grubProcess->readAllStandardError();
                success = false;
            }
        }
        
        // Step 5: Cleanup
        progress->setLabelText("Cleaning up...");
        progress->setValue(90);
        
        QProcess::execute("umount", QStringList() << actualMountPoint + "/sys");
        QProcess::execute("umount", QStringList() << actualMountPoint + "/proc");
        QProcess::execute("umount", QStringList() << actualMountPoint + "/dev");
        
        if (needsMount) {
            QProcess::execute("umount", QStringList() << actualMountPoint);
        }
    }
    
    progress->setValue(100);
    
    // Show result
    if (success) {
        QMessageBox::information(this, "GRUB Update Complete",
            "Successfully updated GRUB configuration.\n\n"
            "The target system should now show all available kernels in the boot menu.");
        
        m_statusLabel->setText("GRUB updated successfully");
    } else {
        QMessageBox::critical(this, "GRUB Update Failed",
            QString("Failed to update GRUB.\n\nError: %1").arg(errorMessage));
        
        m_statusLabel->setText("GRUB update failed");
    }
}

void KernelManager::onApplyPatch() { /* Implementation */ }
void KernelManager::onRevertPatch() { /* Implementation */ }
void KernelManager::onCreatePatch() { /* Implementation */ }
void KernelManager::onLoadPatchFile() { /* Implementation */ }
void KernelManager::onRefreshPatches() { /* Implementation */ }
void KernelManager::onApplyKernelParameter() { /* Implementation */ }
void KernelManager::onUpdateBootParameters() { /* Implementation */ }
void KernelManager::onEditKernelConfig() { /* Implementation */ }
void KernelManager::onSaveKernelConfig() { /* Implementation */ }
void KernelManager::onLoadModule() { /* Implementation */ }
void KernelManager::onUnloadModule() { /* Implementation */ }
void KernelManager::onBlacklistModule() { /* Implementation */ }

void KernelManager::onBrowseKernelDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Kernel Directory", m_kernelDirectoryEdit->text());
    if (!dir.isEmpty()) {
        m_kernelDirectoryEdit->setText(dir);
        m_kernelDirectory = dir;
        onRefreshKernels(); // Refresh kernel list with new directory
    }
}

void KernelManager::onCopyCurrentKernel()
{
    // Get current kernel version first
    QProcess currentProcess;
    currentProcess.start("uname", QStringList() << "-r");
    currentProcess.waitForFinished();
    QString currentKernel = currentProcess.readAllStandardOutput().trimmed();
    
    if (currentKernel.isEmpty()) {
        QMessageBox::warning(this, "Error", "Could not determine current kernel version.");
        return;
    }
    
    // Create dialog to choose destination
    QDialog dialog(this);
    dialog.setWindowTitle("Copy Current Kernel");
    dialog.setFixedSize(500, 350);
    dialog.setStyleSheet("background-color: #DCDCDC;");
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    
    // Title
    QLabel *titleLabel = new QLabel(QString("Copy Current Kernel (%1)").arg(currentKernel));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(titleLabel);
    
    // Info label
    QLabel *infoLabel = new QLabel("Choose where to copy the currently running kernel:");
    infoLabel->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(infoLabel);
    
    // Destination options
    QGroupBox *destGroup = new QGroupBox("Copy Destination");
    destGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *destLayout = new QVBoxLayout(destGroup);
    
    QRadioButton *toTweakerDirRadio = new QRadioButton("To Tweaker Directory (for kernel installation)");
    toTweakerDirRadio->setChecked(true);
    toTweakerDirRadio->setStyleSheet("color: #000000;");
    destLayout->addWidget(toTweakerDirRadio);
    
    QRadioButton *toMountedDeviceRadio = new QRadioButton("To Mounted Device (direct install)");
    toMountedDeviceRadio->setStyleSheet("color: #000000;");
    destLayout->addWidget(toMountedDeviceRadio);
    
    // Path selection
    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLineEdit *pathEdit = new QLineEdit();
    pathEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    pathLayout->addWidget(pathEdit);
    
    QPushButton *browseButton = new QPushButton("📁 Browse");
    browseButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 4px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(browseButton, &QPushButton::clicked, [&]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Destination", pathEdit->text());
        if (!dir.isEmpty()) {
            pathEdit->setText(dir);
        }
    });
    pathLayout->addWidget(browseButton);
    
    destLayout->addLayout(pathLayout);
    
    dialogLayout->addWidget(destGroup);
    
    // Files to copy info
    QLabel *filesLabel = new QLabel("Files to be copied:\n• vmlinuz-" + currentKernel + "\n• initrd.img-" + currentKernel + "\n• System.map-" + currentKernel + "\n• config-" + currentKernel);
    filesLabel->setStyleSheet("color: #000000; background-color: #F0F0F0; border: 1px solid #000000; padding: 10px; margin: 10px;");
    dialogLayout->addWidget(filesLabel);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->setStyleSheet(
        "QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; }"
        "QPushButton:hover { background-color: #E0E0E0; }"
    );
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    dialogLayout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString destPath = pathEdit->text().trimmed();
        bool isDeviceInstall = toMountedDeviceRadio->isChecked();
        
        if (destPath.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please select a destination path.");
            return;
        }
        
        // Create progress dialog
        QProgressDialog progress("Copying kernel files...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowTitle("Copy Kernel Progress");
        progress.setValue(0);
        
        bool success = true;
        QString errorMessage;
        
        // Prepare Ubuntu kernel file locations
        struct KernelFile {
            QString sourcePath;
            QString destPath;
            QString description;
            bool required;
        };
        
        QList<KernelFile> kernelFiles;
        
        // Main kernel files in /boot
        kernelFiles.append({QString("/boot/vmlinuz-%1").arg(currentKernel), 
                           isDeviceInstall ? destPath + "/boot/" : destPath + "/",
                           "Kernel image", true});
        kernelFiles.append({QString("/boot/initrd.img-%1").arg(currentKernel), 
                           isDeviceInstall ? destPath + "/boot/" : destPath + "/",
                           "Initial ramdisk", true});
        kernelFiles.append({QString("/boot/System.map-%1").arg(currentKernel), 
                           isDeviceInstall ? destPath + "/boot/" : destPath + "/",
                           "System map", false});
        kernelFiles.append({QString("/boot/config-%1").arg(currentKernel), 
                           isDeviceInstall ? destPath + "/boot/" : destPath + "/",
                           "Kernel configuration", false});
        
        // Kernel headers (if doing device install)
        if (isDeviceInstall) {
            QString headersPath = QString("/usr/src/linux-headers-%1").arg(currentKernel);
            if (QDir(headersPath).exists()) {
                kernelFiles.append({headersPath, 
                                  destPath + "/usr/src/",
                                  "Kernel headers", false});
            }
            
            // Kernel modules
            QString modulesPath = QString("/lib/modules/%1").arg(currentKernel);
            if (QDir(modulesPath).exists()) {
                kernelFiles.append({modulesPath, 
                                  destPath + "/lib/modules/",
                                  "Kernel modules", false});
            }
            
            // Kernel source (if available)
            QString sourcePath = QString("/usr/src/linux-source-%1").arg(currentKernel.split('-').first());
            if (QDir(sourcePath).exists()) {
                kernelFiles.append({sourcePath, 
                                  destPath + "/usr/src/",
                                  "Kernel source", false});
            }
        }
        
        // Create destination directories
        for (const auto &file : kernelFiles) {
            QDir().mkpath(file.destPath);
        }
        
        int totalFiles = kernelFiles.size();
        int completedFiles = 0;
        
        for (const auto &file : kernelFiles) {
            if (progress.wasCanceled()) {
                return;
            }
            
            progress.setLabelText(QString("Copying %1...").arg(file.description));
            progress.setValue((completedFiles * 100) / totalFiles);
            
            QFileInfo sourceInfo(file.sourcePath);
            
            if (sourceInfo.exists()) {
                QString destFile;
                
                if (sourceInfo.isDir()) {
                    // Copy directory recursively
                    QString dirName = sourceInfo.fileName();
                    destFile = file.destPath + dirName;
                    
                    // Remove existing directory if present
                    if (QDir(destFile).exists()) {
                        QDir(destFile).removeRecursively();
                    }
                    
                    // Use cp -r for directory copying
                    QProcess copyProcess;
                    copyProcess.start("cp", QStringList() << "-r" << file.sourcePath << file.destPath);
                    copyProcess.waitForFinished(30000); // 30 second timeout
                    
                    if (copyProcess.exitCode() != 0) {
                        QString error = QString("Failed to copy %1: %2").arg(file.description).arg(QString(copyProcess.readAllStandardError()));
                        if (file.required) {
                            errorMessage = error;
                            success = false;
                            break;
                        } else {
                            m_statusLabel->setText(QString("Warning: %1").arg(error));
                        }
                    }
                } else {
                    // Copy single file
                    QString fileName = sourceInfo.fileName();
                    destFile = file.destPath + fileName;
                    
                    if (QFile::exists(destFile)) {
                        QFile::remove(destFile);
                    }
                    
                    if (!QFile::copy(file.sourcePath, destFile)) {
                        QString error = QString("Failed to copy %1").arg(file.description);
                        if (file.required) {
                            errorMessage = error;
                            success = false;
                            break;
                        } else {
                            m_statusLabel->setText(QString("Warning: %1").arg(error));
                        }
                    }
                }
            } else {
                QString error = QString("%1 not found at %2").arg(file.description).arg(file.sourcePath);
                if (file.required) {
                    errorMessage = error;
                    success = false;
                    break;
                } else {
                    m_statusLabel->setText(QString("Warning: %1").arg(error));
                }
            }
            
            completedFiles++;
        }
        
        progress.setValue(100);
        
        if (success) {
            QString message = QString("Successfully copied kernel %1 to %2").arg(currentKernel).arg(destPath);
            if (isDeviceInstall) {
                message += "\n\nNote: You may need to update GRUB on the target device.";
            }
            QMessageBox::information(this, "Success", message);
            if (!isDeviceInstall) {
                onRefreshKernels(); // Refresh kernel list if copied to tweaker dir
            }
        } else {
            QMessageBox::critical(this, "Error", errorMessage);
        }
    }
}

void KernelManager::onBackupKernel()
{
    QListWidgetItem *item = m_kernelList->currentItem();
    if (!item) return;
    
    QString kernelVersion = cleanKernelVersion(item->text());
    
    // Create backup dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Back Up Kernel");
    dialog.setFixedSize(600, 400);
    dialog.setStyleSheet("background-color: #DCDCDC;");
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    
    // Title
    QLabel *titleLabel = new QLabel(QString("Back Up Kernel %1").arg(kernelVersion));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(titleLabel);
    
    // Info label
    QLabel *infoLabel = new QLabel("Create a backup archive of the selected kernel and its components:");
    infoLabel->setStyleSheet("color: #000000; margin: 10px;");
    dialogLayout->addWidget(infoLabel);
    
    // Backup options
    QGroupBox *optionsGroup = new QGroupBox("Backup Options");
    optionsGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    
    // What to backup
    QCheckBox *backupBootCheckbox = new QCheckBox("Backup boot files (vmlinuz, initrd, config, System.map)");
    backupBootCheckbox->setChecked(true);
    backupBootCheckbox->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(backupBootCheckbox);
    
    QCheckBox *backupModulesCheckbox = new QCheckBox("Backup kernel modules (/lib/modules)");
    backupModulesCheckbox->setChecked(true);
    backupModulesCheckbox->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(backupModulesCheckbox);
    
    QCheckBox *backupHeadersCheckbox = new QCheckBox("Backup kernel headers (/usr/src/linux-headers)");
    backupHeadersCheckbox->setChecked(false);
    backupHeadersCheckbox->setStyleSheet("color: #000000;");
    optionsLayout->addWidget(backupHeadersCheckbox);
    
    // Destination selection
    QLabel *destLabel = new QLabel("Backup destination:");
    destLabel->setStyleSheet("color: #000000; margin-top: 10px;");
    optionsLayout->addWidget(destLabel);
    
    QHBoxLayout *destLayout = new QHBoxLayout();
    QLineEdit *destEdit = new QLineEdit(QDir::homePath() + "/kernel-backups/");
    destEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    destLayout->addWidget(destEdit);
    
    QPushButton *browseDestButton = new QPushButton("📁 Browse");
    browseDestButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 4px; } QPushButton:hover { background-color: #E0E0E0; }");
    connect(browseDestButton, &QPushButton::clicked, [&]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Backup Destination", destEdit->text());
        if (!dir.isEmpty()) {
            destEdit->setText(dir + "/");
        }
    });
    destLayout->addWidget(browseDestButton);
    
    optionsLayout->addLayout(destLayout);
    
    dialogLayout->addWidget(optionsGroup);
    
    // Archive name
    QGroupBox *nameGroup = new QGroupBox("Archive Settings");
    nameGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *nameLayout = new QVBoxLayout(nameGroup);
    
    QLabel *nameLabel = new QLabel("Archive filename:");
    nameLabel->setStyleSheet("color: #000000;");
    nameLayout->addWidget(nameLabel);
    
    QString defaultName = QString("kernel-%1-backup-%2.tar.gz")
                         .arg(kernelVersion)
                         .arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
    
    QLineEdit *nameEdit = new QLineEdit(defaultName);
    nameEdit->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 1px solid #000000;");
    nameLayout->addWidget(nameEdit);
    
    dialogLayout->addWidget(nameGroup);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Create Backup");
    buttonBox->setStyleSheet(
        "QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; padding: 8px; }"
        "QPushButton:hover { background-color: #E0E0E0; }"
    );
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    dialogLayout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString backupDir = destEdit->text().trimmed();
        QString archiveName = nameEdit->text().trimmed();
        
        if (backupDir.isEmpty() || archiveName.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please specify backup destination and filename.");
            return;
        }
        
        // Ensure backup directory exists
        QDir().mkpath(backupDir);
        
        QString fullArchivePath = backupDir + "/" + archiveName;
        
        // Create progress dialog
        QProgressDialog progress("Creating kernel backup...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowTitle("Backup Progress");
        progress.setValue(0);
        
        // Prepare files to backup
        QStringList filesToBackup;
        QStringList backupSources;
        
        if (backupBootCheckbox->isChecked()) {
            QStringList bootFiles = {
                QString("/boot/vmlinuz-%1").arg(kernelVersion),
                QString("/boot/initrd.img-%1").arg(kernelVersion),
                QString("/boot/System.map-%1").arg(kernelVersion),
                QString("/boot/config-%1").arg(kernelVersion)
            };
            
            for (const QString &file : bootFiles) {
                if (QFile::exists(file)) {
                    backupSources.append(file);
                }
            }
        }
        
        if (backupModulesCheckbox->isChecked()) {
            QString modulesPath = QString("/lib/modules/%1").arg(kernelVersion);
            if (QDir(modulesPath).exists()) {
                backupSources.append(modulesPath);
            }
        }
        
        if (backupHeadersCheckbox->isChecked()) {
            QString headersPath = QString("/usr/src/linux-headers-%1").arg(kernelVersion);
            if (QDir(headersPath).exists()) {
                backupSources.append(headersPath);
            }
        }
        
        if (backupSources.isEmpty()) {
            QMessageBox::warning(this, "Error", "No kernel files found to backup.");
            return;
        }
        
        progress.setLabelText("Creating tar archive...");
        progress.setValue(25);
        
        // Create tar command
        QProcess tarProcess;
        QStringList tarArgs;
        tarArgs << "-czf" << fullArchivePath;
        tarArgs.append(backupSources);
        
        progress.setValue(50);
        
        tarProcess.start("tar", tarArgs);
        tarProcess.waitForFinished(60000); // 60 second timeout
        
        progress.setValue(100);
        
        if (tarProcess.exitCode() == 0) {
            QFileInfo archiveInfo(fullArchivePath);
            QString sizeStr = QString("%1 MB").arg(archiveInfo.size() / (1024.0 * 1024.0), 0, 'f', 1);
            
            QMessageBox::information(this, "Backup Complete", 
                QString("Kernel backup created successfully!\n\n"
                       "Location: %1\n"
                       "Size: %2\n"
                       "Files backed up: %3")
                       .arg(fullArchivePath)
                       .arg(sizeStr)
                       .arg(backupSources.size()));
        } else {
            QString error = QString(tarProcess.readAllStandardError());
            QMessageBox::critical(this, "Backup Failed", 
                QString("Failed to create backup archive.\n\nError: %1").arg(error));
        }
    }
}

QString KernelManager::cleanKernelVersion(const QString &rawVersion) const
{
    QString cleaned = rawVersion;
    
    // Remove emoji prefix if present
    if (cleaned.startsWith("🐧 ")) {
        cleaned = cleaned.mid(2);
    }
    
    // Remove "(Installed)" suffix if present
    if (cleaned.endsWith(" (Installed)")) {
        cleaned = cleaned.left(cleaned.length() - 12); // Remove " (Installed)"
    }
    
    return cleaned.trimmed();
}

void KernelManager::onModuleSelectionChanged() { /* Implementation */ }

void KernelManager::onShowJoshuaFixes()
{
    QMessageBox::information(this,
                             "Joshua's Fixes",
                             "Joshua's kernel fixes feature is coming soon.");
}