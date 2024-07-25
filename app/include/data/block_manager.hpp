#pragma once

#include <QObject>

#include <QtNodes/Definitions>

class TabManager;

class BlockManager : public QObject
{
    Q_OBJECT
public:
    BlockManager(std::shared_ptr<TabManager> tabManager, QObject *parent = nullptr);

signals:
    void nodeSelected(QtNodes::NodeId id);

public slots:
    void onSelectionChanged();

private slots:
    void onNewTabCreated(QWidget* view);

private:
    std::shared_ptr<TabManager> m_tabManager;
};
