#include "data/tab_manager.hpp"

#include <QDebug>

#include <QtNodes/DagGraphicsScene>
#include <QtNodes/DirectedAcyclicGraphModel>
#include <QtNodes/GraphicsView>

#include "data/block_manager.hpp"

using QtNodes::DagGraphicsScene;
using QtNodes::DirectedAcyclicGraphModel;
using QtNodes::GraphicsView;

TabComponents::TabComponents(QWidget *parent)
    : m_graph(new DirectedAcyclicGraphModel(BlockManager::getRegistry()))
    , m_scene(new DagGraphicsScene(*m_graph, parent))
    , m_view(new GraphicsView(m_scene))
{
    m_graph->setParent(parent);
    // Qt bug for MacOS throws warnings when using touch pad with graphics view
    // touch pad seems to trigger touch events, so touch events are disabled to supress the bug
    m_view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    QObject::connect(m_scene, &DagGraphicsScene::sceneLoaded, m_view, &GraphicsView::centerScene);
    if (parent)
        QObject::connect(m_scene, &DagGraphicsScene::modified, parent, [parent]() {
            parent->setWindowModified(true);
        });
}

QFileInfo TabComponents::getFile() const
{
    return m_scene->getFile();
}

TabManager::TabManager(QObject *parent)
    : QObject(parent)
{}

void TabManager::setBlockManager(std::shared_ptr<BlockManager> blockManager)
{
    if (m_blockManager == blockManager)
        return;
    m_blockManager = blockManager;
}

std::optional<TabComponents> TabManager::getCurrentTab() const
{
    return getTab(m_currentView);
}

std::optional<TabComponents> TabManager::getTab(QWidget *view) const
{
    if (m_tabs.count(view) < 1)
        return std::nullopt;
    return m_tabs.at(view);
}

DirectedAcyclicGraphModel *TabManager::currentGraph() const
{
    if (auto tab = getCurrentTab())
        return tab->getGraph();
    return nullptr;
}

DagGraphicsScene *TabManager::currentScene() const
{
    if (auto tab = getCurrentTab())
        return tab->getScene();
    return nullptr;
}

GraphicsView *TabManager::currentView() const
{
    if (auto tab = getCurrentTab())
        return tab->getView();
    return nullptr;
}

QString TabManager::currentTabName() const
{
    if (auto tab = getCurrentTab())
        return tab->getScene()->getFile().baseName();
    return QString();
}

bool TabManager::addTab(const TabComponents &tab)
{
    if (m_tabs.count(tab.getView()) > 0)
        return false;
    m_tabs[tab.getView()] = std::move(tab);
    emit newTabCreated(tab.getView(), tab.getScene()->getFile().baseName());
    emit currentChanged(tab.getView());
    return true;
}

void TabManager::removeTab(const TabComponents &tab)
{
    return removeTab(tab.getView());
}

void TabManager::removeTab(ViewWidget *view)
{
    m_tabs.erase(view);
}

void TabManager::newTab()
{
    TabComponents tab(m_tabParent);
    addTab(tab);
}

bool TabManager::save()
{
    auto tab = getCurrentTab();
    if (!tab)
        return false;
    auto scene = tab->getScene();
    if (!scene || !scene->save())
        return false;
    qInfo() << "File saved to: " << scene->getFile().absoluteFilePath();
    emit tabFileNameChanged(tab->getView(), scene->getFile().baseName());
    return true;
}

bool TabManager::saveAs()
{
    auto tab = getCurrentTab();
    if (!tab)
        return false;
    auto scene = tab->getScene();
    if (!scene || !scene->saveAs())
        return false;
    qInfo() << "File saved as: " << scene->getFile().absoluteFilePath();
    emit tabFileNameChanged(tab->getView(), scene->getFile().baseName());
    return true;
}

bool TabManager::open()
{
    // open in a new tab
    TabComponents tab(m_tabParent);
    auto scene = tab.getScene();
    if (!scene || !scene->load() || openIfExists(scene))
        return false;
    return addTab(tab);
}

bool TabManager::openFrom(const QString &filePath)
{
    // open in a new tab
    TabComponents tab(m_tabParent);
    auto scene = tab.getScene();
    if (!scene || !scene->load(filePath) || openIfExists(scene))
        return false;
    return addTab(tab);
}

bool TabManager::openIfExists(QtNodes::DagGraphicsScene *scene)
{
    QFileInfo a;
    if (!scene)
        return false;
    for (auto tab : m_tabs)
        if (tab.second.getFile() == scene->getFile()) { // file exists, open that tab
            // setCurrentWidget(tab.first);
            return true;
        }
    return false;
}
