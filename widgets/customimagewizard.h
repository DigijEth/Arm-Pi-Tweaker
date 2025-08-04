#ifndef CUSTOMIMAGEWIZARD_H
#define CUSTOMIMAGEWIZARD_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QProcess>
#include <QGroupBox>
#include <QRadioButton>
#include <QStackedWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../imagebuilder.h"

class CustomImageWizard : public QWidget
{
    Q_OBJECT

public:
    explicit CustomImageWizard(QWidget *parent = nullptr);
    ~CustomImageWizard();
    // Set GitHub API token for authenticated requests
    void setGithubToken(const QString &token);

signals:
    void logMessage(const QString &message);
    void buildProgress(int value);
    void buildFinished(bool success);
    void switchToStatusTab();

private slots:
    void onBuildClicked();
    void onSourceTypeChanged();
    void onDownloadSourceClicked();
    void onBrowseLocalSourceClicked();
    void onBrowseKernelSourceClicked();
    void onBrowseLocalKernelSourceClicked();
    void onBrowseOutputDirClicked();
    void onRefreshBranchesClicked();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onProcessOutput();
    void onProcessError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void validateInputs();
    void onRefreshRockchipBranchesClicked();
    void updateBuildSteps();
    void updateBuildStepDisplay(const QString &step, const QString &description);

private:
    void setupUI();
    void downloadSource();
    void cloneRepository(const QString &url, const QString &branch, const QString &targetDir);
    void fetchBranches();
    void fetchRockchipBranches();
    void startBuildProcess();
    void buildKernel();
    void buildUBoot();
    void buildRootfs();
    void createImage();
    void executeCommand(const QString &command, const QString &workDir = QString());
    QString getLivecdRootfsBranch(const QString &suite);
    
    // UI Components - Live Image Creator (formerly Source Selection)
    QGroupBox *m_sourceGroup;
    
    // Live Image Provider/Branch controls (for Ubuntu base system)
    QComboBox *m_liveImageProviderCombo;
    QComboBox *m_branchCombo;  // This is the live image branch combo
    QPushButton *m_refreshBranchesButton;
    QCheckBox *m_customLiveImageCheck;
    QLineEdit *m_customLiveImageEdit;
    
    // Source type controls (converted from radio to checkboxes)
    QCheckBox *m_localSourceCheck;
    QCheckBox *m_kernelOnlyCheck;
    
    QStackedWidget *m_sourceStack;
    QStackedWidget *m_kernelSourceStack;
    
    // Local source widgets
    QLineEdit *m_localSourceEdit;
    QPushButton *m_browseLocalButton;
    
    // Download source widgets (legacy - may be removed)
    QProgressBar *m_downloadProgress;
    QLabel *m_downloadStatusLabel;
    
    // Custom kernel widgets (legacy - removed)
    
    // Kernel Build specific options
    QGroupBox *m_kernelBuildGroup;
    QComboBox *m_rockchipBranchCombo;
    QPushButton *m_refreshRockchipBranchesButton;
    QCheckBox *m_customKernelCheck;
    QLineEdit *m_customKernelEdit;
    QCheckBox *m_localKernelCheck;
    QLineEdit *m_localKernelSourceEdit;
    QPushButton *m_browseLocalKernelButton;
    QCheckBox *m_enableF2fsCheck;
    QCheckBox *m_enableCompressionCheck;
    QCheckBox *m_enableDebugCheck;
    
    // Joshua Riek's kernel patches
    QCheckBox *m_applyKernelPatchesCheck;  // For kernel-only build
    QPushButton *m_selectKernelPatchesButton;
    QCheckBox *m_applyMainPatchesCheck;    // For full build in main frame
    QPushButton *m_selectMainPatchesButton;
    QStringList m_selectedPatches;
    
    // Additional kernel-related widgets for new structure
    
    // Kernel-only build source type selection
    QRadioButton *m_kernelLocalSourceRadio;
    QRadioButton *m_kernelOnlineSourceRadio;
    
    // Build configuration
    QComboBox *m_suiteCombo;
    QComboBox *m_flavorCombo;
    QComboBox *m_partitionTypeCombo;
    
    // Build options
    QCheckBox *m_cleanBuildCheck;
    QCheckBox *m_verboseCheck;
    QCheckBox *m_includeWifiCheck;
    QCheckBox *m_includeBluetoothCheck;
    QCheckBox *m_includeGpuDriversCheck;
    
    // Output
    QLineEdit *m_outputDirEdit;
    QPushButton *m_browseDirButton;
    
    // Build control
    QPushButton *m_buildButton;
    QPushButton *m_stopButton;
    QProgressBar *m_buildProgress;
    QLabel *m_statusLabel;
    
    // Build steps display
    QGroupBox *m_buildStepsGroup;
    QLabel *m_stepKernelLabel;
    QLabel *m_stepUBootLabel;
    QLabel *m_stepRootfsLabel;
    QLabel *m_stepImageLabel;
    
    // Process handling
    QProcess *m_buildProcess;
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentDownload;
    QString m_githubToken;
    ImageBuilder *m_imageBuilder;
    
    // Build state
    QString m_sourceDir;
    QString m_kernelDir;
    QString m_buildDir;
    QString m_outputDir;
    bool m_isBuilding;
    
    enum BuildStep {
        StepNone,
        StepDownloading,
        StepDownloadingKernel,
        StepKernel,
        StepUBoot,
        StepRootfs,
        StepImage
    };
    BuildStep m_currentStep;
};

#endif // CUSTOMIMAGEWIZARD_H