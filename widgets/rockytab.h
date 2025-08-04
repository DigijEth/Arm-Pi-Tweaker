#ifndef ROCKYTAB_H
#define ROCKYTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QThread>
#include <QScrollArea>
#include <QSplitter>

class LlamaWorker;

class RockyTab : public QWidget
{
    Q_OBJECT

public:
    explicit RockyTab(QWidget *parent = nullptr);
    ~RockyTab();

private slots:
    void onSendMessage();
    void onClearChat();
    void onLoadModel();
    void onStopGeneration();
    void onLlamaOutput(const QString& token);
    void onLlamaFinished();
    void onLlamaError(const QString& error);
    void onModelLoaded(bool success, const QString& message);
    void onClearDebug();

private:
    void setupUI();
    void setupChatArea();
    void setupSettingsArea();
    void setupDebugArea();
    void setupControlButtons();
    void addMessageToChat(const QString& message, bool isUser = true);
    void scrollToBottom();
    void updateGenerationState(bool isGenerating);
    void addDebugMessage(const QString& message);
    QString formatPromptForGemma(const QString& userMessage);

    // UI Components
    QVBoxLayout *m_mainLayout;
    QSplitter *m_mainSplitter;
    
    // Chat Area
    QWidget *m_chatWidget;
    QVBoxLayout *m_chatLayout;
    QScrollArea *m_chatScrollArea;
    QWidget *m_chatContent;
    QVBoxLayout *m_chatContentLayout;
    QTextEdit *m_inputEdit;
    QHBoxLayout *m_inputLayout;
    QPushButton *m_sendButton;
    QPushButton *m_clearButton;
    QPushButton *m_stopButton;
    
    // Settings Area
    QWidget *m_settingsWidget;
    QVBoxLayout *m_settingsLayout;
    
    // Model Settings
    QGroupBox *m_modelGroup;
    QLineEdit *m_modelPathEdit;
    QPushButton *m_loadModelButton;
    QLabel *m_modelStatusLabel;
    
    // Generation Settings
    QGroupBox *m_genGroup;
    QSpinBox *m_maxTokensSpin;
    QSlider *m_temperatureSlider;
    QLabel *m_temperatureLabel;
    QSlider *m_topPSlider;
    QLabel *m_topPLabel;
    QSpinBox *m_contextSizeSpin;
    QSpinBox *m_threadsSpin;
    
    // Advanced Settings
    QGroupBox *m_advancedGroup;
    QCheckBox *m_streamingCheck;
    QCheckBox *m_debugCheck;
    
    // Debug Output
    QTextEdit *m_debugOutput;
    QWidget *m_debugWidget;
    QVBoxLayout *m_debugLayout;
    QPushButton *m_clearDebugButton;
    
    // Status
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // Backend
    LlamaWorker *m_worker;
    bool m_isGenerating;
    bool m_modelLoaded;
    QString m_currentModelPath;
    
    // Track current AI message for streaming
    QLabel *m_currentAIMessageLabel;
};

// Worker thread for Llama operations
class LlamaWorker : public QThread
{
    Q_OBJECT

public:
    explicit LlamaWorker(QObject *parent = nullptr);
    ~LlamaWorker();
    
    void loadModel(const QString& modelPath, int contextSize, int threads);
    void generateText(const QString& prompt, int maxTokens, float temperature, float topP, bool streaming);
    void stopGeneration();

signals:
    void newToken(const QString& token);
    void finished();
    void error(const QString& error);
    void modelLoaded(bool success, const QString& message);

protected:
    void run() override;

private:
    enum Operation {
        LoadModel,
        GenerateText
    };
    
    struct LoadModelParams {
        QString modelPath;
        int contextSize;
        int threads;
    };
    
    struct GenerateParams {
        QString prompt;
        int maxTokens;
        float temperature;
        float topP;
        bool streaming;
    };
    
    Operation m_operation;
    LoadModelParams m_loadParams;
    GenerateParams m_genParams;
    
    // Llama.cpp objects
    void* m_model;
    void* m_context;
    bool m_shouldStop;
    
    void performLoadModel();
    void performGeneration();
    void cleanup();
};

#endif // ROCKYTAB_H
