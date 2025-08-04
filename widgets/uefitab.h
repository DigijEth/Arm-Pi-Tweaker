#ifndef UEFITAB_H
#define UEFITAB_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTextEdit;
class QProgressBar;
class QTableWidget;
class QGroupBox;
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

class UefiTab : public QWidget
{
    Q_OBJECT

public:
    explicit UefiTab(QWidget *parent = nullptr);
    ~UefiTab();

private slots:
    void checkCurrentUefi();
    void checkForUpdates();
    void loadAvailablePatches();
    void applySelectedPatches();
    void flashUefi();
    void backupCurrentUefi();
    void restoreUefiBackup();
    void onFlashProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFlashProcessOutput();
    void selectUefiFile();
    void verifyUefiImage();

private:
    // UI Elements
    QLabel *m_currentVersionLabel;
    QLabel *m_boardLabel;
    QLabel *m_spiFlashLabel;
    QLabel *m_statusLabel;
    
    QPushButton *m_checkUpdatesBtn;
    QPushButton *m_flashBtn;
    QPushButton *m_backupBtn;
    QPushButton *m_restoreBtn;
    QPushButton *m_selectFileBtn;
    QPushButton *m_verifyBtn;
    
    QTextEdit *m_logOutput;
    QProgressBar *m_progressBar;
    
    QTableWidget *m_featuresTable;
    QTableWidget *m_patchesTable;
    QTableWidget *m_updatesTable;
    
    QGroupBox *m_infoGroup;
    QGroupBox *m_flashGroup;
    QGroupBox *m_patchGroup;
    
    QComboBox *m_flashMethodCombo;
    QCheckBox *m_verifyAfterFlash;
    QCheckBox *m_autoBackup;
    
    // Process handling
    QProcess *m_flashProcess;
    QString m_selectedUefiPath;
    QString m_spiDevice;
    
    // UEFI Information
    struct UefiInfo {
        QString version;
        QString buildDate;
        QString board;
        QStringList features;
        quint32 size;
        quint32 fitOffset;
    };
    
    struct UefiPatch {
        QString id;
        QString name;
        QString description;
        QString version;
        QString type; // "feature", "bugfix", "enhancement"
        QStringList dependencies;
        quint32 offset;
        QByteArray data;
    };
    
    struct UefiUpdate {
        QString version;
        QString releaseDate;
        QString downloadUrl;
        QString changelog;
        QString checksum;
        quint32 size;
    };
    
    UefiInfo m_currentUefi;
    QList<UefiPatch> m_availablePatches;
    QList<UefiUpdate> m_availableUpdates;
    
    // Helper methods
    void setupUi();
    void detectSpiDevice();
    bool readCurrentUefi(UefiInfo &info);
    bool parseUefiImage(const QString &path, UefiInfo &info);
    bool downloadUpdate(const UefiUpdate &update);
    bool applyPatch(const UefiPatch &patch, QByteArray &uefiData);
    bool writeToSpiFlash(const QByteArray &data, quint32 offset = 0);
    QString calculateChecksum(const QByteArray &data);
    void updateFeaturesList();
    void loadPatchDatabase();
    
    // Safety checks
    bool isOrangePi5Plus();
    bool verifyImageCompatibility(const QString &imagePath);
    bool checkBatteryStatus();
    bool lockSpiDevice();
    void unlockSpiDevice();
    
    // Safe flash implementation
    void performSafeFlash();
    bool flashBlock(int fd, quint32 offset, const QByteArray &data);
    bool verifyBlock(int fd, quint32 offset, const QByteArray &expected);
    bool eraseBlock(int fd, quint32 offset);
};

#endif // UEFITAB_H