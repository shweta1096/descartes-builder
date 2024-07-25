#include "data/block_manager.hpp"

#include <QDebug>

#include <QtNodes/DagGraphicsScene>

#include "data/tab_manager.hpp"
#include "ui/models/fdf_block_model.hpp"

using QtNodes::DagGraphicsScene;

BlockManager::BlockManager(std::shared_ptr<TabManager> tabManager, QObject *parent)
    : QObject(parent)
    , m_tabManager(tabManager)
{
    connect(m_tabManager.get(),
            &TabManager::currentChanged,
            this,
            &BlockManager::onSelectionChanged);
    connect(m_tabManager.get(), &TabManager::newTabCreated, this, &BlockManager::onNewTabCreated);
}

void BlockManager::onNewTabCreated(QWidget *view)
{
    if (auto tab = m_tabManager->getTab(view))
        connect(tab->getScene(),
                &DagGraphicsScene::selectionChanged,
                this,
                &BlockManager::onSelectionChanged);
}

void BlockManager::onSelectionChanged()
{
    auto scene = m_tabManager->currentScene();
    if (!scene)
        return emit nodeSelected(QtNodes::InvalidNodeId);
    auto selection = scene->selectedNodes();
    emit nodeSelected(selection.size() < 1 ? QtNodes::InvalidNodeId : selection.at(0));
}
