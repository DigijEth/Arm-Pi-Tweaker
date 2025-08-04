#include "upgradewidget.h"
#include <QScrollArea>
#include <QFrame>
#include <QFont>
#include <QTime>
#include <iostream>

UpgradeWidget::UpgradeWidget(QWidget *parent)
    : QWidget(parent)
    , m_extractGroup(nullptr)
    , m_upgradeGroup(nullptr)
    , m_patchGroup(nullptr)
    , m_rollbackGroup(nullptr)
    , m_warningGroup(nullptr)
    , m_extractButton(nullptr)
    , m_upgradeButton(nullptr)
    , m_patchButton(nullptr)
    , m_rollbackButton(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_logOutput(nullptr)
{
    std::cout << "DEBUG: UpgradeWidget constructor started" << std::endl;
    setupUI();
    std::cout << "DEBUG: UpgradeWidget constructor completed" << std::endl;
}

void UpgradeWidget::setupUI()
{
    std::cout << "DEBUG: UpgradeWidget::setupUI() started" << std::endl;
    
    std::cout << "DEBUG: Creating main layout" << std::endl;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create scroll area for the content
    std::cout << "DEBUG: Creating scroll area" << std::endl;
    QScrollArea *scrollArea = new QScrollArea();
    
    std::cout << "DEBUG: Creating scroll content widget" << std::endl;
    QWidget *scrollContent = new QWidget();
    
    std::cout << "DEBUG: Creating scroll layout" << std::endl;
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    
    // Warning section
    std::cout << "DEBUG: Creating warning group box" << std::endl;
    m_warningGroup = new QGroupBox("⚠️ IMPORTANT WARNINGS");
    m_warningGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #FF00FF; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    std::cout << "DEBUG: Creating warning layout" << std::endl;
    QVBoxLayout *warningLayout = new QVBoxLayout(m_warningGroup);
    
    std::cout << "DEBUG: Creating warning labels" << std::endl;
    QLabel *warn1 = new QLabel("• GPU drivers will be auto-detected from /gpu directory");
    warn1->setStyleSheet("color: #FF00FF; font-weight: bold;");
    
    std::cout << "DEBUG: Adding warn1 to layout" << std::endl;
    warningLayout->addWidget(warn1);
    
    QLabel *warn2 = new QLabel("• System backup is created automatically before patching");
    warn2->setStyleSheet("color: #FF00FF; font-weight: bold;");
    
    std::cout << "DEBUG: Adding warn2 to layout" << std::endl;
    warningLayout->addWidget(warn2);
    
    QLabel *warn3 = new QLabel("• Internet connection required for Ubuntu upgrade");
    warn3->setStyleSheet("color: #FF00FF; font-weight: bold;");
    warningLayout->addWidget(warn3);
    
    QLabel *warn4 = new QLabel("• Process may take 30-60 minutes total");
    warn4->setStyleSheet("color: #FF00FF; font-weight: bold;");
    warningLayout->addWidget(warn4);
    
    QLabel *warn5 = new QLabel("• Use rollback if something goes wrong");
    warn5->setStyleSheet("color: #FF00FF; font-weight: bold;");
    warningLayout->addWidget(warn5);
    
    scrollLayout->addWidget(m_warningGroup);
    
    // Step 1: Create backup and restore image
    m_extractButton = new QPushButton("�");
    m_extractButton->setFixedSize(50, 50);
    m_extractButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_extractGroup = createStepGroup(
        "� Step 1: Create a System Restore Image and Backup",
        "Create comprehensive backup and restore image of your current system",
        m_extractButton,
        "This will launch the backup wizard to create a full system backup"
    );
    scrollLayout->addWidget(m_extractGroup);
    
    connect(m_extractButton, &QPushButton::clicked, this, &UpgradeWidget::extractDriversRequested);
    
    // Step 2: Select custom upgrade file
    m_upgradeButton = new QPushButton("�");
    m_upgradeButton->setFixedSize(50, 50);
    m_upgradeButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_upgradeGroup = createStepGroup(
        "📁 Step 2: Select Custom Upgrade File",
        "Browse and select your custom upgrade.dat file",
        m_upgradeButton,
        "This will open a file browser to select the upgrade.dat file"
    );
    scrollLayout->addWidget(m_upgradeGroup);
    
    connect(m_upgradeButton, &QPushButton::clicked, this, &UpgradeWidget::runUpgradeRequested);
    
    // Step 3: Apply upgrade with TweakerUEFI option
    m_patchButton = new QPushButton("�");
    m_patchButton->setFixedSize(50, 50);
    m_patchButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #000000; border: 2px solid #000000; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #E0E0E0; }");
    
    // Create step 3 group with checkbox
    QGroupBox *patchGroupWithCheck = new QGroupBox("🚀 Step 3: Apply Custom Upgrade");
    patchGroupWithCheck->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QHBoxLayout *patchMainLayout = new QHBoxLayout(patchGroupWithCheck);
    patchMainLayout->addWidget(m_patchButton);
    
    QVBoxLayout *patchTextLayout = new QVBoxLayout();
    QLabel *patchDescLabel = new QLabel("Apply the selected custom upgrade to your system");
    patchDescLabel->setWordWrap(true);
    patchDescLabel->setStyleSheet("color: #000000; margin: 5px;");
    patchTextLayout->addWidget(patchDescLabel);
    
    QCheckBox *tweakerUefiCheck = new QCheckBox("Include TweakerUEFI");
    tweakerUefiCheck->setStyleSheet("color: #000000; font-weight: bold; margin: 5px;");
    patchTextLayout->addWidget(tweakerUefiCheck);
    
    QLabel *patchHelpLabel = new QLabel("This will apply the custom upgrade with optional TweakerUEFI integration");
    patchHelpLabel->setWordWrap(true);
    patchHelpLabel->setStyleSheet("color: #666666; font-style: italic; margin: 5px;");
    patchTextLayout->addWidget(patchHelpLabel);
    
    patchMainLayout->addLayout(patchTextLayout);
    scrollLayout->addWidget(patchGroupWithCheck);
    
    connect(m_patchButton, &QPushButton::clicked, this, &UpgradeWidget::patchSystemRequested);
    
    // Step 4: Coming soon message
    m_rollbackButton = new QPushButton("🚧");
    m_rollbackButton->setFixedSize(50, 50);
    m_rollbackButton->setStyleSheet("QPushButton { background-color: #F0F0F0; color: #FFA500; border: 2px solid #000000; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_rollbackGroup = createStepGroup(
        "� Step 4: Coming Soon - Upgrades are Still Highly Unstable",
        "Advanced upgrade features are under development",
        m_rollbackButton,
        "Future versions will include stable upgrade functionality"
    );
    scrollLayout->addWidget(m_rollbackGroup);
    
    connect(m_rollbackButton, &QPushButton::clicked, this, &UpgradeWidget::rollbackRequested);
    
    // Progress and status section
    QGroupBox *statusGroup = new QGroupBox("📊 Progress & Status");
    statusGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    
    m_statusLabel = new QLabel("Ready - Select an operation above");
    m_statusLabel->setStyleSheet("font-weight: bold; color: #000000;");
    statusLayout->addWidget(m_statusLabel);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 2px solid #000000; border-radius: 5px; background-color: #F0F0F0; color: #000000; }"
        "QProgressBar::chunk { background-color: #000000; }"
    );
    m_progressBar->setVisible(false);
    statusLayout->addWidget(m_progressBar);
    
    m_logOutput = new QTextEdit();
    m_logOutput->setMaximumHeight(200);
    m_logOutput->setReadOnly(true);
    m_logOutput->setFont(QFont("Consolas", 9));
    m_logOutput->setStyleSheet("background-color: #F0F0F0; color: #000000; border: 2px solid #000000;");
    statusLayout->addWidget(m_logOutput);
    
    scrollLayout->addWidget(statusGroup);
    
    // Set up scroll area
    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    mainLayout->addWidget(scrollArea);
}

