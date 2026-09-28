#include "LLMEngine.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <llama.h>

#include <vector>

namespace {

QString resolvePromptPath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        appDir + "/prompts/system_prompt.txt",
        appDir + "/../prompts/system_prompt.txt",
        QDir::currentPath() + "/prompts/system_prompt.txt",
    };

    for (const QString& path : candidates) {
        if (QFile::exists(path)) {
            return path;
        }
    }
    return candidates.last();
}

} // namespace

LLMEngine::LLMEngine()
{
    m_systemPrompt = loadSystemPrompt();
}

LLMEngine::~LLMEngine()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    unloadModelUnlocked();
}

QString LLMEngine::loadSystemPrompt()
{
    QFile file(resolvePromptPath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringLiteral(
            "You are an AI medical assistant. Provide educational medical information only. "
            "Do not diagnose. Recommend consulting a healthcare professional.");
    }

    QTextStream in(&file);
    return in.readAll().trimmed();
}

bool LLMEngine::loadModel(const QString& modelPath, int gpuLayers)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_model) {
        unloadModelUnlocked();
    }

    llama_backend_init();
    ggml_backend_load_all();

    llama_model_params modelParams = llama_model_default_params();
    modelParams.n_gpu_layers = gpuLayers;

    m_model = llama_model_load_from_file(modelPath.toUtf8().constData(), modelParams);
    if (!m_model) {
        return false;
    }

    llama_context_params ctxParams = llama_context_default_params();
    ctxParams.n_ctx   = 4096;
    ctxParams.n_batch = 512;
    ctxParams.no_perf = true;

    m_context = llama_init_from_model(m_model, ctxParams);
    if (!m_context) {
        llama_model_free(m_model);
        m_model = nullptr;
        return false;
    }

    rebuildSampler(0.7f);
    return true;
}

void LLMEngine::unloadModel()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    unloadModelUnlocked();
}

void LLMEngine::unloadModelUnlocked()
{
    if (m_sampler) {
        llama_sampler_free(m_sampler);
        m_sampler = nullptr;
    }
    if (m_context) {
        llama_free(m_context);
        m_context = nullptr;
    }
    if (m_model) {
        llama_model_free(m_model);
        m_model = nullptr;
    }
    m_lastTemperature = -1.0f;
}

bool LLMEngine::isLoaded() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_model != nullptr && m_context != nullptr;
}

void LLMEngine::rebuildSampler(float temperature)
{
    if (m_sampler) {
        llama_sampler_free(m_sampler);
        m_sampler = nullptr;
    }

    auto sparams = llama_sampler_chain_default_params();
    sparams.no_perf = true;
    m_sampler = llama_sampler_chain_init(sparams);
    llama_sampler_chain_add(m_sampler, llama_sampler_init_temp(temperature));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
    m_lastTemperature = temperature;
}

QString LLMEngine::buildFullPrompt(const QString& userMessage) const
{
    return QStringLiteral(
               "System: %1\n\n"
               "User: %2\n\n"
               "Assistant:")
        .arg(m_systemPrompt, userMessage.trimmed());
}

QString LLMEngine::generateResponse(const QString& userMessage,
                                    float temperature,
                                    int maxTokens)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_model || !m_context || !m_sampler) {
        return QStringLiteral("Error: Model is not loaded.");
    }

    if (temperature != m_lastTemperature) {
        rebuildSampler(temperature);
    }

    // Each question is independent — clear KV cache before generating.
    llama_memory_clear(llama_get_memory(m_context), true);

    const QString fullPrompt = buildFullPrompt(userMessage);
    const std::string promptUtf8 = fullPrompt.toUtf8().constData();

    const llama_vocab* vocab = llama_model_get_vocab(m_model);

    const int nPrompt = -llama_tokenize(
        vocab, promptUtf8.c_str(), static_cast<int32_t>(promptUtf8.size()),
        nullptr, 0, true, true);

    if (nPrompt <= 0) {
        return QStringLiteral("Error: Failed to tokenize the prompt.");
    }

    std::vector<llama_token> tokens(static_cast<size_t>(nPrompt));
    if (llama_tokenize(vocab, promptUtf8.c_str(),
                       static_cast<int32_t>(promptUtf8.size()),
                       tokens.data(), static_cast<int32_t>(tokens.size()),
                       true, true) < 0) {
        return QStringLiteral("Error: Failed to tokenize the prompt.");
    }

    llama_batch batch = llama_batch_get_one(tokens.data(), static_cast<int32_t>(tokens.size()));

    if (llama_model_has_encoder(m_model)) {
        if (llama_encode(m_context, batch) != 0) {
            return QStringLiteral("Error: Model encoding failed.");
        }

        llama_token decoderStart = llama_model_decoder_start_token(m_model);
        if (decoderStart == LLAMA_TOKEN_NULL) {
            decoderStart = llama_vocab_bos(vocab);
        }
        batch = llama_batch_get_one(&decoderStart, 1);
    }

    QString response;
    int generated = 0;

    while (generated < maxTokens) {
        if (llama_decode(m_context, batch) != 0) {
            return QStringLiteral("Error: Inference failed during decoding.");
        }

        const llama_token newToken = llama_sampler_sample(m_sampler, m_context, -1);

        if (llama_vocab_is_eog(vocab, newToken)) {
            break;
        }

        char piece[256];
        const int n = llama_token_to_piece(vocab, newToken, piece, sizeof(piece), 0, true);
        if (n > 0) {
            response += QString::fromUtf8(piece, n);
        }

        batch = llama_batch_get_one(const_cast<llama_token*>(&newToken), 1);
        ++generated;
    }

    return response.trimmed();
}
