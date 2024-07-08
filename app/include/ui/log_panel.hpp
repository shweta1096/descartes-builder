#pragma once

#include <QTextEdit>
#include <QMutex>

class LogPanel : public QTextEdit
{
    Q_OBJECT
public:
    LogPanel(QWidget *parent = nullptr);
};