#include "welcometab.h"
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QSpacerItem>

WelcomeTab::WelcomeTab(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
{
    setupUI();
}

WelcomeTab::~WelcomeTab()
{
}

void WelcomeTab::setupUI()
{
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Scroll area for content
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Content widget
    m_contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    // Setup sections
    setupWelcomeSection();
    setupWarningSection();
    setupRockySection();
    setupQuickStartSection();
    setupFooterSection();
    
    // Add sections to content layout
    contentLayout->addWidget(m_welcomeGroup);
    contentLayout->addWidget(m_warningGroup);
    contentLayout->addWidget(m_rockyGroup);
    contentLayout->addWidget(m_quickStartGroup);
    contentLayout->addStretch();
    contentLayout->addWidget(m_footerGroup);
    
    // Set content widget to scroll area
    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);
}

void WelcomeTab::setupWelcomeSection()
{
    m_welcomeGroup = new QGroupBox("Welcome to ARM Pi Tweaker & Rocky AI");
    m_welcomeGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 16px; color: #2E86AB; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_welcomeGroup);
    
    // Title
    m_titleLabel = new QLabel("🔧 ARM Pi Tweaker & Rocky AI Development Suite");
    m_titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #1F5F99; "
        "padding: 10px; margin: 10px;"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_titleLabel);
    
    // Version
    m_versionLabel = new QLabel("Version 1.0.0 Alpha - Orange Pi 5 Plus Edition");
    m_versionLabel->setStyleSheet(
        "font-size: 14px; font-style: italic; color: #666; "
        "padding: 5px;"
    );
    m_versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_versionLabel);
    
    // Welcome message
    m_welcomeText = new QLabel();
    m_welcomeText->setText(
        "<html><body style='font-size: 14px; line-height: 1.6;'>"
        "<p><b>Thank you for using ARM Pi Tweaker!</b></p>"
        "<p>This comprehensive development suite is specifically designed for the "
        "<b>Orange Pi 5 Plus with RK3588S chipset</b>. We've created this tool to fill "
        "the critical gap left by Rockchip's lack of official documentation and development resources.</p>"
        "<p><b>What you can do with this tool:</b></p>"
        "<ul>"
        "<li><b>Custom Kernel Development</b> - Build and deploy custom Linux kernels</li>"
        "<li><b>Live Image Creation</b> - Generate bootable images for various use cases</li>"
        "<li><b>Driver Development</b> - Optimize Mesa PanVK and other GPU drivers</li>"
        "<li><b>AI-Powered Optimization</b> - Use Rocky AI for intelligent system tuning</li>"
        "<li><b>System Monitoring</b> - Real-time performance analysis and optimization</li>"
        "</ul>"
        "<p>We're excited to have you as part of our community of Orange Pi 5 Plus developers!</p>"
        "</body></html>"
    );
    m_welcomeText->setWordWrap(true);
    m_welcomeText->setStyleSheet("padding: 15px; background-color: #f8f9fa; border-radius: 8px;");
    layout->addWidget(m_welcomeText);
}

void WelcomeTab::setupWarningSection()
{
    m_warningGroup = new QGroupBox("⚠️ CRITICAL SAFETY WARNING");
    m_warningGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 16px; color: #DC3545; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_warningGroup);
    
    m_warningTitle = new QLabel("🚨 EXPERIMENTAL SOFTWARE - READ CAREFULLY 🚨");
    m_warningTitle->setStyleSheet(
        "font-size: 18px; font-weight: bold; color: #DC3545; "
        "padding: 10px; text-align: center;"
    );
    m_warningTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_warningTitle);
    
    m_warningText = new QTextEdit();
    m_warningText->setReadOnly(true);
    m_warningText->setMaximumHeight(200);
    m_warningText->setHtml(
        "<html><body style='font-size: 13px; color: #721c24;'>"
        "<p><b>THIS SOFTWARE IS HIGHLY EXPERIMENTAL AND CAN CAUSE PERMANENT DAMAGE</b></p>"
        "<p><b>CRITICAL RISKS:</b></p>"
        "<ul>"
        "<li><b>Device Malfunction:</b> Improper use may render your Orange Pi 5 Plus completely unbootable</li>"
        "<li><b>Data Loss:</b> All data on your device could be permanently lost</li>"
        "<li><b>Hardware Damage:</b> Incorrect kernel modifications can damage hardware components</li>"
        "<li><b>System Instability:</b> Experimental features may cause crashes and instability</li>"
        "</ul>"
        "<p><b>DISCLAIMER:</b></p>"
        "<p style='background-color: #f5c6cb; padding: 10px; border-radius: 5px;'>"
        "<b>The creator of this application is NOT RESPONSIBLE for any:</b><br>"
        "• Lost or damaged files<br>"
        "• Device malfunctions or failures<br>"
        "• Hardware damage or destruction<br>"
        "• Data corruption or loss<br>"
        "• System instability or crashes<br>"
        "<b>USE AT YOUR OWN RISK!</b>"
        "</p>"
        "<p><b>BEFORE PROCEEDING:</b></p>"
        "<ul>"
        "<li>Create a <b>FULL SYSTEM BACKUP</b> before making ANY changes</li>"
        "<li>Have a recovery plan and bootable backup image ready</li>"
        "<li>Understand that you are solely responsible for any consequences</li>"
        "<li>Consider using a dedicated development device</li>"
        "</ul>"
        "</body></html>"
    );
    m_warningText->setStyleSheet(
        "background-color: #f8d7da; border: 2px solid #DC3545; "
        "border-radius: 8px; padding: 10px;"
    );
    layout->addWidget(m_warningText);
}

