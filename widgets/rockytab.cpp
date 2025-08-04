#include "rockytab.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextCursor>
#include <QTimer>
#include <QDateTime>
#include <QFrame>
#include <QStandardPaths>
#include <QDir>
#include <QFont>
#include <QDebug>

// Include llama.cpp headers
extern "C" {
#include "../include/3rdparty/llamacpp/include/llama.h"
}

RockyTab::RockyTab(QWidget *parent)
    : QWidget(parent)
    , m_worker(nullptr)
    , m_isGenerating(false)
    , m_modelLoaded(false)
    , m_currentAIMessageLabel(nullptr)
{
    setupUI();
    
    // Initialize llama.cpp
    llama_backend_init();
}

RockyTab::~RockyTab()
{
    if (m_worker) {
        m_worker->stopGeneration();
        m_worker->wait(3000); // Wait up to 3 seconds
        delete m_worker;
    }
    
    // Cleanup llama.cpp
    llama_backend_free();
}

void RockyTab::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    setupChatArea();
    setupSettingsArea();
    
    // Add splitter to main layout
    m_mainLayout->addWidget(m_mainSplitter);
    
    // Set splitter proportions (chat area takes more space)
    m_mainSplitter->setSizes({700, 300});
    
    // Add debug area below the main splitter
    setupDebugArea();
    
    // Status bar at bottom
    QHBoxLayout *statusLayout = new QHBoxLayout();
    m_statusLabel = new QLabel("Ready - Load a model to begin");
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_progressBar);
    
    m_mainLayout->addLayout(statusLayout);
}

