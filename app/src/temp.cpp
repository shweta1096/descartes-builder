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
}