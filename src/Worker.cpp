#include "Worker.h"
#include "LLMEngine.h"

Worker::Worker(QObject* parent)
    : QObject(parent)
    , m_engine(std::make_unique<LLMEngine>())
{
}

Worker::~Worker()
{
    if (m_engine) {
        m_engine->unloadModel();
    }
}

void Worker::loadModel(const QString& modelPath, int gpuLayers)
{
    if (!m_engine) {
        emit modelLoaded(false, QStringLiteral("Engine not initialized."));
        return;
    }

    const bool ok = m_engine->loadModel(modelPath, gpuLayers);
    if (ok) {
        emit modelLoaded(true, QStringLiteral("Model loaded successfully."));
    } else {
        emit modelLoaded(false, QStringLiteral("Failed to load model: %1").arg(modelPath));
    }
}

void Worker::generateResponse(const QString& userMessage, float temperature, int maxTokens)
{
    if (!m_engine || !m_engine->isLoaded()) {
        emit errorOccurred(QStringLiteral("Model is not loaded. Check Settings."));
        return;
    }

    const QString response = m_engine->generateResponse(userMessage, temperature, maxTokens);
    if (response.startsWith(QStringLiteral("Error:"))) {
        emit errorOccurred(response);
    } else {
        emit responseReady(response);
    }
}

void Worker::unloadModel()
{
    if (m_engine) {
        m_engine->unloadModel();
    }
}
