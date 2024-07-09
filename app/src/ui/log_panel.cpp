#include "ui/log_panel.hpp"

#include "log_manager.hpp"

LogPanel::LogPanel(QWidget *parent)
    : QTextEdit(parent),
      m_DEFAULT_TEXT_COLOR(QApplication::palette().text().color())
{
    setReadOnly(true);
    LogManager::instance().registerLogPanel(this);
}

void LogPanel::appendMessage(const QString &text, const QColor &color)
{
    setTextColor(color.isValid() ? color : m_DEFAULT_TEXT_COLOR);
    append(text);
}