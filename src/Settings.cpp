#include "Settings.h"
#include "ui_Settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

QString AppSettings::defaultModelPath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        appDir + "/models/model.gguf",
        appDir + "/../models/model.gguf",
        appDir + "/../../models/model.gguf",
        QDir::currentPath() + "/models/model.gguf",
    };

    for (const QString& path : candidates) {
        if (QFileInfo::exists(path)) {
            return QDir::toNativeSeparators(path);
        }
    }
    return QDir::toNativeSeparators(candidates.first());
}

AppSettings AppSettings::load()
{
    QSettings settings(QStringLiteral("AIDoctor"), QStringLiteral("AIDoctor"));

    AppSettings cfg;
    cfg.modelPath   = settings.value(QStringLiteral("modelPath"), defaultModelPath()).toString();
    cfg.temperature = settings.value(QStringLiteral("temperature"), 0.7).toFloat();
    cfg.maxTokens   = settings.value(QStringLiteral("maxTokens"), 512).toInt();
    cfg.gpuLayers   = settings.value(QStringLiteral("gpuLayers"), 0).toInt();
    return cfg;
}

void AppSettings::save() const
{
    QSettings settings(QStringLiteral("AIDoctor"), QStringLiteral("AIDoctor"));
    settings.setValue(QStringLiteral("modelPath"), modelPath);
    settings.setValue(QStringLiteral("temperature"), temperature);
    settings.setValue(QStringLiteral("maxTokens"), maxTokens);
    settings.setValue(QStringLiteral("gpuLayers"), gpuLayers);
}

SettingsDialog::SettingsDialog(const AppSettings& current, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , m_settings(current)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("Settings"));
    populate(current);

    connect(ui->browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseClicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

AppSettings SettingsDialog::settings() const
{
    return m_settings;
}

void SettingsDialog::populate(const AppSettings& settings)
{
    ui->modelPathEdit->setText(settings.modelPath);
    ui->temperatureSpin->setValue(settings.temperature);
    ui->maxTokensSpin->setValue(settings.maxTokens);
    ui->gpuLayersSpin->setValue(settings.gpuLayers);
}

void SettingsDialog::onBrowseClicked()
{
    const QString file = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Select GGUF Model"),
        QFileInfo(ui->modelPathEdit->text()).absolutePath(),
        QStringLiteral("GGUF Models (*.gguf);;All Files (*)"));

    if (!file.isEmpty()) {
        ui->modelPathEdit->setText(QDir::toNativeSeparators(file));
    }
}

void SettingsDialog::onApplyClicked()
{
    m_settings.modelPath   = ui->modelPathEdit->text().trimmed();
    m_settings.temperature = static_cast<float>(ui->temperatureSpin->value());
    m_settings.maxTokens   = ui->maxTokensSpin->value();
    m_settings.gpuLayers   = ui->gpuLayersSpin->value();
    m_settings.save();
    accept();
}