void WelcomeTab::setupRockySection()
{
    m_rockyGroup = new QGroupBox("🤖 Meet Rocky AI - Your Development Assistant");
    m_rockyGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 16px; color: #28A745; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_rockyGroup);
    
    m_rockyTitle = new QLabel("🚧 Rocky AI - Extremely Early Development 🚧");
    m_rockyTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #FFC107; "
        "padding: 10px; background-color: #FFF3CD; border-radius: 5px;"
    );
    m_rockyTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_rockyTitle);
    
    m_rockyDescription = new QTextEdit();
    m_rockyDescription->setReadOnly(true);
    m_rockyDescription->setMaximumHeight(180);
    m_rockyDescription->setHtml(
        "<html><body style='font-size: 13px; line-height: 1.5;'>"
        "<p><b>What is Rocky AI?</b></p>"
        "<p>Rocky is an integrated AI designed to aid in Orange Pi 5 Plus development. Built on llama.cpp with specialized knowledge of RK3588S hardware and Mesa PanVK drivers.</p>"
        "<p><b>Current Capabilities (Alpha Stage):</b></p>"
        "<ul>"
        "<li><b>Interactive Chat:</b> Ask questions about kernel configuration and driver issues</li>"
        "<li><b>Real-time Monitoring:</b> Live system performance tracking and analysis</li>"
        "<li><b>Code Assistance:</b> Basic help with driver development and optimization</li>"
        "<li><b>System Analysis:</b> Hardware configuration recommendations</li>"
        "</ul>"
        "<p><b>⚠️ Development Status:</b></p>"
        "<p style='background-color: #fff3cd; padding: 8px; border-radius: 4px;'>"
        "<b>Rocky AI is in EXTREMELY EARLY DEVELOPMENT.</b> Expect limited functionality, "
        "potential bugs, and incomplete features. This is primarily a technology demonstration "
        "and should not be relied upon for critical development tasks."
        "</p>"
        "<p><b>Long-term Goals:</b></p>"
        "<p>Rocky's ultimate goal is to be integrated into the OS with custom drivers to capture as much data as possible. "
        "This will enable us to create the best drivers and optimize the system to ensure it runs at full capacity. "
        "We're specifically focused on developing Mali G610 drivers that are 100% functional and achieving complete system optimization.</p>"
        "</body></html>"
    );
    m_rockyDescription->setStyleSheet(
        "background-color: #d4edda; border: 1px solid #28A745; "
        "border-radius: 8px; padding: 10px;"
    );
    layout->addWidget(m_rockyDescription);
}

