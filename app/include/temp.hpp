// this class is meant to hold temporary code for testing purpose
// will be removed in the future or excluded from compilation
#pragma onc

#include <QObject>

class Temp : public QObject
{
    Q_OBJECT
public:
    Temp(QObject *parent = nullptr);

public slots:
    bool runPython();
    void printInfo();
    void printDebug();
    void printError();
};
