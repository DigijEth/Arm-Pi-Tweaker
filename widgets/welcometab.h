#ifndef WELCOMETAB_H
#define WELCOMETAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QScrollArea>
#include <QGroupBox>
#include <QPushButton>
#include <QFrame>

class WelcomeTab : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeTab(QWidget *parent = nullptr);
    ~WelcomeTab();

private slots:
    void onQuickStartClicked();
    void onDocumentationClicked();
    void onCommunityClicked();

private:
    void setupUI();
    void setupWelcomeSection();
    void setupWarningSection();
    void setupRockySection();
    void setupQuickStartSection();
    void setupFooterSection();

    // UI Components
    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    
    // Sections
    QGroupBox *m_welcomeGroup;
    QGroupBox *m_warningGroup;
    QGroupBox *m_rockyGroup;
    QGroupBox *m_quickStartGroup;
    QGroupBox *m_footerGroup;
    
    // Welcome section
    QLabel *m_titleLabel;
    QLabel *m_versionLabel;
    QLabel *m_welcomeText;
    
    // Warning section
    QLabel *m_warningTitle;
    QTextEdit *m_warningText;
    
    // Rocky section
    QLabel *m_rockyTitle;
    QTextEdit *m_rockyDescription;
    
    // Quick start section
    QLabel *m_quickStartTitle;
    QTextEdit *m_quickStartGuide;
    QPushButton *m_quickStartButton;
    QPushButton *m_docButton;
    QPushButton *m_communityButton;
    
    // Footer
    QLabel *m_footerText;
    QLabel *m_licenseText;
};

#endif // WELCOMETAB_H
