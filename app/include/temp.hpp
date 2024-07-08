#pragma once

#include <QObject>

class Temp : public QObject
{
    Q_OBJECT
public:
    Temp(QObject *parent = nullptr);

public slots:
    bool runPython();
};
