#include "temp.hpp"

#include <QDebug>

#include "python/py_process.hpp"

Temp::Temp(QObject *parent) : QObject(parent)
{
}

bool Temp::runPython()
{
    QStringList args = {"Hello", "World"};
    QResource script(":/test.py");

    auto output = python::run(script, args);
    if (!output.error.isEmpty())
        qCritical() << output.error;
    else
        qInfo() << output.output;
    return true;
}

void Temp::printInfo()
{
    qInfo() << "This is an info print";
}

void Temp::printDebug()
{
    qDebug() << "This is a debug print";
}

void Temp::printError()
{
    qCritical() << "This is an error print";
}