void RockyTab::setupChatArea()
{
    m_chatWidget = new QWidget();
    m_chatLayout = new QVBoxLayout(m_chatWidget);
    
    // Chat display area
    m_chatScrollArea = new QScrollArea();
    m_chatContent = new QWidget();
    m_chatContentLayout = new QVBoxLayout(m_chatContent);
    m_chatContentLayout->setAlignment(Qt::AlignTop);
    
    m_chatScrollArea->setWidget(m_chatContent);
    m_chatScrollArea->setWidgetResizable(true);
    m_chatScrollArea->setStyleSheet(
        "QScrollArea { background-color: #f5f5f5; border: 1px solid #ddd; }"
        "QWidget { background-color: #f5f5f5; }"
    );
    
    m_chatLayout->addWidget(m_chatScrollArea);
    
    // Input area
    QWidget *inputWidget = new QWidget();
    m_inputLayout = new QHBoxLayout(inputWidget);
    
    m_inputEdit = new QTextEdit();
    m_inputEdit->setMaximumHeight(100);
    m_inputEdit->setPlaceholderText("Type your message here...");
    
    setupControlButtons();
    
    m_inputLayout->addWidget(m_inputEdit, 1);
    
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(m_sendButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();
    
    m_inputLayout->addLayout(buttonLayout);
    
    m_chatLayout->addWidget(inputWidget);
    
    m_mainSplitter->addWidget(m_chatWidget);
}

void RockyTab::setupSettingsArea()
{
    m_settingsWidget = new QWidget();
    m_settingsLayout = new QVBoxLayout(m_settingsWidget);
    
    // Model Settings Group
    m_modelGroup = new QGroupBox("Model Settings");
    QVBoxLayout *modelLayout = new QVBoxLayout(m_modelGroup);
    
    QHBoxLayout *modelPathLayout = new QHBoxLayout();
    m_modelPathEdit = new QLineEdit();
    m_modelPathEdit->setPlaceholderText("Path to .gguf model file...");
    m_loadModelButton = new QPushButton("Browse & Load");
    
    modelPathLayout->addWidget(m_modelPathEdit);
    modelPathLayout->addWidget(m_loadModelButton);
    
    m_modelStatusLabel = new QLabel("No model loaded");
    m_modelStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    
    modelLayout->addLayout(modelPathLayout);
    modelLayout->addWidget(m_modelStatusLabel);
    
    // Generation Settings Group
    m_genGroup = new QGroupBox("Generation Settings");
    QVBoxLayout *genLayout = new QVBoxLayout(m_genGroup);
    
    // Max tokens
    QHBoxLayout *maxTokensLayout = new QHBoxLayout();
    maxTokensLayout->addWidget(new QLabel("Max Tokens:"));
    m_maxTokensSpin = new QSpinBox();
    m_maxTokensSpin->setRange(1, 4096);
    m_maxTokensSpin->setValue(512);
    maxTokensLayout->addWidget(m_maxTokensSpin);
    maxTokensLayout->addStretch();
    genLayout->addLayout(maxTokensLayout);
    
    // Temperature
    QHBoxLayout *tempLayout = new QHBoxLayout();
    tempLayout->addWidget(new QLabel("Temperature:"));
    m_temperatureSlider = new QSlider(Qt::Horizontal);
    m_temperatureSlider->setRange(1, 200);
    m_temperatureSlider->setValue(80);
    m_temperatureLabel = new QLabel("0.8");
    tempLayout->addWidget(m_temperatureSlider);
    tempLayout->addWidget(m_temperatureLabel);
    genLayout->addLayout(tempLayout);
    
    // Top-P
    QHBoxLayout *topPLayout = new QHBoxLayout();
    topPLayout->addWidget(new QLabel("Top-P:"));
    m_topPSlider = new QSlider(Qt::Horizontal);
    m_topPSlider->setRange(1, 100);
    m_topPSlider->setValue(95);
    m_topPLabel = new QLabel("0.95");
    topPLayout->addWidget(m_topPSlider);
    topPLayout->addWidget(m_topPLabel);
    genLayout->addLayout(topPLayout);
    
    // Context size
    QHBoxLayout *contextLayout = new QHBoxLayout();
    contextLayout->addWidget(new QLabel("Context Size:"));
    m_contextSizeSpin = new QSpinBox();
    m_contextSizeSpin->setRange(512, 8192);
    m_contextSizeSpin->setValue(2048);
    contextLayout->addWidget(m_contextSizeSpin);
    contextLayout->addStretch();
    genLayout->addLayout(contextLayout);
    
    // Threads
    QHBoxLayout *threadsLayout = new QHBoxLayout();
    threadsLayout->addWidget(new QLabel("Threads:"));
    m_threadsSpin = new QSpinBox();
    m_threadsSpin->setRange(1, 16);
    m_threadsSpin->setValue(4);
    threadsLayout->addWidget(m_threadsSpin);
    threadsLayout->addStretch();
    genLayout->addLayout(threadsLayout);
    
    // Advanced Settings Group
    m_advancedGroup = new QGroupBox("Advanced Settings");
    QVBoxLayout *advLayout = new QVBoxLayout(m_advancedGroup);
    
    m_streamingCheck = new QCheckBox("Streaming Output");
    m_streamingCheck->setChecked(true);
    m_debugCheck = new QCheckBox("Debug Mode");
    
    advLayout->addWidget(m_streamingCheck);
    advLayout->addWidget(m_debugCheck);
    
    // Add all groups to settings layout
    m_settingsLayout->addWidget(m_modelGroup);
    m_settingsLayout->addWidget(m_genGroup);
    m_settingsLayout->addWidget(m_advancedGroup);
    m_settingsLayout->addStretch();
    
    m_mainSplitter->addWidget(m_settingsWidget);
    
    // Connect signals
    connect(m_loadModelButton, &QPushButton::clicked, this, &RockyTab::onLoadModel);
    connect(m_temperatureSlider, &QSlider::valueChanged, [this](int value) {
        m_temperatureLabel->setText(QString::number(value / 100.0, 'f', 2));
    });
    connect(m_topPSlider, &QSlider::valueChanged, [this](int value) {
        m_topPLabel->setText(QString::number(value / 100.0, 'f', 2));
    });
}

void RockyTab::setupControlButtons()
{
    m_sendButton = new QPushButton("Send");
    m_sendButton->setDefault(true);
    m_sendButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #cccccc; }"
    );
    
    m_stopButton = new QPushButton("Stop");
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #da190b; }"
        "QPushButton:disabled { background-color: #cccccc; }"
    );
    
    m_clearButton = new QPushButton("Clear");
    m_clearButton->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #1976D2; }"
    );
    
    // Connect signals
    connect(m_sendButton, &QPushButton::clicked, this, &RockyTab::onSendMessage);
    connect(m_stopButton, &QPushButton::clicked, this, &RockyTab::onStopGeneration);
    connect(m_clearButton, &QPushButton::clicked, this, &RockyTab::onClearChat);
    
    // Allow Enter to send message
    connect(m_inputEdit, &QTextEdit::textChanged, [this]() {
        // Enable send button only if there's text and model is loaded
        m_sendButton->setEnabled(!m_inputEdit->toPlainText().trimmed().isEmpty() && 
                                m_modelLoaded && !m_isGenerating);
    });
}

