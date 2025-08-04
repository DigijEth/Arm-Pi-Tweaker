#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <unistd.h>
#include <sys/types.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Check for root/sudo privileges
    if (geteuid() != 0) {
        // Check if we're already in a re-execution attempt
        if (argc > 1 && QString(argv[1]) == "--elevated-rerun") {
            QMessageBox::critical(nullptr, "Elevation Failed",
                "Failed to obtain elevated privileges.\n\n"
                "Please run the application manually with sudo:\n"
                "sudo " + QString(argv[0]));
            return 1;
        }
        
        // Prompt user about elevation
        QMessageBox msgBox;
        msgBox.setWindowTitle("Elevated Privileges Required");
        msgBox.setText("Arm-Pi Tweaker requires elevated privileges to function properly.");
        msgBox.setInformativeText("The application needs root access for:\n"
                                 "• Building custom images\n"
                                 "• Installing GPU drivers\n"
                                 "• Managing kernels\n"
                                 "• System upgrades\n\n"
                                 "Click OK to enter your password and continue.");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        
        if (msgBox.exec() == QMessageBox::Cancel) {
            return 0;
        }
        
        // Re-execute ourselves with pkexec for GUI authentication
        QStringList args;
        
        // For Qt applications, we need to pass display environment variables to pkexec
        args << "env";
        
        // Add environment variables for X11
        if (getenv("DISPLAY")) {
            args << QString("DISPLAY=%1").arg(getenv("DISPLAY"));
        }
        if (getenv("XAUTHORITY")) {
            args << QString("XAUTHORITY=%1").arg(getenv("XAUTHORITY"));
        }
        
        // Add environment variables for Wayland
        if (getenv("WAYLAND_DISPLAY")) {
            args << QString("WAYLAND_DISPLAY=%1").arg(getenv("WAYLAND_DISPLAY"));
        }
        if (getenv("XDG_RUNTIME_DIR")) {
            args << QString("XDG_RUNTIME_DIR=%1").arg(getenv("XDG_RUNTIME_DIR"));
        }
        
        // Add Qt platform
        if (getenv("QT_QPA_PLATFORM")) {
            args << QString("QT_QPA_PLATFORM=%1").arg(getenv("QT_QPA_PLATFORM"));
        }
        
        // Add the application path and flag
        args << argv[0];  // The application path
        args << "--elevated-rerun";  // Flag to prevent infinite loop
        
        // Try pkexec first (for GUI authentication)
        QProcess pkexecProcess;
        int exitCode = pkexecProcess.execute("pkexec", args);        if (exitCode != 0) {
            // If pkexec fails, show instructions
            QMessageBox::critical(nullptr, "Elevation Failed",
                "Could not automatically elevate privileges.\n\n"
                "Please run the application manually with sudo:\n"
                "sudo " + QString(argv[0]));
        }
        
        // Exit this non-elevated instance
        return 0;
    }
    
    // Set application properties
    app.setApplicationName("Arm-Pi Tweaker");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("Setec Labs");
    app.setOrganizationDomain("seteclabs.com");
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Apply custom ARM-Pi theme
    QPalette armPiPalette;
    armPiPalette.setColor(QPalette::Window, QColor(220, 220, 220));     // Background: Light Grey
    armPiPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));       // Text: Black
    armPiPalette.setColor(QPalette::Base, QColor(240, 240, 240));       // Input backgrounds: Lighter grey
    armPiPalette.setColor(QPalette::AlternateBase, QColor(200, 200, 200)); // Alternate backgrounds
    armPiPalette.setColor(QPalette::ToolTipBase, QColor(240, 240, 240));
    armPiPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    armPiPalette.setColor(QPalette::Text, QColor(0, 0, 0));             // Text: Black
    armPiPalette.setColor(QPalette::Button, QColor(200, 200, 200));     // Button background: Grey
    armPiPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));       // Button text: Black
    armPiPalette.setColor(QPalette::BrightText, QColor(255, 0, 255));   // Important text: #FF00FF
    armPiPalette.setColor(QPalette::Link, QColor(0, 0, 255));           // Links: Blue
    armPiPalette.setColor(QPalette::Highlight, QColor(0, 0, 0));        // Selection: Black
    armPiPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255)); // Selected text: White
    app.setPalette(armPiPalette);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}