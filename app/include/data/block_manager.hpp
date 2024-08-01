#pragma once

#include <QObject>

#include <QtNodes/Definitions>

namespace QtNodes {
class NodeDelegateModelRegistry;
}

class TabManager;

class BlockManager : public QObject
{
    Q_OBJECT
public:
    BlockManager(QObject *parent = nullptr);
    static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> getRegistry() { return m_registry; }
    void setTabManager(std::shared_ptr<TabManager> tabManager);
    QJsonObject getJson(QtNodes::NodeId id);

signals:
    void nodeSelected(QtNodes::NodeId id);

public slots:
    void onSelectionChanged();

private slots:
    void onNewTabCreated(QWidget *view);

private:
    static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> m_registry;
    std::shared_ptr<TabManager> m_tabManager;
    std::vector<QtNodes::NodeId> m_selectedNodes;
};