void RockyTab::setupDebugArea()
{
    m_debugWidget = new QWidget();
    m_debugLayout = new QVBoxLayout(m_debugWidget);
    
    // Debug area header
    QHBoxLayout *debugHeaderLayout = new QHBoxLayout();
    QLabel *debugLabel = new QLabel("Debug Output");
    debugLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    
    m_clearDebugButton = new QPushButton("Clear Debug");
    m_clearDebugButton->setMaximumWidth(100);
    m_clearDebugButton->setStyleSheet(
        "QPushButton { background-color: #607D8B; color: white; padding: 4px 8px; }"
        "QPushButton:hover { background-color: #546E7A; }"
    );
    
    debugHeaderLayout->addWidget(debugLabel);
    debugHeaderLayout->addStretch();
    debugHeaderLayout->addWidget(m_clearDebugButton);
    
    // Debug output text area
    m_debugOutput = new QTextEdit();
    m_debugOutput->setMaximumHeight(150);
    m_debugOutput->setReadOnly(true);
    m_debugOutput->setFont(QFont("Courier", 9));
    m_debugOutput->setStyleSheet(
        "QTextEdit { "
        "background-color: #1e1e1e; "
        "color: #d4d4d4; "
        "border: 1px solid #3e3e3e; "
        "}"
    );
    
    m_debugLayout->addLayout(debugHeaderLayout);
    m_debugLayout->addWidget(m_debugOutput);
    
    m_mainLayout->addWidget(m_debugWidget);
    
    // Connect clear button
    connect(m_clearDebugButton, &QPushButton::clicked, this, &RockyTab::onClearDebug);
    
    // Add initial debug message
    addDebugMessage("🚀 Rocky Debug Console initialized");
}

QString RockyTab::formatPromptForGemma(const QString& userMessage)
{
    // Gemma 3 instruction format
    return QString("<start_of_turn>user\n%1<end_of_turn>\n<start_of_turn>model\n").arg(userMessage);
}

void RockyTab::onSendMessage()
{
    QString message = m_inputEdit->toPlainText().trimmed();
    if (message.isEmpty() || !m_modelLoaded || m_isGenerating) {
        return;
    }
    
    addDebugMessage(QString("📤 User input: \"%1\"").arg(message));
    
    // Add user message to chat
    addMessageToChat(message, true);
    
    // Clear input
    m_inputEdit->clear();
    
    addDebugMessage("🔄 Starting text generation...");
    
    // Start generation
    updateGenerationState(true);
    
    // Create worker if needed
    if (!m_worker) {
        m_worker = new LlamaWorker(this);
        connect(m_worker, &LlamaWorker::newToken, this, &RockyTab::onLlamaOutput);
        connect(m_worker, &LlamaWorker::finished, this, &RockyTab::onLlamaFinished);
        connect(m_worker, &LlamaWorker::error, this, &RockyTab::onLlamaError);
        connect(m_worker, &LlamaWorker::modelLoaded, this, &RockyTab::onModelLoaded);
        
        addDebugMessage("🔧 LlamaWorker created and connected");
    }
    
    // Start generation with proper chat formatting
    QString formatted_prompt = formatPromptForGemma(message);
    addDebugMessage(QString("📝 Formatted prompt: \"%1\"").arg(formatted_prompt));
    
    addDebugMessage(QString("⚙️ Generation params - Max tokens: %1, Temp: %2, Top-P: %3")
                   .arg(m_maxTokensSpin->value())
                   .arg(m_temperatureSlider->value() / 100.0f, 0, 'f', 2)
                   .arg(m_topPSlider->value() / 100.0f, 0, 'f', 2));
    
    m_worker->generateText(
        formatted_prompt,
        m_maxTokensSpin->value(),
        m_temperatureSlider->value() / 100.0f,
        m_topPSlider->value() / 100.0f,
        m_streamingCheck->isChecked()
    );
}

