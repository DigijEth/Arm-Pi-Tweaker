#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QWidget>
#include <QGroupBox>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QProcess>
#include <QMap>

class SystemManager;

struct StorageDevice {
    QString device;          // e.g., /dev/sda
    QString mountPoint;      // e.g., /mnt/sda1
    QString filesystem;      // e.g., ext4, ntfs
    QString label;          // Volume label
    QString size;           // Total size
    QString used;           // Used space
    QString available;      // Available space
    bool isRemovable;       // USB, SD card, etc.
    bool isMounted;         // Currently mounted
    bool isSystemDrive;     // Contains the OS
};

class StorageManager : public QWidget
{
    Q_OBJECT

public:
    explicit StorageManager(SystemManager *systemManager, QWidget *parent = nullptr);

signals:
    void operationStarted(const QString &operation);
    void progressUpdated(int progress);
    void operationCompleted(bool success, const QString &message);

private slots:
    void scanStorageDevices();
    void onDeviceSelectionChanged();
    void onMountDevice();
    void onUnmountDevice();
    void onCopyLiveImage();
    void onBurnToSDCard();
    void onCreateSnapshot();
    void onDriveCopy();
    void updateDeviceInfo();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessOutput();

private:
    void setupUI();
    void createSystemInfoGroup();
    void createDeviceListGroup();
    void createActionsGroup();
    void createProgressGroup();
    
    void detectSystemInstallation();
    QString formatSize(qint64 bytes);
    bool isLiveSystem();
    void executeCommand(const QString &command, const QStringList &args);
    
    // UI Components
    QGroupBox *m_systemInfoGroup;
    QGroupBox *m_deviceListGroup;
    QGroupBox *m_actionsGroup;
    QGroupBox *m_progressGroup;
    
    // System info
    QLabel *m_systemLocationLabel;
    QLabel *m_systemTypeLabel;
    QLabel *m_bootDeviceLabel;
    
    // Device list
    QListWidget *m_deviceList;
    QTextEdit *m_deviceInfoText;
    QPushButton *m_refreshButton;
    QPushButton *m_mountButton;
    QPushButton *m_unmountButton;
    
    // Actions
    QPushButton *m_copyLiveImageButton;
    QPushButton *m_burnSDCardButton;
    QPushButton *m_createSnapshotButton;
    QPushButton *m_driveCopyButton;
    
    // Options
    QComboBox *m_targetDeviceCombo;
    QCheckBox *m_includeHomeCheck;
    QCheckBox *m_compressCheck;
    QCheckBox *m_verifyCheck;
    
    // Progress
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QTextEdit *m_logOutput;
    QPushButton *m_cancelButton;
    
    // Backend
    SystemManager *m_systemManager;
    QMap<QString, StorageDevice> m_devices;
    QString m_selectedDevice;
    QProcess *m_currentProcess;
    QTimer *m_scanTimer;
    
    // State
    bool m_isLiveSystem;
    QString m_systemDevice;
    QString m_currentOperation;
};

#endif // STORAGEMANAGER_H