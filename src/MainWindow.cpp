#include "MainWindow.h"
#include "ChatWidget.h"
#include "Worker.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QMetaObject>
#include <QThread>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settings(AppSettings::load())
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("AI Doctor"));
    resize(900, 700);

    m_chatWidget = new ChatWidget(this);
    ui->chatLayout->addWidget(m_chatWidget);

    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(ui->inputEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onSettingsTriggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    m_chatWidget->addSystemMessage(
        QStringLiteral("Welcome to AI Doctor. Ask a general health question. "
                       "This tool provides educational information only."));

    setupWorkerThread();
    setInputEnabled(false);
    statusBar()->showMessage(QStringLiteral("Loading model..."));
    loadModel();
}

MainWindow::~MainWindow()
{
    if (m_workerThread) {
        QMetaObject::invokeMethod(m_worker, "unloadModel", Qt::QueuedConnection);
        m_workerThread->quit();
        m_workerThread->wait();
    }
    delete ui;
}

void MainWindow::setupWorkerThread()
{
    m_workerThread = new QThread(this);
    m_worker = new Worker();
    m_worker->moveToThread(m_workerThread);

    connect(m_worker, &Worker::modelLoaded, this, &MainWindow::onModelLoaded);
    connect(m_worker, &Worker::responseReady, this, &MainWindow::onResponseReady);
    connect(m_worker, &Worker::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    m_workerThread->start();
}

void MainWindow::loadModel()
{
    QMetaObject::invokeMethod(
        m_worker,
        "loadModel",
        Qt::QueuedConnection,
        Q_ARG(QString, m_settings.modelPath),
        Q_ARG(int, m_settings.gpuLayers));
}

void MainWindow::setInputEnabled(bool enabled)
{
    ui->inputEdit->setEnabled(enabled);
    ui->sendButton->setEnabled(enabled);
}

void MainWindow::onSendClicked()
{
    const QString text = ui->inputEdit->text().trimmed();
    if (text.isEmpty() || !m_modelReady || m_generating) {
        return;
    }

    m_generating = true;
    setInputEnabled(false);
    ui->inputEdit->clear();

    m_chatWidget->addUserMessage(text);
    statusBar()->showMessage(QStringLiteral("Generating response..."));

    QMetaObject::invokeMethod(
        m_worker,
        "generateResponse",
        Qt::QueuedConnection,
        Q_ARG(QString, text),
        Q_ARG(float, m_settings.temperature),
        Q_ARG(int, m_settings.maxTokens));
}

void MainWindow::onSettingsTriggered()
{
    SettingsDialog dialog(m_settings, this);
    if (dialog.exec() == QDialog::Accepted) {
        const AppSettings updated = dialog.settings();
        const bool pathChanged = updated.modelPath != m_settings.modelPath
                                 || updated.gpuLayers != m_settings.gpuLayers;
        m_settings = updated;

        if (pathChanged) {
            m_modelReady = false;
            setInputEnabled(false);
            statusBar()->showMessage(QStringLiteral("Reloading model..."));
            loadModel();
        }
    }
}

void MainWindow::onAboutTriggered()
{
    showAboutDialog();
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(
        this,
        QStringLiteral("About AI Doctor"),
        QStringLiteral(
            "<h3>AI Doctor v1.0</h3>"
            "<p>Offline AI medical assistant powered by llama.cpp and GGUF models.</p>"
            "<p>Provides general health information for educational purposes only.</p>"
            "<p><b>Not a substitute for professional medical advice.</b></p>"));
}

void MainWindow::onModelLoaded(bool success, const QString& message)
{
    m_modelReady = success;
    setInputEnabled(success);
    statusBar()->showMessage(message);

    if (!success) {
        m_chatWidget->addSystemMessage(
            QStringLiteral("Could not load the model. Open Settings and select a valid GGUF file."));
        QMessageBox::warning(
            this,
            QStringLiteral("Model Load Failed"),
            QStringLiteral("Failed to load the GGUF model.\n\n%1\n\n"
                           "Place a model in models/model.gguf or browse to one in Settings.")
                .arg(message));
    }
}

void MainWindow::onResponseReady(const QString& response)
{
    m_generating = false;
    setInputEnabled(true);

    m_chatWidget->addAssistantMessage(response);
    statusBar()->showMessage(QStringLiteral("Ready"));
}

void MainWindow::onErrorOccurred(const QString& error)
{
    m_generating = false;
    setInputEnabled(m_modelReady);
    m_chatWidget->addSystemMessage(error);
    statusBar()->showMessage(QStringLiteral("Error"));
}
