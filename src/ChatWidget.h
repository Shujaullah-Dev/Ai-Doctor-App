#pragma once

#include <QWidget>

class QTextEdit;

// Displays a ChatGPT-style conversation area with user and AI messages.
class ChatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidget(QWidget* parent = nullptr);

    void addUserMessage(const QString& text);
    void addAssistantMessage(const QString& text);
    void addSystemMessage(const QString& text);
    void clearChat();

private:
    void appendMessage(const QString& role, const QString& text, const QString& color);

    QTextEdit* m_chatView = nullptr;
};
