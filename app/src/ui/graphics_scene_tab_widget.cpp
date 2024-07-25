#include "ui/graphics_scene_tab_widget.hpp"

#include <QPushButton>
#include <QTabBar>
#include <QWidget>

#include "data/tab_manager.hpp"

GraphicsSceneTabWidget::GraphicsSceneTabWidget(std::shared_ptr<TabManager> tabManager,
                                               QWidget *parent)
    : QTabWidget(parent)
    , m_tabManager(tabManager)
{
    auto runButton = new QPushButton("Run");
    setCornerWidget(runButton);
    connect(runButton, &QPushButton::clicked, this, &GraphicsSceneTabWidget::runClicked);

    connect(this,
            &GraphicsSceneTabWidget::tabCloseRequested,
            this,
            &GraphicsSceneTabWidget::closeTab);
    connect(this,
            &GraphicsSceneTabWidget::countChanged,
            this,
            &GraphicsSceneTabWidget::onTabCountChanged);
    connect(this,
            &GraphicsSceneTabWidget::currentChanged,
            this,
            &GraphicsSceneTabWidget::onCurrentChanged);
    connect(m_tabManager.get(),
            &TabManager::newTabCreated,
            this,
            &GraphicsSceneTabWidget::onNewTabCreated);
    connect(m_tabManager.get(),
            &TabManager::currentChanged,
            this,
            &GraphicsSceneTabWidget::setCurrentWidget);
    connect(m_tabManager.get(),
            &TabManager::tabFileNameChanged,
            this,
            &GraphicsSceneTabWidget::onTabFileNameChanged);

    // init with 1 blank tab
    m_tabManager->newTab();
}

void GraphicsSceneTabWidget::closeCurrentTab()
{
    closeTab(currentIndex());
}

void GraphicsSceneTabWidget::closeTab(int index)
{
    if (index < 0 || index >= count())
        return;

    auto targetWidget = widget(index);
    removeTab(index);

    m_tabManager->removeTab(targetWidget);
}

void GraphicsSceneTabWidget::onTabCountChanged(int count)
{
    setTabsClosable(count > 1);
}

void GraphicsSceneTabWidget::onCurrentChanged(const int &index)
{
    auto view = widget(index);
    if (m_tabManager->currentWidget() == view)
        return;
    m_tabManager->setCurrentView(view);
}

void GraphicsSceneTabWidget::onNewTabCreated(QWidget *widget, const QString &fileName)
{
    int index = addTab(widget, fileName.isEmpty() ? "blank" : fileName);
    setCurrentIndex(index);
}

void GraphicsSceneTabWidget::onTabFileNameChanged(QWidget *widget, const QString &fileName)
{
    setTabText(indexOf(widget), fileName);
}

void GraphicsSceneTabWidget::tabInserted(int index)
{
    QTabWidget::tabInserted(index);
    emit countChanged(count());
}

void GraphicsSceneTabWidget::tabRemoved(int index)
{
    QTabWidget::tabRemoved(index);
    emit countChanged(count());
}