#include "ui/log_panel.hpp"

LogPanel::LogPanel(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
    LogManager::instance().registerLogPanel(this);
}

LogManager &LogManager::instance()
{
    static LogManager instance;
    return instance;
}

void LogManager::registerLogPanel(LogPanel *panel)
{
    QMutexLocker locker(&m_mutex);
    if (!m_logPanels.contains(panel))
        m_logPanels.push_back(panel);
}

void LogManager::appendMessage(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    for (LogPanel *panel : m_logPanels)
        if (panel)
            panel->append(message);
}