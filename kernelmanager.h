#ifndef KERNELMANAGER_H
#define KERNELMANAGER_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTabWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QRadioButton>

class SystemManager;
class QProgressDialog;

class KernelManager : public QWidget
{
    Q_OBJECT

public:
    explicit KernelManager(SystemManager *systemManager, QWidget *parent = nullptr);
    
    // Public accessors for settings
    QString getKernelDirectory() const { return m_kernelDirectory; }
    void setKernelDirectory(const QString &dir) { 
        m_kernelDirectory = dir; 
        m_kernelDirectoryEdit->setText(dir);
    }

signals:
    // Kernel Management
    void setDefaultKernelRequested(const QString &kernelVersion);
    void removeKernelRequested(const QString &kernelVersion);
    void updateInitramfsRequested(const QString &kernelVersion);
    void installKernelRequested(const QString &kernelPackage);
    void installKernelToDeviceRequested(const QString &kernelVersion, const QString &devicePath);
    
    // Kernel Patching
    void applyPatchRequested(const QString &patchFile);
    void revertPatchRequested(const QString &patchName);
    void createPatchRequested(const QString &originalFile, const QString &modifiedFile);
    
    // Live Configuration
    void applyKernelParameterRequested(const QString &parameter, const QString &value);
    void updateBootParametersRequested(const QStringList &parameters);
    void updateKernelConfigRequested(const QString &configOption, const QString &value);
    
    // Module Management
    void loadModuleRequested(const QString &moduleName);
    void unloadModuleRequested(const QString &moduleName);
    void blacklistModuleRequested(const QString &moduleName);

private slots:
    // Kernel Management
    void onRefreshKernels();
    void onSetDefaultKernel();
    void onRemoveKernel();
    void onUpdateInitramfs();
    void onInstallKernel();
    void onKernelSelectionChanged();
    void onUpdateGrub();
    void onViewKernelConfig();
    void onInstallKernelToDevice();
    void onUpdateGrubOnDevice();
    void onBrowseKernelDirectory();
    void onCopyCurrentKernel();
    void onBackupKernel();
    void onShowJoshuaFixes();
    
    // Patching
    void onApplyPatch();
    void onRevertPatch();
    void onCreatePatch();
    void onLoadPatchFile();
    void onRefreshPatches();
    
    // Live Configuration
    void onApplyKernelParameter();
    void onUpdateBootParameters();
    void onEditKernelConfig();
    void onSaveKernelConfig();
    
    // Module Management
    void onLoadModule();
    void onUnloadModule();
    void onBlacklistModule();
    void onRefreshModules();
    void onModuleSelectionChanged();

private:
    void setupUI();
    void createKernelManagementTab();
    void createPatchingTab();
    void createLiveConfigTab();
    void createModuleManagementTab();
    
    void performKernelInstallation(const QString &kernelVersion, 
                                   const QString &devicePath,
                                   bool mountRoot,
                                   bool updateGrub,
                                   bool copyModules,
                                   const QString &customMountPoint,
                                   bool isInstalledKernel,
                                   QProgressDialog *progress);
    
    void performGrubUpdate(const QString &mountPoint,
                          const QString &devicePath,
                          bool needsMount,
                          bool updateInitramfs,
                          QProgressDialog *progress);
    
    // Helper functions
    QString cleanKernelVersion(const QString &rawVersion) const;
    
    // UI Components - Main
    QTabWidget *m_tabWidget;
    QLabel *m_statusLabel;
    QString m_kernelDirectory;
    QLineEdit *m_kernelDirectoryEdit;
    QPushButton *m_browseKernelDirButton;
    QPushButton *m_copyCurrentKernelButton;
    QPushButton *m_backupKernelButton;
    
    // Kernel Management Tab
    QGroupBox *m_kernelListGroup;
    QGroupBox *m_kernelActionsGroup;
    QGroupBox *m_kernelDetailsGroup;
    QListWidget *m_kernelList;
    QLabel *m_currentKernelLabel;
    QLabel *m_defaultKernelLabel;
    QTextEdit *m_kernelDetailsText;
    QPushButton *m_refreshButton;
    QPushButton *m_setDefaultButton;
    QPushButton *m_removeButton;
    QPushButton *m_updateInitramfsButton;
    QPushButton *m_updateGrubButton;
    QPushButton *m_viewConfigButton;
    QPushButton *m_installKernelButton;
    QPushButton *m_installToDeviceButton;
    QPushButton *m_updateGrubOnDeviceButton;
    QPushButton *m_joshuaFixesButton;
    QComboBox *m_availableKernelsCombo;
    
    // Custom kernel download options
    QGroupBox *m_customKernelGroup;
    QRadioButton *m_remoteKernelRadio;
    QRadioButton *m_localKernelRadio;
    QLineEdit *m_remoteKernelEdit;
    QLineEdit *m_localKernelEdit;
    
    // Joshua's fixes
    QStringList m_selectedJoshuaFixes;
    
    // Patching Tab
    QGroupBox *m_patchListGroup;
    QGroupBox *m_patchActionsGroup;
    QListWidget *m_patchList;
    QListWidget *m_appliedPatchesList;
    QPushButton *m_loadPatchButton;
    QPushButton *m_applyPatchButton;
    QPushButton *m_revertPatchButton;
    QPushButton *m_createPatchButton;
    QTextEdit *m_patchPreviewText;
    
    // Live Configuration Tab
    QGroupBox *m_kernelParamsGroup;
    QGroupBox *m_bootParamsGroup;
    QGroupBox *m_configOptionsGroup;
    QListWidget *m_kernelParamsList;
    QLineEdit *m_paramNameEdit;
    QLineEdit *m_paramValueEdit;
    QPushButton *m_applyParamButton;
    QTextEdit *m_bootParamsEdit;
    QPushButton *m_updateBootParamsButton;
    QListWidget *m_configOptionsList;
    QTextEdit *m_configEditor;
    QPushButton *m_saveConfigButton;
    
    // Module Management Tab
    QGroupBox *m_loadedModulesGroup;
    QGroupBox *m_availableModulesGroup;
    QGroupBox *m_moduleActionsGroup;
    QListWidget *m_loadedModulesList;
    QListWidget *m_availableModulesList;
    QTextEdit *m_moduleInfoText;
    QPushButton *m_loadModuleButton;
    QPushButton *m_unloadModuleButton;
    QPushButton *m_blacklistModuleButton;
    QPushButton *m_refreshModulesButton;
    QLineEdit *m_moduleSearchEdit;
    
    // Backend
    SystemManager *m_systemManager;
    QStringList m_installedKernels;
    QStringList m_availableKernels;
    QStringList m_loadedModules;
    QStringList m_availableModules;
    QStringList m_appliedPatches;
    QString m_currentKernel;
    QString m_defaultKernel;
};

#endif // KERNELMANAGER_H