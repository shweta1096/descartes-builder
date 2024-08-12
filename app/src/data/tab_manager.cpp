#include "data/tab_manager.hpp"

#include <QDebug>

#include <QtNodes/DagGraphicsScene>
#include <QtNodes/GraphicsView>

#include "data/block_manager.hpp"
#include "data/custom_graph.hpp"

using QtNodes::DagGraphicsScene;
using QtNodes::GraphicsView;

TabComponents::TabComponents(QWidget *parent, std::optional<QFileInfo> fileInfo)
    : m_graph(new CustomGraph(BlockManager::getRegistry()))
    , m_scene(new DagGraphicsScene(*m_graph, parent))
    , m_view(new GraphicsView(m_scene))
    , m_dir(std::make_shared<QTemporaryDir>())
{
    m_graph->setParent(parent);
    if (fileInfo)
        m_localFile = fileInfo.value();
    qDebug() << m_localFile;
    if (!m_dir->isValid())
        qCritical() << "Temp dir failed to init";
    qDebug() << m_dir->path();
    // Qt bug for MacOS throws warnings when using touch pad with graphics view
    // touch pad seems to trigger touch events, so touch events are disabled to supress the bug
    m_view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    QObject::connect(m_scene, &DagGraphicsScene::sceneLoaded, m_view, &GraphicsView::centerScene);
    if (parent)
        QObject::connect(m_scene, &DagGraphicsScene::modified, parent, [parent]() {
            parent->setWindowModified(true);
        });
}

TabComponents::~TabComponents()
{
    m_view->deleteLater();
    m_scene->deleteLater();
    m_graph->deleteLater();
}

TabManager::TabManager(QObject *parent)
    : QObject(parent)
{}

std::shared_ptr<TabComponents> TabManager::getCurrentTab() const
{
    return getTab(m_currentView);
}

std::shared_ptr<TabComponents> TabManager::getTab(QWidget *view) const
{
    if (m_tabs.count(view) < 1)
        return std::shared_ptr<TabComponents>();
    return m_tabs.at(view);
}

CustomGraph *TabManager::currentGraph() const
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

bool TabManager::addTab(std::shared_ptr<TabComponents> tab)
{
    if (!tab || m_tabs.count(tab->getView()) > 0)
        return false;
    m_tabs[tab->getView()] = tab;
    emit tabCreated(tab->getView());
    setCurrentView(tab->getView());
    return true;
}

void TabManager::removeTab(const TabComponents &tab)
{
    return removeTab(tab.getView());
}

void TabManager::removeTab(ViewWidget *view)
{
    m_tabs.erase(view);
    emit tabDeleted(view);
}

void TabManager::setCurrentView(ViewWidget *view)
{
    if (m_currentView == view)
        return;
    // qDebug() << "Current view changed to: " << m_tabs.at(view).getScene()->getFile().baseName();
    m_currentView = view;
    emit currentChanged(view);
}

QFileInfo TabManager::getFileInfo(ViewWidget *view) const
{
    if (m_tabs.count(view) < 1)
        return QFileInfo();
    return m_tabs.at(view)->getScene()->getFile();
}

void TabManager::clear()
{
    auto it = m_tabs.begin();
    while (it != m_tabs.end()) {
        auto view = it->first;
        it = m_tabs.erase(it);
        emit tabDeleted(view);
    }
}

void TabManager::newTab()
{
    addTab(std::make_shared<TabComponents>(m_tabParent));
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
    auto tab = std::make_shared<TabComponents>(m_tabParent);
    auto scene = tab->getScene();
    if (!scene || !scene->load() || openIfExists(scene->getFile()))
        return false;
    return addTab(tab);
}

bool TabManager::openFrom(const QString &filePath)
{
    // open in a new tab
    QFileInfo file(filePath);
    auto tab = std::make_shared<TabComponents>(m_tabParent, file);
    auto scene = tab->getScene();
    if (!scene || !scene->load(filePath) || openIfExists(file))
        return false;
    return addTab(tab);
}

bool TabManager::openIfExists(const QFileInfo &file)
{
    if (!file.exists())
        return false;
    for (auto tab : m_tabs)
        if (tab.second->getFileInfo() == file) { // file exists, open that tab
            setCurrentView(tab.first);
            return true;
        }
    return false;
}
