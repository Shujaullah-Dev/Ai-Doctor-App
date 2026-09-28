#pragma once

#include <QString>
#include <memory>
#include <mutex>
#include <string>

struct llama_context;
struct llama_model;
struct llama_sampler;

// Wraps llama.cpp for local GGUF model loading and text generation.
// All public methods are thread-safe via an internal mutex.
class LLMEngine {
public:
    LLMEngine();
    ~LLMEngine();

    LLMEngine(const LLMEngine&) = delete;
    LLMEngine& operator=(const LLMEngine&) = delete;

    // Load a GGUF model from disk. Returns false on failure.
    bool loadModel(const QString& modelPath, int gpuLayers = 0);

    // Release model and context resources.
    void unloadModel();

    bool isLoaded() const;

    // Generate a response for a user message. The system prompt is prepended
    // automatically. Each call is stateless (KV cache is cleared first).
    QString generateResponse(const QString& userMessage,
                             float temperature = 0.7f,
                             int maxTokens = 512);

    // Load the system prompt text from prompts/system_prompt.txt
    static QString loadSystemPrompt();

private:
    QString buildFullPrompt(const QString& userMessage) const;
    void rebuildSampler(float temperature);
    void unloadModelUnlocked();

    llama_model*   m_model   = nullptr;
    llama_context* m_context = nullptr;
    llama_sampler* m_sampler = nullptr;

    QString m_systemPrompt;
    float   m_lastTemperature = -1.0f;

    mutable std::mutex m_mutex;
};