QGroupBox* UpgradeWidget::createStepGroup(const QString &title, const QString &description, 
                                         QPushButton *button, const QString &helpText)
{
    QGroupBox *group = new QGroupBox(title);
    group->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #000000; border: 2px solid #000000; "
        "border-radius: 5px; margin: 5px; padding-top: 10px; background-color: #DCDCDC; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QHBoxLayout *mainLayout = new QHBoxLayout(group);
    
    // Left side: button
    mainLayout->addWidget(button);
    
    // Right side: text content
    QVBoxLayout *textLayout = new QVBoxLayout();
    
    QLabel *descLabel = new QLabel(description);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #000000; font-size: 14px; font-weight: bold;");
    textLayout->addWidget(descLabel);
    
    QLabel *helpLabel = new QLabel(helpText);
    helpLabel->setStyleSheet("color: #00FFFF; font-size: 11px;");
    helpLabel->setWordWrap(true);
    textLayout->addWidget(helpLabel);
    
    mainLayout->addLayout(textLayout);
    
    return group;
}

void UpgradeWidget::updateProgress(int value)
{
    m_progressBar->setValue(value);
    if (value > 0 && !m_progressBar->isVisible()) {
        m_progressBar->setVisible(true);
    }
    if (value >= 100) {
        m_progressBar->setVisible(false);
    }
}

void UpgradeWidget::updateStatus(const QString &message)
{
    m_statusLabel->setText(message);
    m_logOutput->append(QString("[%1] %2").arg(QTime::currentTime().toString()).arg(message));
    
    // Auto-scroll to bottom
    QTextCursor cursor = m_logOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logOutput->setTextCursor(cursor);
}

void UpgradeWidget::setButtonsEnabled(bool enabled)
{
    m_extractButton->setEnabled(enabled);
    m_upgradeButton->setEnabled(enabled);
    m_patchButton->setEnabled(enabled);
    m_rollbackButton->setEnabled(enabled);
}