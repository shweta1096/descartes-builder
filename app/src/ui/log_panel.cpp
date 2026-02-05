#include "ui/log_panel.hpp"

#include "log_manager.hpp"
#include <QApplication>
#include <QFontDatabase>

LogPanel::LogPanel(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_DEFAULT_TEXT_COLOR(QApplication::palette().text().color())
{
    setReadOnly(true);
    LogManager::instance().registerLogPanel(this);
}

void LogPanel::appendMessage(const QString &text, const QColor &color)
{
    auto f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(12);
    setFont(f);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    QTextCharFormat format = currentCharFormat();
    format.setForeground(QBrush(color.isValid() ? color : m_DEFAULT_TEXT_COLOR));
    auto cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);
    setCurrentCharFormat(format);
    appendPlainText(text);
}