void WelcomeTab::setupQuickStartSection()
{
    m_quickStartGroup = new QGroupBox("🚀 Quick Start Guide");
    m_quickStartGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 16px; color: #6F42C1; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_quickStartGroup);
    
    m_quickStartTitle = new QLabel("📋 Step-by-Step Guide for Each Feature");
    m_quickStartTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #6F42C1; padding: 5px;"
    );
    layout->addWidget(m_quickStartTitle);
    
    m_quickStartGuide = new QTextEdit();
    m_quickStartGuide->setReadOnly(true);
    m_quickStartGuide->setMaximumHeight(300);
    m_quickStartGuide->setHtml(
        "<html><body style='font-size: 12px; line-height: 1.4;'>"
        "<p><b>🔧 System Management & Backup:</b></p>"
        "<ol>"
        "<li><b>Create Backup:</b> Go to 'Upgrade' tab → 'Create System Restore Image' → Follow backup wizard</li>"
        "<li><b>Custom Images:</b> Use 'Custom Images' tab to build bootable images</li>"
        "<li><b>UEFI Management:</b> Configure advanced boot options in 'UEFI' tab</li>"
        "</ol>"
        "<p><b>🤖 Rocky AI Chat:</b></p>"
        "<ol>"
        "<li><b>Load Model:</b> Go to 'Rocky' tab → Click 'Load Model' → Select GGUF model file</li>"
        "<li><b>Start Chatting:</b> Type your questions about Orange Pi development</li>"
        "<li><b>Monitor System:</b> Watch real-time debug output in the console below</li>"
        "</ol>"
        "<p><b>⚙️ Driver Development:</b></p>"
        "<ol>"
        "<li><b>GPU Manager:</b> Monitor and configure Mesa PanVK drivers</li>"
        "<li><b>Kernel Manager:</b> Build custom kernels with Orange Pi patches</li>"
        "<li><b>Storage Manager:</b> Manage storage devices and partitions</li>"
        "</ol>"
        "<p><b>🔬 Advanced Features:</b></p>"
        "<ol>"
        "<li><b>System Monitoring:</b> Real-time performance tracking (coming soon)</li>"
        "<li><b>Driver Optimization:</b> AI-powered Mesa PanVK tuning (in development)</li>"
        "<li><b>Custom Hardware:</b> Device tree and GPIO configuration</li>"
        "</ol>"
        "<p><b>⚡ Pro Tips:</b></p>"
        "<ul>"
        "<li>Always create backups before making system changes</li>"
        "<li>Start with basic features before attempting advanced modifications</li>"
        "<li>Use Rocky AI for guidance when unsure about configurations</li>"
        "<li>Monitor system logs for any issues or warnings</li>"
        "<li>Keep recovery images on separate storage devices</li>"
        "</ul>"
        "</body></html>"
    );
    m_quickStartGuide->setStyleSheet(
        "background-color: #e7e3ff; border: 1px solid #6F42C1; "
        "border-radius: 8px; padding: 10px;"
    );
    layout->addWidget(m_quickStartGuide);
    
    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_quickStartButton = new QPushButton("🚀 Interactive Quick Start");
    m_quickStartButton->setStyleSheet(
        "QPushButton { background-color: #6F42C1; color: white; font-weight: bold; "
        "padding: 10px 20px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #5a359a; }"
    );
    connect(m_quickStartButton, &QPushButton::clicked, this, &WelcomeTab::onQuickStartClicked);
    buttonLayout->addWidget(m_quickStartButton);
    
    m_docButton = new QPushButton("📚 Full Documentation");
    m_docButton->setStyleSheet(
        "QPushButton { background-color: #17A2B8; color: white; font-weight: bold; "
        "padding: 10px 20px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #138496; }"
    );
    connect(m_docButton, &QPushButton::clicked, this, &WelcomeTab::onDocumentationClicked);
    buttonLayout->addWidget(m_docButton);
    
    m_communityButton = new QPushButton("🌐 Join Community");
    m_communityButton->setStyleSheet(
        "QPushButton { background-color: #28A745; color: white; font-weight: bold; "
        "padding: 10px 20px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #218838; }"
    );
    connect(m_communityButton, &QPushButton::clicked, this, &WelcomeTab::onCommunityClicked);
    buttonLayout->addWidget(m_communityButton);
    
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
}

void WelcomeTab::setupFooterSection()
{
    m_footerGroup = new QGroupBox();
    m_footerGroup->setStyleSheet("QGroupBox { border: none; }");
    
    QVBoxLayout *layout = new QVBoxLayout(m_footerGroup);
    
    m_footerText = new QLabel();
    m_footerText->setText(
        "<html><body style='font-size: 12px; color: #666; text-align: center;'>"
        "<p><b>ARM Pi Tweaker & Rocky AI Development Suite</b><br>"
        "Built with ❤️ by the ARM development community for Orange Pi 5 Plus enthusiasts</p>"
        "<p>🔧 <b>Hardware Target:</b> Orange Pi 5 Plus (RK3588S)<br>"
        "🤖 <b>AI Framework:</b> llama.cpp with custom Orange Pi optimizations<br>"
        "⚡ <b>Purpose:</b> Fill the gap left by lack of official Rockchip documentation</p>"
        "</body></html>"
    );
    m_footerText->setAlignment(Qt::AlignCenter);
    m_footerText->setWordWrap(true);
    layout->addWidget(m_footerText);
    
    m_licenseText = new QLabel();
    m_licenseText->setText(
        "<html><body style='font-size: 11px; color: #999; text-align: center;'>"
        "<p>⚖️ Licensed under MIT License | "
        "⚠️ Use at your own risk | "
        "🔒 No warranty provided</p>"
        "<p><i>Remember: Always backup your system before making modifications!</i></p>"
        "</body></html>"
    );
    m_licenseText->setAlignment(Qt::AlignCenter);
    m_licenseText->setWordWrap(true);
    layout->addWidget(m_licenseText);
}

// Slot implementations
void WelcomeTab::onQuickStartClicked()
{
    QMessageBox::information(this, "Interactive Quick Start",
        "The interactive quick start tutorial will be available in the next update.\n\n"
        "For now, please follow the step-by-step guide above or check the full documentation.");
}

void WelcomeTab::onDocumentationClicked()
{
    QMessageBox::information(this, "Documentation",
        "Opening the comprehensive README.md file...\n\n"
        "You can also find detailed documentation in the docs/ folder of the project repository.");
    
    // Try to open README.md if available
    QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/../README.md"));
}

void WelcomeTab::onCommunityClicked()
{
    QMessageBox::information(this, "Community Support",
        "Community channels are being set up!\n\n"
        "Coming soon:\n"
        "• Discord server for real-time chat\n"
        "• Matrix room for open-source communication\n"
        "• GitHub discussions for development topics\n"
        "• Wiki for collaborative documentation\n\n"
        "For now, please use GitHub issues for bug reports and feature requests.");
}
