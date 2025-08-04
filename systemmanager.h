#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QStringList>

class SystemManager : public QObject
{
    Q_OBJECT

public:
    explicit SystemManager(QObject *parent = nullptr);
    
    void extractDrivers();
    void runUbuntuUpgrade();
    void patchSystem();
    void rollbackUpgrade();
    
    // GPU Management
    void installGpuDriver(const QString &driverPath);
    void removeGpuDriver(const QString &driverName);
    void switchGpuDriver(const QString &driverType);
    void testGpuDriver();
    QString detectCurrentGpuDriver();
    QStringList scanAvailableGpuDrivers();
    
    // Kernel Management
    void installKernel(const QString &kernelPackage);
    void removeKernel(const QString &kernelVersion);
    void setDefaultKernel(const QString &kernelVersion);
    void updateInitramfs(const QString &kernelVersion);
    void updateGrub();
    QStringList getInstalledKernels();
    QString getCurrentKernel();
    QString getDefaultKernel();
    
    // Kernel Patching
    void applyKernelPatch(const QString &patchFile);
    void revertKernelPatch(const QString &patchName);
    void createKernelPatch(const QString &originalFile, const QString &modifiedFile);
    QStringList getAppliedPatches();
    
    // Live Configuration
    void applyKernelParameter(const QString &parameter, const QString &value);
    void updateBootParameters(const QStringList &parameters);
    void updateKernelConfig(const QString &configOption, const QString &value);
    
    // Module Management
    void loadKernelModule(const QString &moduleName);
    void unloadKernelModule(const QString &moduleName);
    void blacklistKernelModule(const QString &moduleName);
    QStringList getLoadedModules();
    QStringList getAvailableModules();
    QString getModuleInfo(const QString &moduleName);

signals:
    void progressUpdated(int percentage);
    void statusUpdated(const QString &message);
    void operationCompleted(bool success, const QString &message);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessOutput();

private:
    bool runCommand(const QString &command, const QStringList &arguments = QStringList());
    bool checkPrerequisites();
    bool checkUpgradePrerequisites();
    bool prepareSystemForUpgrade();
    QString getUpgradeSourcePath();
    void createBackup();
    bool checkDiskSpace();
    bool updatePackageLists();
    bool fixBrokenPackages();
    QString detectGpuDrivers();
    QStringList findFilesInDirectory(const QString &directory, const QStringList &patterns);
    
    QProcess *m_currentProcess;
    QString m_currentOperation;
    QTimer *m_progressTimer;
    int m_simulatedProgress;
};

#endif // SYSTEMMANAGER_H