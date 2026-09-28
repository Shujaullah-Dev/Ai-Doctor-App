#include "ChatWidget.h"

#include <QScrollBar>
#include <QTextEdit>
#include <QVBoxLayout>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
{
    m_chatView = new QTextEdit(this);
    m_chatView->setReadOnly(true);
    m_chatView->setPlaceholderText(QStringLiteral("Ask a health question to get started..."));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_chatView);
}

void ChatWidget::addUserMessage(const QString& text)
{
    appendMessage(QStringLiteral("You"), text, QStringLiteral("#89b4fa"));
}

void ChatWidget::addAssistantMessage(const QString& text)
{
    appendMessage(QStringLiteral("AI Doctor"), text, QStringLiteral("#a6e3a1"));
}

void ChatWidget::addSystemMessage(const QString& text)
{
    appendMessage(QStringLiteral("System"), text, QStringLiteral("#f9e2af"));
}

void ChatWidget::clearChat()
{
    m_chatView->clear();
}

void ChatWidget::appendMessage(const QString& role, const QString& text, const QString& color)
{
    const QString html = QStringLiteral(
                             "<div style='margin-bottom:14px;'>"
                             "<span style='color:%1; font-weight:600;'>%2</span><br/>"
                             "<span style='color:#cdd6f4;'>%3</span>"
                             "</div>")
                             .arg(color, role.toHtmlEscaped(), text.toHtmlEscaped().replace(QLatin1Char('\n'), QStringLiteral("<br/>")));

    m_chatView->append(html);
    m_chatView->verticalScrollBar()->setValue(m_chatView->verticalScrollBar()->maximum());
}
