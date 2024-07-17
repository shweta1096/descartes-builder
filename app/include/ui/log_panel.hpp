#pragma once

#include <QPlainTextEdit>
#include <QMutex>
#include <QColor>

class LogPanel : public QPlainTextEdit
{
    Q_OBJECT
public:
    LogPanel(QWidget *parent = nullptr);

    void appendMessage(const QString &text, const QColor &color = QColor());

private:
    const QColor m_DEFAULT_TEXT_COLOR;
};