#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QScrollArea>
#include <QStatusBar>
#include <QSplitter>
#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>

#include "imagebuilder.h"

class UpgradeWidget;
class SystemManager;
class GpuManager;
class KernelManager;
class StorageManager;
class CustomImageWizard;
class UefiTab;
class RockyTab;
class WelcomeTab;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onExtractDrivers();
    void onRunUpgrade();
    void onPatchSystem();
    void onRollbackUpgrade();
    void showAbout();
    void showSettings();
    void onImageBuilderLogMessage(const QString &message);
    void onSwitchToStatusTab();
    
    // Menu/toolbar actions
    void toggleTabsVisibility(bool visible);
    void showAllWidgetsMenu();

    // Slots for ImageBuilder signals
    void handleRequestDownloadDir();
    void handleRequestBuildConfirmation(const QString &kernelPath);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void updateDynamicSizes();
    int calculateFontSize(int baseSize);
    void setupMenuBar();
    void setupTabs();
    void setupWelcomeTab();
    void setupUpgradeTab();
    void setupImageEditorTab();
    void setupKernelManagerTab();
    void setupGpuManagerTab();
    void setupStorageTab();
    void setupUefiTab();
    void setupSystemTweaksTab();
    void setupStatusTab();
    void setupRockyTab();
    
    // UI Components
    QTabWidget *m_tabWidget;
    WelcomeTab *m_welcomeTab;
    QWidget *m_upgradeTab;
    QWidget *m_imageEditorTab;
    QWidget *m_kernelManagerTab;
    QWidget *m_gpuManagerTab;
    QWidget *m_storageTab;
    QWidget *m_uefiTab;
    QWidget *m_systemTweaksTab;
    QWidget *m_statusTab;
    RockyTab *m_rockyTab;
    
    // Upgrade Tab Components
    UpgradeWidget *m_upgradeWidget;
    QTextEdit *m_statusOutput;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // Manager Components
    GpuManager *m_gpuManager;
    KernelManager *m_kernelManager;
    StorageManager *m_storageManager;
    CustomImageWizard *m_customImageWizard;
    UefiTab *m_uefiTabWidget;
    
    // Backend
    SystemManager *m_systemManager;
    QString m_githubToken;
    // How to store GitHub token
    enum TokenStorageMode { MemoryOnly, EncryptedAtRest };
    TokenStorageMode m_tokenStorageMode;
};

#endif // MAINWINDOW_H