#pragma once

#include <QObject>

class BlockManager : public QObject
{
    Q_OBJECT
public:
    BlockManager(QObject *parent = nullptr);
};
