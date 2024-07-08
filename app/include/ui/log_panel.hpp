#pragma once

#include <QTextEdit>
#include <QMutex>

class LogPanel : public QTextEdit
{
    Q_OBJECT
public:
    LogPanel(QWidget *parent = nullptr);
};

class LogManager : public QObject
{
    Q_OBJECT
public:
    static LogManager &instance();
    void registerLogPanel(LogPanel *panel);
    void appendMessage(const QString &message);

private:
    LogManager() = default;
    ~LogManager() = default;
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;

    QVector<LogPanel *> m_logPanels;
    QMutex m_mutex;
};