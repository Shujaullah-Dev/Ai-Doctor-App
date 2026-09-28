#pragma once

#include <QDialog>
#include <QString>

namespace Ui {
class SettingsDialog;
}

// Application configuration values used by the LLM engine.
struct AppSettings {
    QString modelPath;
    float   temperature  = 0.7f;
    int     maxTokens    = 512;
    int     gpuLayers    = 0;

    static AppSettings load();
    void save() const;
    static QString defaultModelPath();
};

// Settings dialog for model path, temperature, max tokens, and GPU layers.
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(const AppSettings& current, QWidget* parent = nullptr);
    ~SettingsDialog() override;

    AppSettings settings() const;

private slots:
    void onBrowseClicked();
    void onApplyClicked();

private:
    void populate(const AppSettings& settings);

    Ui::SettingsDialog* ui = nullptr;
    AppSettings m_settings;
};
