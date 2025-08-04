#ifndef GPUMANAGER_H
#define GPUMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QListWidget>
#include <QCheckBox>
#include <QVector>
#include <QTimer>
#include <QEvent>

class SystemManager;

class GpuManager : public QWidget
{
    Q_OBJECT

public:
    explicit GpuManager(SystemManager *systemManager, QWidget *parent = nullptr);

signals:
    void installDriverRequested(const QString &driverPath);
    void removeDriverRequested(const QString &driverName);
    void switchDriverRequested(const QString &driverType);

private slots:
    void onScanDrivers();
    void onInstallDriver();
    void onRemoveDriver();
    void onSwitchDriver();
    void onDriverSelectionChanged();
    void updateDriverStatus();
    void updateGpuGraph();
    void onOpenDriverLocation();

private:
    void setupUI();
    void createGpuGraphGroup();
    void createDriverInfoGroup();
    void createDriverActionsGroup();
    void createDriverConfigGroup();
    
    // Real system monitoring functions
    double readGpuFrequency();
    double readGpuTemperature();
    double readGpuUsage();
    double readCpuFrequency();
    double readCpuTemperature();
    double readCpuUsage();
    
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    
    // UI Components
    QGroupBox *m_gpuGraphGroup;
    QGroupBox *m_driverInfoGroup;
    QGroupBox *m_driverActionsGroup;
    QGroupBox *m_driverConfigGroup;
    
    // GPU Graph components
    QWidget *m_gpuGraphWidget;
    QLabel *m_powerVoltageLabel;
    QLabel *m_powerWattsLabel;
    QLabel *m_systemResourcesLabel;
    QLabel *m_cpuFreqLabel;
    QLabel *m_cpuTempLabel;
    QLabel *m_cpuUsageLabel;
    QTimer *m_graphUpdateTimer;
    QVector<double> m_voltageData;
    QVector<double> m_powerData;
    QVector<double> m_usageData;
    QVector<double> m_cpuFreqData;
    QVector<double> m_cpuTempData;
    QVector<double> m_cpuUsageData;
    
    // Driver info components
    QLabel *m_currentDriverLabel;
    QLabel *m_gpuInfoLabel;
    QLabel *m_driverVersionLabel;
    QLabel *m_driverLibVersionLabel;
    QLabel *m_driverDateLabel;
    QLabel *m_driverCreatorLabel;
    QLabel *m_driverSupportsLabel;
    QLabel *m_driverLocationLink;
    QLabel *m_mesaPanfrostLink;
    QLabel *m_panforkLink;
    QLabel *m_panthorLink;
    QLabel *m_armValhallLink;
    QListWidget *m_availableDriversList;
    QTextEdit *m_driverDetailsText;
    
    QComboBox *m_driverTypeCombo;
    QPushButton *m_scanButton;
    QPushButton *m_installButton;
    QPushButton *m_removeButton;
    QPushButton *m_switchButton;
    QPushButton *m_testButton;
    
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // Backend
    SystemManager *m_systemManager;
    QStringList m_availableDrivers;
    QString m_currentDriver;
    QString m_driverLocation;
};

#endif // GPUMANAGER_H