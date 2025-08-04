#ifndef UPGRADEWIDGET_H
#define UPGRADEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

class UpgradeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpgradeWidget(QWidget *parent = nullptr);

signals:
    void extractDriversRequested();
    void runUpgradeRequested();
    void patchSystemRequested();
    void rollbackRequested();

public slots:
    void updateProgress(int value);
    void updateStatus(const QString &message);
    void setButtonsEnabled(bool enabled);

private:
    void setupUI();
    QGroupBox* createStepGroup(const QString &title, const QString &description, 
                              QPushButton *button, const QString &helpText);

    // UI Components
    QGroupBox *m_extractGroup;
    QGroupBox *m_upgradeGroup;
    QGroupBox *m_patchGroup;
    QGroupBox *m_rollbackGroup;
    QGroupBox *m_warningGroup;
    
    QPushButton *m_extractButton;
    QPushButton *m_upgradeButton;
    QPushButton *m_patchButton;
    QPushButton *m_rollbackButton;
    
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QTextEdit *m_logOutput;
};

#endif // UPGRADEWIDGET_H