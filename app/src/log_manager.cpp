#include "log_manager.hpp"

namespace
{
    std::map<QtMsgType, QColor> TYPE_COLOR = {
        {QtWarningMsg, Qt::yellow},
        {QtCriticalMsg, Qt::red},
        {QtFatalMsg, Qt::red},
    };
    std::map<QtMsgType, QString>
        TYPE_STRING = {
            {QtDebugMsg, "debug"},
            {QtInfoMsg, "info"},
            {QtWarningMsg, "warning"},
            {QtCriticalMsg, "error"},
            {QtFatalMsg, "fatal"},
    };

    QString getApplicationPath()
    {
        auto dirPath = QApplication::applicationDirPath();
        int appIndex = dirPath.indexOf(".app");
        if (appIndex != -1)
        { // macOs will return a path within the bundled .app
            int endIndex = dirPath.lastIndexOf('/', appIndex);
            dirPath = dirPath.left(endIndex);
        }
        return dirPath;
    }

    QFileInfo getLogFile()
    {
        static QFileInfo fileInfo;
        if (fileInfo.filePath().isEmpty())
        {
            const QString FILE_NAME = QDateTime::currentDateTime().toString("yyyy-MM-dd_h-mmAP") + ".log";
            QDir logDir(getApplicationPath() + "/logs");
            QFileInfo info(logDir, FILE_NAME);
            fileInfo = file::createUnique(info);
        }
        return fileInfo;
    }

    void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        static QFile logFile(getLogFile().absoluteFilePath());
        if (!logFile.isOpen())
        {
            logFile.open(QIODevice::Append | QIODevice::Text);
        }
        QTextStream out(&logFile);

        // use qSetMessagePattern to format
        auto message = qFormatLogMessage(type, context, msg);

        out << message << "\n";
        std::cerr << message.toStdString() << std::endl;

        emit LogManager::instance().callAppendMessage(message, type);

        out.flush();

        if (type == QtFatalMsg)
            abort();
    }
}

LogManager &LogManager::instance()
{
    static LogManager instance;
    return instance;
}

void LogManager::init()
{
    // this must be queued connection to trigger appendMessage on a GUI thread
    connect(this, &LogManager::callAppendMessage, this, &LogManager::appendMessage, Qt::QueuedConnection);
    qSetMessagePattern("[%{time yyyy-MM-dd HH:mm:ss.zzz}] [%{if-debug}debug%{endif}%{if-info}info%{endif}%{if-warning}warning%{endif}%{if-critical}error%{endif}%{if-fatal}fatal%{endif}] %{file}:%{line}<%{function}>: %{message}");
    qInstallMessageHandler(logHandler);
}

void LogManager::registerLogPanel(LogPanel *panel)
{
    if (!m_logPanels.contains(panel))
    {
        m_logPanels.push_back(panel);
        if (!m_notPrinted.empty())
        {
            for (auto message : m_notPrinted)
                panel->appendMessage(message.string, message.color);
            m_notPrinted.clear();
        }
    }
}

void LogManager::appendMessage(const QString &message, const QtMsgType &type)
{
    QColor color;
    if (TYPE_COLOR.count(type) > 0)
        color = TYPE_COLOR[type];
    for (LogPanel *panel : m_logPanels)
        if (panel)
            panel->appendMessage(message, color);
    if (m_logPanels.isEmpty())
        m_notPrinted.push_back({message, color});
}
