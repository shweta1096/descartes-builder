#include "data/block_manager.hpp"

#include <QDebug>

BlockManager::BlockManager(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Block Manager Init";
}
