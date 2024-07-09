#pragma once

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QApplication>
#include <QDir>

#include <iostream>

#include "file/file.hpp"

#include "ui/log_panel.hpp"

class LogManager : public QObject
{
    Q_OBJECT
public:
    static LogManager &instance();
    void init();
    void registerLogPanel(LogPanel *panel);
    void appendMessage(const QString &message, const QtMsgType &type = QtInfoMsg);

private:
    LogManager() = default;
    ~LogManager() = default;
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;

    QVector<LogPanel *> m_logPanels;
    QMutex m_mutex;
};