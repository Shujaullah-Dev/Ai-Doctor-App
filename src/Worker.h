#pragma once

#include <QObject>
#include <QString>

#include <memory>

class LLMEngine;

// Runs LLM inference on a background thread. The GUI communicates with this
// worker exclusively through Qt signals and slots.
class Worker : public QObject {
    Q_OBJECT

public:
    explicit Worker(QObject* parent = nullptr);
    ~Worker() override;

public slots:
    void loadModel(const QString& modelPath, int gpuLayers);
    void generateResponse(const QString& userMessage, float temperature, int maxTokens);
    void unloadModel();

signals:
    void modelLoaded(bool success, const QString& message);
    void responseReady(const QString& response);
    void errorOccurred(const QString& error);

private:
    std::unique_ptr<LLMEngine> m_engine;
};
