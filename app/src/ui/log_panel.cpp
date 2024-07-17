#include "ui/log_panel.hpp"

#include "log_manager.hpp"

LogPanel::LogPanel(QWidget *parent)
    : QPlainTextEdit(parent),
      m_DEFAULT_TEXT_COLOR(QApplication::palette().text().color())
{
    setReadOnly(true);
    LogManager::instance().registerLogPanel(this);
}

void LogPanel::appendMessage(const QString &text, const QColor &color)
{
    QTextCharFormat format = currentCharFormat();
    format.setForeground(QBrush(color.isValid() ? color : m_DEFAULT_TEXT_COLOR));
    setCurrentCharFormat(format);
    appendPlainText(text);
}