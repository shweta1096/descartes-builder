#include "ui/graphics_scene_tab_widget.hpp"

#include <QWidget>
#include <QTabBar>
#include <QPushButton>

#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

#include "ui/model_registry.hpp"

namespace
{
    std::shared_ptr<NodeDelegateModelRegistry> registry = model_registry::registerDataModels();
}

TabComponents::TabComponents(QWidget *parent)
    : m_model(new DataFlowGraphModel(registry)),
      m_scene(new DataFlowGraphicsScene(*m_model, parent)),
      m_view(new GraphicsView(m_scene))
{
    m_model->setParent(parent);
    // Qt bug for MacOS throws warnings when using touch pad with graphics view
    // touch pad seems to trigger touch events, so touch events are disabled to supress the bug
    m_view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    QObject::connect(m_scene, &DataFlowGraphicsScene::sceneLoaded, m_view, &GraphicsView::centerScene);
    if (parent)
        QObject::connect(m_scene, &DataFlowGraphicsScene::modified, parent, [parent]()
                         { parent->setWindowModified(true); });
}

GraphicsSceneTabWidget::GraphicsSceneTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    connect(this, &GraphicsSceneTabWidget::tabCloseRequested, this, &GraphicsSceneTabWidget::closeTab);

    auto newTabButton = new QPushButton("+");
    setCornerWidget(newTabButton);
    connect(newTabButton, &QPushButton::clicked, this, &GraphicsSceneTabWidget::addBlankTab);

    connect(this, &GraphicsSceneTabWidget::countChanged, this, &GraphicsSceneTabWidget::onTabCountChanged);

    // init with 1 blank tab
    addBlankTab();
}

void GraphicsSceneTabWidget::addBlankTab()
{
    TabComponents tab(qobject_cast<QWidget *>(parent()));
    int index = addTab(tab.getView(), "blank");
    m_tabs[widget(index)] = std::move(tab);
    setCurrentIndex(index);
}

bool GraphicsSceneTabWidget::save()
{
    auto scene = getCurrentScene();
    if (!scene || !scene->save())
        return false;
    setCurrentTabText(scene->getFile().baseName());
    qInfo() << "File saved to: " << scene->getFile().absoluteFilePath();
    return true;
}

bool GraphicsSceneTabWidget::saveAs()
{
    auto scene = getCurrentScene();
    if (!scene || !scene->saveAs())
        return false;
    setCurrentTabText(scene->getFile().baseName());
    qInfo() << "File saved as: " << scene->getFile().absoluteFilePath();
    return true;
}

bool GraphicsSceneTabWidget::open()
{
    auto scene = getCurrentScene();
    if (scene->isEmpty())
    { // open in current tab
        if (!scene || !scene->load())
            return false;
        setCurrentTabText(scene->getFile().baseName());
        return true;
    }
    // open in a new tab
    TabComponents tab(qobject_cast<QWidget *>(parent()));
    scene = tab.getScene();
    if (!scene || !scene->load())
        return false;
    int index = addTab(tab.getView(), scene->getFile().baseName());
    m_tabs[widget(index)] = std::move(tab);
    setCurrentIndex(index);
    return true;
}

void GraphicsSceneTabWidget::closeTab(int index)
{
    if (index < 0 || index >= count())
        return;

    auto targetWidget = widget(index);
    removeTab(index);

    m_tabs.erase(targetWidget);
}

void GraphicsSceneTabWidget::onTabCountChanged()
{
    setTabsClosable(count() > 1);
}

void GraphicsSceneTabWidget::setCurrentTabText(const QString &label)
{
    setTabText(currentIndex(), label);
}

void GraphicsSceneTabWidget::tabInserted(int index)
{
    QTabWidget::tabInserted(index);
    emit countChanged();
}

void GraphicsSceneTabWidget::tabRemoved(int index)
{
    QTabWidget::tabRemoved(index);
    emit countChanged();
}

QtNodes::DataFlowGraphicsScene *GraphicsSceneTabWidget::getCurrentScene() const
{
    if (!count())
        return nullptr;
    return m_tabs.at(currentWidget()).getScene();
}