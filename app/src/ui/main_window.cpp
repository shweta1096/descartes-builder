#include "ui/main_window.hpp"

#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QLabel>
#include <QActionGroup>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

#include "data/constants.hpp"
#include "ui/model_registry.hpp"
#include "temp.hpp"

#include <QtUtility/media/media.hpp>

MainWindow::MainWindow()
    : m_temp(new Temp(this))
{
    initScene();
    initMenuBar();
    initPrimarySideBar();

    setWindowTitle("DesCartes Builder");
    setGeometry(QApplication::primaryScreen()->availableGeometry());
}

void MainWindow::initScene()
{
    ConnectionStyle::setConnectionStyle(constants::CONNECTION_STYLE);
    std::shared_ptr<NodeDelegateModelRegistry> registry = model_registry::registerDataModels();

    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);

    // this model is referenced by DataFlowGraphicsScene and needs to be kept
    // for the lifetime of mainWidget
    auto dataFlowGraphModel = new DataFlowGraphModel(registry);
    dataFlowGraphModel->setParent(m_centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(m_centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_scene = new DataFlowGraphicsScene(*dataFlowGraphModel, m_centralWidget);
    auto view = new GraphicsView(m_scene);
    layout->addWidget(view);

    QObject::connect(m_scene, &DataFlowGraphicsScene::sceneLoaded, view, &GraphicsView::centerScene);
    QObject::connect(m_scene, &DataFlowGraphicsScene::modified, m_centralWidget, [this]()
                     { m_centralWidget->setWindowModified(true); });
}

void MainWindow::initMenuBar()
{
    auto menuBar = new QMenuBar();
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("File");
    auto saveAction = fileMenu->addAction("Save Scene");
    auto loadAction = fileMenu->addAction("Load Scene");

    QMenu *tempMenu = menuBar->addMenu("Temp");
    auto pythonAction = tempMenu->addAction("Run Python");

    saveAction->setShortcut(QKeySequence::Save);
    loadAction->setShortcut(QKeySequence::Open);

    connect(saveAction, &QAction::triggered, m_scene, [this]()
            {
        if (m_scene->save())
            m_centralWidget->setWindowModified(false); });
    connect(loadAction, &QAction::triggered, m_scene, &DataFlowGraphicsScene::load);
    connect(pythonAction, &QAction::triggered, m_temp, &Temp::runPython);
}

void MainWindow::initPrimarySideBar()
{
    struct SideBarWidgetData
    {
        QIcon icon;
        QString title;
        QWidget *widget;
    };
    std::vector<SideBarWidgetData> widgets = {
        {media::recolor(QIcon(":/blocks.png"), constants::COLOR_SECONDARY),
         "Blocks",
         new QLabel("Block Dock Widget")},
        {media::recolor(QIcon(":/menu.png"), constants::COLOR_SECONDARY),
         "Menu",
         new QLabel("Menu Dock Widget")},
        {media::recolor(QIcon(":/settings.png"), constants::COLOR_SECONDARY),
         "Settings",
         new QLabel("Settings Dock Widget")},
        {media::recolor(QIcon(":/information.png"), constants::COLOR_SECONDARY),
         "Information",
         new QLabel("Information Dock Widget")},
    };

    auto toolBar = new QToolBar("Primary Side Bar");
    toolBar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, toolBar);

    auto primarySideBarGroup = new QActionGroup(this);
    primarySideBarGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    for (auto widgetData : widgets)
    {
        auto action = toolBar->addAction(widgetData.icon, widgetData.title);
        action->setCheckable(true);
        primarySideBarGroup->addAction(action);
        auto dockWidget = new QDockWidget(widgetData.title);
        dockWidget->setTitleBarWidget(new QLabel(widgetData.title));
        dockWidget->setWidget(widgetData.widget);
        dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
        dockWidget->hide();

        connect(action, &QAction::toggled, dockWidget, &QDockWidget::setVisible);
    }
}
