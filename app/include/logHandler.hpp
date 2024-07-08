#pragma once

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QApplication>
#include <QDir>

#include <iostream>

#include "file/file.hpp"

namespace log
{
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

        QByteArray localMsg = msg.toLocal8Bit();
        const char *file = context.file ? context.file : "";
        const char *function = context.function ? context.function : "";
        switch (type)
        {
        case QtDebugMsg:
            out << "Debug: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            std::cerr << "Debug: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            break;
        case QtInfoMsg:
            out << "Info: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            std::cerr << "Info: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            break;
        case QtWarningMsg:
            out << "Warning: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            std::cerr << "Warning: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            break;
        case QtCriticalMsg:
            out << "Critical: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            std::cerr << "Critical: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            break;
        case QtFatalMsg:
            out << "Fatal: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            std::cerr << "Fatal: " << localMsg.constData() << " (" << file << ":" << context.line << ", " << function << ")\n";
            abort();
        }
        out.flush();
    }
}