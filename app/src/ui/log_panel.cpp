#include "ui/log_panel.hpp"

#include "log_manager.hpp"

LogPanel::LogPanel(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
    LogManager::instance().registerLogPanel(this);
}