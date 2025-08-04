#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ImageBuilder : public QObject
{
    Q_OBJECT

public:
    enum Suite {
        Jammy,   // 22.04 LTS
        Noble,   // 24.04 LTS  
        Oracular, // 24.10
        Plucky   // 25.04
    };

    enum Flavor {
        Desktop,
        Server
    };

    enum PartitionType {
        EXT4,
        F2FS
    };

    enum BuildMode {
        FullBuild,      // Complete image build
        KernelOnly,     // Build kernel packages only
        UBootOnly,      // Build U-Boot packages only
        RootfsOnly,     // Build rootfs only
        ImageOnly       // Create image from existing components
    };

    enum KernelSource {
        RemoteKernel,   // Clone from Joshua Riek's linux-rockchip
        LocalKernel,    // Use local kernel source directory
        CustomGitKernel // Clone from custom git URL
    };

    struct BuildConfiguration {
        QString board = "orangepi-5-plus";
        Suite suite = Noble;
        Flavor flavor = Desktop;
        BuildMode buildMode = FullBuild;
        KernelSource kernelSource = RemoteKernel;
        PartitionType partitionType = EXT4;
        
        // Directories
        QString baseDir;
        QString sourcesDir;
        QString buildDir;
        QString outputDir;
        
        // Kernel source options
        QString localKernelPath;        // Path to local kernel source
        QString customKernelGitUrl;     // Custom git URL for kernel
        QString customKernelBranch;     // Custom git branch
        
        // Build options
        bool cleanBuild = false;
        bool verboseOutput = false;
        bool includeWifi = true;
        bool includeBluetooth = true;
        bool includeGpuDrivers = true;
        bool includeCameraEngine = true;
        bool includeWiringPi = true;
    };

    explicit ImageBuilder(QObject *parent = nullptr);
    ~ImageBuilder();

    // Configuration methods
    void setConfiguration(const BuildConfiguration &config);
    BuildConfiguration getConfiguration() const { return m_config; }
    // Set a custom download location for source repositories
    void setDownloadLocation(const QString &path);
    
    // Suite/Flavor utilities
    static QString suiteToString(Suite suite);
    static QString flavorToString(Flavor flavor);
    static QString partitionTypeToString(PartitionType partitionType);
    static Suite stringToSuite(const QString &suiteStr);
    static Flavor stringToFlavor(const QString &flavorStr);
    static PartitionType stringToPartitionType(const QString &partitionStr);
    
    // Branch mapping utilities  
    QString getLivecdRootfsBranch(Suite suite) const;
    QString getKernelBranch(Suite suite) const;
    QStringList getPPAsForSuite(Suite suite) const;
    
    // Build methods
    void startBuild();
    void startKernelOnlyBuild();
    void cancelBuild();
    
    // Status methods
    bool isBuilding() const { return m_isBuilding; }
    QString getCurrentStep() const { return m_currentStepDescription; }
    int getProgress() const { return m_progress; }

signals:
    void buildStarted();
    void buildProgress(int percentage, const QString &description);
    void buildStepChanged(const QString &step, const QString &description);
    void buildLogMessage(const QString &message);
    void buildCompleted(bool success, const QString &message);
    void buildError(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessOutput();

private:
    // Core build methods
    void initializeBuild();
    void buildKernel();
    void buildUBoot();
    void buildRootfs();
    void configureImage();
    void createDiskImage();
    
    // Repository management
    void cloneRepository(const QString &url, const QString &branch, const QString &targetDir);
    void setupKernelSource();
    bool validateKernelSource();
    
    // Environment setup
    void setupBuildEnvironment();
    void createDirectories();
    QJsonObject generateEnvironmentVariables() const;
    
    // Suite-specific configuration
    void configurePPAs(const QString &chrootDir);
    void configureSnapPackages(const QString &chrootDir);
    void configurePackageList(const QString &chrootDir);
    void applyBoardSpecificHook(const QString &chrootDir);
    
    // Partitioning and image creation
    void createDesktopPartitions(const QString &imagePath, const QString &loopDevice);
    void createServerPartitions(const QString &imagePath, const QString &loopDevice);
    void createF2FSPartitions(const QString &imagePath, const QString &loopDevice);
    void installBootloader(const QString &imagePath, const QString &loopDevice);
    QString getFilesystemTypeString() const;
    QString getPartitionLabel() const;
    
    // Process management
    void executeCommand(const QString &command, const QString &workingDir = QString());
    void setCurrentStep(const QString &step, const QString &description);
    void updateProgress(int percentage);
    void logMessage(const QString &message);
    void logError(const QString &error);
    
    // Utility methods
    QString getVersionString() const;
    QString getImageFileName() const;
    QString getRootfsFileName() const;
    bool hasRequiredTools();
    QString generateUUID() const;
    
private:
    BuildConfiguration m_config;
    QProcess *m_process;
    QTimer *m_progressTimer;
    
    // Build state
    bool m_isBuilding;
    QString m_currentStepDescription;
    int m_progress;
    int m_totalSteps;
    int m_currentStepIndex;
    
    // Build artifacts tracking
    QString m_kernelImageDeb;
    QString m_kernelHeadersDeb;
    QString m_kernelModulesDeb;
    QString m_kernelBuildinfoTarget;
    QString m_kernelRockchipHeadersDeb;
    QString m_ubootDeb;
    QString m_rootfsTar;
    
    // Directories
    QString m_livecdRootfsDir;
    QString m_linuxRockchipDir;
    QString m_ubootSourceDir;
    QString m_buildRootfsDir;
    QString m_chrootDir;
    // Custom download location
    QString m_downloadDir;
    
    // Build steps tracking
    enum BuildStep {
        StepInitialization,
        StepKernelBuild,
        StepUBootBuild,
        StepRootfsBuild,
        StepImageConfig,
        StepImageCreation,
        StepFinalization
    };
    
    BuildStep m_currentBuildStep;
    QStringList m_buildSteps;
    
    // Constants
    static const QString BOARD_NAME;
    static const QString BOARD_MAKER;
    static const QString BOARD_SOC;
    static const QString BOARD_CPU;
    static const QString UBOOT_PACKAGE;
    static const QString UBOOT_RULES_TARGET;
    static const QString KERNEL_FLAVOR;
    
    // Repository URLs
    static const QString LIVECD_ROOTFS_URL;
    static const QString LINUX_ROCKCHIP_URL;
    static const QString UBOOT_RADXA_URL;
    static const QString UBOOT_BRANCH;
    static const QString UBOOT_COMMIT;
};

Q_DECLARE_METATYPE(ImageBuilder::Suite)
Q_DECLARE_METATYPE(ImageBuilder::Flavor)
Q_DECLARE_METATYPE(ImageBuilder::BuildMode)
Q_DECLARE_METATYPE(ImageBuilder::KernelSource)
Q_DECLARE_METATYPE(ImageBuilder::PartitionType)

#endif // IMAGEBUILDER_H