void RockyTab::onClearChat()
{
    // Clear all chat messages
    QLayoutItem *item;
    while ((item = m_chatContentLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void RockyTab::onLoadModel()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select GGUF Model File",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "GGUF Model Files (*.gguf);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    m_modelPathEdit->setText(fileName);
    m_currentModelPath = fileName;
    
    addDebugMessage(QString("📂 Loading model: %1").arg(QFileInfo(fileName).fileName()));
    addDebugMessage(QString("🔧 Context size: %1, Threads: %2")
                   .arg(m_contextSizeSpin->value())
                   .arg(m_threadsSpin->value()));
    
    // Update status
    m_modelStatusLabel->setText("Loading model...");
    m_modelStatusLabel->setStyleSheet("color: #ff9800; font-style: italic;");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Indeterminate progress
    
    // Create worker if needed
    if (!m_worker) {
        m_worker = new LlamaWorker(this);
        connect(m_worker, &LlamaWorker::newToken, this, &RockyTab::onLlamaOutput);
        connect(m_worker, &LlamaWorker::finished, this, &RockyTab::onLlamaFinished);
        connect(m_worker, &LlamaWorker::error, this, &RockyTab::onLlamaError);
        connect(m_worker, &LlamaWorker::modelLoaded, this, &RockyTab::onModelLoaded);
        
        addDebugMessage("🔧 LlamaWorker created for model loading");
    }
    
    // Load model
    m_worker->loadModel(fileName, m_contextSizeSpin->value(), m_threadsSpin->value());
}

void RockyTab::onStopGeneration()
{
    if (m_worker && m_isGenerating) {
        m_worker->stopGeneration();
    }
}

void RockyTab::onLlamaOutput(const QString& token)
{
    // Add debug output for each token (but limit verbosity)
    static int tokenCount = 0;
    tokenCount++;
    
    if (tokenCount == 1) {
        addDebugMessage("🎯 First token received - generation starting");
        // Create new AI message for streaming
        m_currentAIMessageLabel = nullptr; // Reset to create new message
    } else if (tokenCount % 10 == 0) {
        addDebugMessage(QString("📝 Generated %1 tokens...").arg(tokenCount));
    }
    
    // If we have an active AI message label, append to it
    if (m_currentAIMessageLabel) {
        QString currentText = m_currentAIMessageLabel->text();
        m_currentAIMessageLabel->setText(currentText + token);
        scrollToBottom();
    } else {
        // Create new AI message and remember the label
        addMessageToChat(token, false);
        
        // Find the label we just created
        int count = m_chatContentLayout->count();
        if (count > 0) {
            QLayoutItem *lastItem = m_chatContentLayout->itemAt(count - 1);
            if (lastItem && lastItem->widget()) {
                QFrame *messageFrame = qobject_cast<QFrame*>(lastItem->widget());
                if (messageFrame) {
                    QHBoxLayout *frameLayout = qobject_cast<QHBoxLayout*>(messageFrame->layout());
                    if (frameLayout) {
                        // Find the message label inside the frame
                        for (int i = 0; i < frameLayout->count(); ++i) {
                            QLayoutItem *item = frameLayout->itemAt(i);
                            if (item && item->widget()) {
                                QLabel *label = qobject_cast<QLabel*>(item->widget());
                                if (label && label->wordWrap()) { // Message labels have wordWrap enabled
                                    m_currentAIMessageLabel = label;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void RockyTab::onLlamaFinished()
{
    updateGenerationState(false);
    m_statusLabel->setText("Generation completed");
    addDebugMessage("✅ Generation completed successfully");
    
    // Reset tracking
    m_currentAIMessageLabel = nullptr;
    
    // Reset token counter
    static int tokenCount = 0;
    tokenCount = 0;
}

void RockyTab::onLlamaError(const QString& error)
{
    updateGenerationState(false);
    m_statusLabel->setText("Error: " + error);
    addDebugMessage(QString("❌ Generation error: %1").arg(error));
    
    // Reset tracking
    m_currentAIMessageLabel = nullptr;
    
    QMessageBox::warning(this, "Llama Error", error);
}

void RockyTab::onModelLoaded(bool success, const QString& message)
{
    m_progressBar->setVisible(false);
    
    if (success) {
        m_modelLoaded = true;
        m_modelStatusLabel->setText("Model loaded: " + QFileInfo(m_currentModelPath).fileName());
        m_modelStatusLabel->setStyleSheet("color: #4CAF50; font-style: italic;");
        m_statusLabel->setText("Model loaded successfully - Ready to chat!");
        
        addDebugMessage("✅ Model loading successful");
        addDebugMessage(QString("📊 Model: %1").arg(QFileInfo(m_currentModelPath).fileName()));
        addDebugMessage(QString("💬 Status: %1").arg(message));
        
        // Add welcome message
        addMessageToChat("Model loaded successfully! You can now start chatting.", false);
    } else {
        m_modelLoaded = false;
        m_modelStatusLabel->setText("Failed to load model");
        m_modelStatusLabel->setStyleSheet("color: #f44336; font-style: italic;");
        m_statusLabel->setText("Failed to load model: " + message);
        
        addDebugMessage("❌ Model loading failed");
        addDebugMessage(QString("🔥 Error: %1").arg(message));
    }
    
    // Update button states
    m_sendButton->setEnabled(m_modelLoaded && !m_inputEdit->toPlainText().trimmed().isEmpty());
}

void RockyTab::addMessageToChat(const QString& message, bool isUser)
{
    QFrame *messageFrame = new QFrame();
    messageFrame->setFrameStyle(QFrame::Box);
    messageFrame->setLineWidth(1);
    
    QHBoxLayout *frameLayout = new QHBoxLayout(messageFrame);
    
    QLabel *avatarLabel = new QLabel();
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet(
        QString("background-color: %1; border-radius: 20px; font-weight: bold; color: white;")
        .arg(isUser ? "#2196F3" : "#4CAF50")
    );
    avatarLabel->setText(isUser ? "U" : "AI");
    
    QLabel *messageLabel = new QLabel(message);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageLabel->setStyleSheet("padding: 8px; background-color: white; border-radius: 8px;");
    
    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm"));
    timeLabel->setStyleSheet("color: #666; font-size: 10px;");
    timeLabel->setAlignment(Qt::AlignTop);
    
    if (isUser) {
        frameLayout->addStretch();
        frameLayout->addWidget(timeLabel);
        frameLayout->addWidget(messageLabel);
        frameLayout->addWidget(avatarLabel);
        messageFrame->setStyleSheet("background-color: #e3f2fd;");
    } else {
        frameLayout->addWidget(avatarLabel);
        frameLayout->addWidget(messageLabel);
        frameLayout->addWidget(timeLabel);
        frameLayout->addStretch();
        messageFrame->setStyleSheet("background-color: #f1f8e9;");
    }
    
    m_chatContentLayout->addWidget(messageFrame);
    scrollToBottom();
}

void RockyTab::scrollToBottom()
{
    QTimer::singleShot(0, [this]() {
        QScrollBar *scrollBar = m_chatScrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });
}

void RockyTab::updateGenerationState(bool isGenerating)
{
    m_isGenerating = isGenerating;
    m_sendButton->setEnabled(!isGenerating && m_modelLoaded && !m_inputEdit->toPlainText().trimmed().isEmpty());
    m_stopButton->setEnabled(isGenerating);
    
    if (isGenerating) {
        m_statusLabel->setText("Generating response...");
        // Add empty AI message to be filled with tokens
        addMessageToChat("", false);
    }
}

// LlamaWorker Implementation
LlamaWorker::LlamaWorker(QObject *parent)
    : QThread(parent)
    , m_model(nullptr)
    , m_context(nullptr)
    , m_shouldStop(false)
{
}

LlamaWorker::~LlamaWorker()
{
    cleanup();
}

void LlamaWorker::loadModel(const QString& modelPath, int contextSize, int threads)
{
    m_operation = LoadModel;
    m_loadParams.modelPath = modelPath;
    m_loadParams.contextSize = contextSize;
    m_loadParams.threads = threads;
    
    start();
}

void LlamaWorker::generateText(const QString& prompt, int maxTokens, float temperature, float topP, bool streaming)
{
    m_operation = GenerateText;
    m_genParams.prompt = prompt;
    m_genParams.maxTokens = maxTokens;
    m_genParams.temperature = temperature;
    m_genParams.topP = topP;
    m_genParams.streaming = streaming;
    
    start();
}

void LlamaWorker::stopGeneration()
{
    m_shouldStop = true;
}

void LlamaWorker::run()
{
    switch (m_operation) {
        case LoadModel:
            performLoadModel();
            break;
        case GenerateText:
            performGeneration();
            break;
    }
}

void LlamaWorker::performLoadModel()
{
    cleanup(); // Clean up any existing model
    
    try {
        // Set up model parameters
        llama_model_params model_params = llama_model_default_params();
        
        // Load model using new API
        m_model = llama_model_load_from_file(m_loadParams.modelPath.toStdString().c_str(), model_params);
        if (!m_model) {
            emit modelLoaded(false, "Failed to load model file");
            return;
        }
        
        // Set up context parameters
        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = m_loadParams.contextSize;
        ctx_params.n_threads = m_loadParams.threads;
        
        // Create context using new API
        m_context = llama_init_from_model(static_cast<llama_model*>(m_model), ctx_params);
        if (!m_context) {
            llama_model_free(static_cast<llama_model*>(m_model));
            m_model = nullptr;
            emit modelLoaded(false, "Failed to create context");
            return;
        }
        
        emit modelLoaded(true, "Model loaded successfully");
        
    } catch (const std::exception& e) {
        cleanup();
        emit modelLoaded(false, QString("Exception: %1").arg(e.what()));
    } catch (...) {
        cleanup();
        emit modelLoaded(false, "Unknown error occurred");
    }
}

void LlamaWorker::performGeneration()
{
    if (!m_model || !m_context) {
        emit error("No model loaded");
        return;
    }
    
    m_shouldStop = false;
    
    try {
        // Get model vocab first
        const struct llama_vocab * vocab = llama_model_get_vocab(static_cast<llama_model*>(m_model));
        
        // Tokenize the prompt using the correct API
        std::string prompt = m_genParams.prompt.toStdString();
        qDebug() << "Tokenizing prompt:" << QString::fromStdString(prompt);
        
        // Find the number of tokens in the prompt using the vocab
        const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, true, true);
        
        if (n_prompt <= 0) {
            emit error(QString("Failed to tokenize prompt (got %1 tokens)").arg(n_prompt));
            return;
        }
        
        qDebug() << "Prompt tokenized to" << n_prompt << "tokens";
        
        // Allocate space for the tokens and tokenize the prompt
        std::vector<llama_token> prompt_tokens(n_prompt);
        const int actual_tokens = llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true);
        if (actual_tokens < 0) {
            emit error("Failed to tokenize the prompt");
            return;
        }
        
        qDebug() << "Successfully tokenized" << actual_tokens << "tokens";
        
        // Create batch for evaluation
        llama_batch batch = llama_batch_get_one(prompt_tokens.data(), actual_tokens);
        
        // Evaluate the prompt
        qDebug() << "Evaluating prompt batch with" << actual_tokens << "tokens";
        if (llama_decode(static_cast<llama_context*>(m_context), batch) != 0) {
            emit error("Failed to evaluate prompt");
            return;
        }
        
        qDebug() << "Prompt evaluation successful, starting generation";
        
        // Initialize sampler with proper parameters
        auto sampler_params = llama_sampler_chain_default_params();
        llama_sampler * sampler = llama_sampler_chain_init(sampler_params);
        
        // Add temperature and top-p sampling
        if (m_genParams.temperature > 0.0f) {
            llama_sampler_chain_add(sampler, llama_sampler_init_temp(m_genParams.temperature));
        }
        if (m_genParams.topP < 1.0f) {
            llama_sampler_chain_add(sampler, llama_sampler_init_top_p(m_genParams.topP, 1));
        }
        
        // Add final sampler (dist for probabilistic sampling or greedy for deterministic)
        if (m_genParams.temperature > 0.0f) {
            llama_sampler_chain_add(sampler, llama_sampler_init_dist(42)); // seed=42
        } else {
            llama_sampler_chain_add(sampler, llama_sampler_init_greedy());
        }
        
        // Generate tokens
        int n_generated = 0;
        const int max_tokens = std::min(m_genParams.maxTokens, 512); // Reasonable limit
        
        qDebug() << "Starting token generation, max tokens:" << max_tokens;
        
        while (n_generated < max_tokens && !m_shouldStop) {
            // Sample next token
            llama_token new_token_id = llama_sampler_sample(sampler, static_cast<llama_context*>(m_context), -1);
            
            // Check for end of sequence or special tokens using vocab API
            if (llama_vocab_is_eog(vocab, new_token_id)) {
                qDebug() << "Stopping: EOS token detected";
                break;
            }
            
            // Convert token to text using vocab API
            char buf[256];
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) {
                emit error("Failed to convert token to text");
                break;
            }
            
            QString token_str = QString::fromUtf8(buf, n);
            qDebug() << "Generated token:" << token_str;
            
            // Check for Gemma-specific stop sequences
            if (token_str.contains("<end_of_turn>") || 
                token_str.contains("User:") || 
                token_str.contains("Human:") ||
                token_str.contains("\n\n\n")) {
                qDebug() << "Stopping: Stop sequence detected:" << token_str;
                break;
            }
            
            emit newToken(token_str);
            
            // Accept the token
            llama_sampler_accept(sampler, new_token_id);
            
            // Create batch for the new token
            llama_batch token_batch = llama_batch_get_one(&new_token_id, 1);
            
            // Evaluate the new token
            if (llama_decode(static_cast<llama_context*>(m_context), token_batch) != 0) {
                emit error("Failed to evaluate token");
                break;
            }
            
            n_generated++;
        }
        
        qDebug() << "Generation completed, generated" << n_generated << "tokens";
        
        // Cleanup sampler
        llama_sampler_free(sampler);
        
        emit finished();
        
    } catch (const std::exception& e) {
        emit error(QString("Generation error: %1").arg(e.what()));
    } catch (...) {
        emit error("Unknown generation error");
    }
}

void LlamaWorker::cleanup()
{
    if (m_context) {
        llama_free(static_cast<llama_context*>(m_context));
        m_context = nullptr;
    }
    
    if (m_model) {
        llama_model_free(static_cast<llama_model*>(m_model));
        m_model = nullptr;
    }
}

void RockyTab::addDebugMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString formattedMessage = QString("[%1] %2").arg(timestamp, message);
    
    m_debugOutput->append(formattedMessage);
    
    // Auto-scroll to bottom
    QScrollBar *scrollBar = m_debugOutput->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
    
    // Keep only last 1000 lines to prevent memory issues
    QTextDocument *doc = m_debugOutput->document();
    if (doc->lineCount() > 1000) {
        QTextCursor cursor = m_debugOutput->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, doc->lineCount() - 900);
        cursor.removeSelectedText();
    }
}

void RockyTab::onClearDebug()
{
    m_debugOutput->clear();
    addDebugMessage("🧹 Debug console cleared");
}
