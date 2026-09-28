#include "MainWindow.h"

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

bool showMedicalDisclaimer()
{
    QDialog dialog;
    dialog.setWindowTitle(QStringLiteral("Medical Disclaimer"));
    dialog.setModal(true);
    dialog.setMinimumWidth(520);

    auto* layout = new QVBoxLayout(&dialog);

    auto* title = new QLabel(QStringLiteral("Important Medical Disclaimer"));
    title->setObjectName(QStringLiteral("titleLabel"));
    title->setAlignment(Qt::AlignCenter);

    auto* text = new QLabel(QStringLiteral(
        "This application provides general health information only. It is not a licensed "
        "medical professional and should not be used for diagnosis or emergency medical care. "
        "Always consult a qualified healthcare provider. If you experience severe symptoms "
        "such as chest pain, difficulty breathing, or signs of stroke, seek emergency medical "
        "attention immediately."));
    text->setWordWrap(true);
    text->setObjectName(QStringLiteral("subtitleLabel"));

    auto* buttons = new QDialogButtonBox();
    auto* acceptButton = buttons->addButton(QStringLiteral("I Understand"), QDialogButtonBox::AcceptRole);
    auto* rejectButton = buttons->addButton(QStringLiteral("Exit"), QDialogButtonBox::RejectRole);
    rejectButton->setObjectName(QStringLiteral("secondaryButton"));

    layout->addWidget(title);
    layout->addSpacing(8);
    layout->addWidget(text);
    layout->addSpacing(16);
    layout->addWidget(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    return dialog.exec() == QDialog::Accepted;
}

void loadStyleSheet(QApplication& app)
{
    QFile styleFile(QStringLiteral(":/styles.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }
}

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("AI Doctor"));
    QApplication::setOrganizationName(QStringLiteral("AIDoctor"));

    loadStyleSheet(app);

    if (!showMedicalDisclaimer()) {
        return 0;
    }

    MainWindow window;
    window.show();

    return app.exec();
}
