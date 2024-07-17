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

signals:
    void callAppendMessage(const QString &message, const QtMsgType &type);

public slots:
    void appendMessage(const QString &message, const QtMsgType &type = QtInfoMsg);

private:
    LogManager() = default;
    ~LogManager() = default;
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;

    QVector<LogPanel *> m_logPanels;
    struct LogMessage
    {
        QString string;
        QColor color;
    };
    std::vector<LogMessage> m_notPrinted;
};