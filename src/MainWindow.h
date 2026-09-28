#pragma once

#include <QMainWindow>

#include "Settings.h"

class ChatWidget;
class Worker;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSendClicked();
    void onSettingsTriggered();
    void onAboutTriggered();
    void onModelLoaded(bool success, const QString& message);
    void onResponseReady(const QString& response);
    void onErrorOccurred(const QString& error);

private:
    void setupWorkerThread();
    void loadModel();
    void setInputEnabled(bool enabled);
    void showAboutDialog();

    Ui::MainWindow* ui = nullptr;
    ChatWidget*     m_chatWidget = nullptr;
    Worker*         m_worker     = nullptr;
    class QThread*  m_workerThread = nullptr;

    AppSettings m_settings;
    bool        m_modelReady = false;
    bool        m_generating = false;
};
