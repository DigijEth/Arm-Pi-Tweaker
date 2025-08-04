
#ifndef BACKUPWIZARD_H
#define BACKUPWIZARD_H

#include <QDialog>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTextEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QProgressBar>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include <QTreeWidgetItem>

class StorageManager;

enum class BackupType {
    LiveBootBackup,
    CompressedWholeDisk,
    Custom
};

enum class DeviceType {
    LocalBackup,
    SDCard,
    USB,
    Network,
    Cloud
};

struct BackupDevice {
    QString name;
    QString path;
    QString filesystem;
    QString size;
    QString available;
    DeviceType type;
    bool isSupported;
};

class BackupWizard : public QDialog
{
    Q_OBJECT

public:
    explicit BackupWizard(StorageManager *storageManager, QWidget *parent = nullptr);
    ~BackupWizard();

    // Public interface for different entry points
    void startBackupWizard();
    void startRestoreWizard();
    void startImageCreationWizard();

signals:
    void backupStarted();
    void backupProgress(int percentage);
    void backupCompleted(bool success, const QString &message);
    void wizardCancelled();

private slots:
    void onNextClicked();
    void onBackClicked();
    void onCancelClicked();
    void onSkipRisksClicked();
    void onContinueClicked();
    void onDeviceSelectionChanged();
    void onBackupTypeChanged();
    void onWholeSystemClicked();
    void onAddFoldersClicked();
    void onAddFilesClicked();
    void onRemoveFoldersClicked();
    void onRemoveFilesClicked();
    void onFileSelectionChanged();
    void onFormatDeviceClicked();
    void onMakeBootableChanged(bool enabled);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void updateProgress();

private:
    enum WizardPage {
        WarningPage = 0,
        DeviceSelectionPage,
        BackupTypePage,
        FileSelectionPage,
        TargetDevicePage,
        FormatPage,
        ConfirmationPage,
        ProgressPage
    };

    void setupUI();
    void setupWarningPage();
    void setupDeviceSelectionPage();
    void setupBackupTypePage();
    void setupFileSelectionPage();
    void setupTargetDevicePage();
    void setupFormatPage();
    void setupConfirmationPage();
    void setupProgressPage();

    void populateDeviceList();
    void populateFileTree();
    void updateSelectedFiles();
    void calculateSpaceRequirements();
    bool validateCurrentPage();
    void updateNavigationButtons();
    void updateConfirmationPage();
    QString formatSize(qint64 bytes);
    QStringList getSelectedFiles();
    bool hasEnoughSpace();
    void startBackupProcess();
    void performLiveBootBackup();
    void performCompressedBackup();
    void performCustomBackup();

    // UI Components
    QStackedWidget *m_pageStack;
    QPushButton *m_backButton;
    QPushButton *m_nextButton;
    QPushButton *m_cancelButton;
    QPushButton *m_skipButton; // For risk warning

    // Warning Page
    QWidget *m_warningPage;
    QLabel *m_warningLabel;
    QPushButton *m_continueButton;

    // Device Selection Page
    QWidget *m_deviceSelectionPage;
    QListWidget *m_deviceList;
    QButtonGroup *m_deviceButtonGroup;
    QLabel *m_deviceInfoLabel;

    // Backup Type Page
    QWidget *m_backupTypePage;
    QRadioButton *m_liveBootRadio;
    QRadioButton *m_compressedRadio;
    QRadioButton *m_customRadio;
    QButtonGroup *m_backupTypeGroup;
    QLabel *m_backupTypeDescription;

    // File Selection Page
    QWidget *m_fileSelectionPage;
    QTreeWidget *m_fileTree;
    QPushButton *m_wholeSystemButton;
    QPushButton *m_addFoldersButton;
    QPushButton *m_addFilesButton;
    QPushButton *m_removeFoldersButton;
    QPushButton *m_removeFilesButton;
    QTextEdit *m_selectedFilesText;
    QLabel *m_spaceRequiredLabel;
    QLabel *m_spaceAvailableLabel;

    // Target Device Page
    QWidget *m_targetDevicePage;
    QComboBox *m_targetDeviceCombo;
    QCheckBox *m_makeBootableCheck;
    QComboBox *m_imageFormatCombo;

    // Format Page
    QWidget *m_formatPage;
    QLabel *m_formatWarningLabel;
    QComboBox *m_formatTypeCombo;
    QLabel *m_formatInfoLabel;

    // Confirmation Page
    QWidget *m_confirmationPage;
    QTextEdit *m_confirmationText;
    QLabel *m_finalSpaceLabel;

    // Progress Page
    QWidget *m_progressPage;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    QTextEdit *m_progressLog;

    // Backend
    StorageManager *m_storageManager;
    QProcess *m_currentProcess;
    QTimer *m_progressTimer;

    // State
    QList<BackupDevice> m_availableDevices;
    BackupDevice m_selectedDevice;
    BackupType m_selectedBackupType;
    QStringList m_selectedFiles;
    QString m_targetPath;
    bool m_makeBootable;
    QString m_imageFormat;
    qint64 m_spaceRequired;
    qint64 m_spaceAvailable;
    int m_currentPage;
};

#endif // BACKUPWIZARD_H
