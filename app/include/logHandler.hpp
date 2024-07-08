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

namespace
{
    std::map<QtMsgType, QString> TYPE_STRING = {
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
}

namespace log
{
    void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        static QFile logFile(getLogFile().absoluteFilePath());
        if (!logFile.isOpen())
        {
            logFile.open(QIODevice::Append | QIODevice::Text);
        }
        QTextStream out(&logFile);

        QByteArray localMsg = msg.toLocal8Bit();
        const QString time = QTime::currentTime().toString("HH:mm:ss.zzz");
        const char *file = context.file ? context.file : "";
        const char *function = context.function ? context.function : "";

        const QString message = QString("[%1] [%2] %3:%4<%5>: %6").arg(time).arg(TYPE_STRING.at(type)).arg(file).arg(context.line).arg(function).arg(localMsg.constData());
        out << message << "\n";
        std::cerr << message.toStdString() << std::endl;

        LogManager::instance().appendMessage(message);

        out.flush();

        if (type == QtFatalMsg)
            abort();
    }
}