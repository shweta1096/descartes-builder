#include "ui/main_window.hpp"

#include <QActionGroup>
#include <QApplication>
#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QScreen>
#include <QToolBar>
#include <QVBoxLayout>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/GraphicsViewStyle>
#include <QtNodes/NodeData>
#include <QtNodes/NodeStyle>

using QtNodes::ConnectionStyle;
using QtNodes::GraphicsViewStyle;
using QtNodes::NodeStyle;

#include <QtUtility/media/media.hpp>

#include "data/constants.hpp"
#include "engine/engine_starter.hpp"
#include "temp.hpp"
#include "ui/graphics_scene_tab_widget.hpp"
#include "ui/log_panel.hpp"
#include "ui/side_bar_widgets/blocks.hpp"

MainWindow::MainWindow()
    : m_engine(EngineStarter::init())
    , m_temp(new Temp(this))
{
    initScene();
    initMenuBar();
    initPrimarySideBar();
    initLogPanel();

    setWindowTitle("DesCartes Builder");
    setGeometry(QApplication::primaryScreen()->availableGeometry());
}

MainWindow::~MainWindow()
{
    qInfo() << "Program has finished.";
}

bool MainWindow::callExecute()
{
    return m_engine->execute(m_graphicsSceneTabWidget->getCurrentGraph());
}

void MainWindow::initScene()
{
    ConnectionStyle::setConnectionStyle(constants::CONNECTION_STYLE);
    GraphicsViewStyle::setStyle(constants::GRAPHICS_VIEW_STYLE);
    NodeStyle::setNodeStyle(constants::NODE_STYLE);

    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(m_centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_graphicsSceneTabWidget = new GraphicsSceneTabWidget(m_centralWidget);
    connect(m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::runClicked,
            this,
            &MainWindow::callExecute);

    layout->addWidget(m_graphicsSceneTabWidget);
}

void MainWindow::initMenuBar()
{
    auto menuBar = new QMenuBar();
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("File");
    auto newAction = fileMenu->addAction("New");
    auto saveAction = fileMenu->addAction("Save");
    auto saveAsAction = fileMenu->addAction("Save As...");
    auto openAction = fileMenu->addAction("Open");
    auto closeAction = fileMenu->addAction("Close current tab");
    closeAction->setDisabled(true);
    auto runAction = fileMenu->addAction("Run");

    newAction->setShortcuts({QKeySequence::New, QKeySequence::AddTab});
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    openAction->setShortcut(QKeySequence::Open);
    closeAction->setShortcut(QKeySequence::Close);
    runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    connect(newAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::newTab);
    connect(saveAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::save);
    connect(saveAsAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::saveAs);
    connect(openAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::open);
    connect(closeAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::closeCurrentTab);
    if (m_graphicsSceneTabWidget)
        connect(m_graphicsSceneTabWidget,
                &GraphicsSceneTabWidget::countChanged,
                closeAction,
                [closeAction](int count) { closeAction->setEnabled(count > 1); });
    connect(runAction, &QAction::triggered, this, &MainWindow::callExecute);

    { // temp menu for testing code
        QMenu *tempMenu = menuBar->addMenu("Temp");
        auto pythonAction = tempMenu->addAction("Run Python");
        auto infoAction = tempMenu->addAction("print info");
        auto debugAction = tempMenu->addAction("print debug");
        auto errorAction = tempMenu->addAction("print error");
        connect(pythonAction, &QAction::triggered, m_temp, &Temp::runPython);
        connect(infoAction, &QAction::triggered, m_temp, &Temp::printInfo);
        connect(debugAction, &QAction::triggered, m_temp, &Temp::printDebug);
        connect(errorAction, &QAction::triggered, m_temp, &Temp::printError);
    }
}

void MainWindow::initPrimarySideBar()
{
    // init widgets
    auto blockWidget = new Blocks();
    connect(m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::nodeSelected,
            blockWidget,
            &Blocks::onNodeSelected);

    // prevent log panel from taking the corner
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    struct SideBarWidgetData
    {
        QIcon icon;
        QString title;
        QWidget *widget;
    };
    std::vector<SideBarWidgetData> widgets = {
        {QtUtility::media::recolor(QIcon(":/blocks.png"), constants::COLOR_SECONDARY),
         "Blocks",
         blockWidget},
        {QtUtility::media::recolor(QIcon(":/menu.png"), constants::COLOR_SECONDARY),
         "Menu",
         new QLabel("Menu Dock Widget")},
        {QtUtility::media::recolor(QIcon(":/settings.png"), constants::COLOR_SECONDARY),
         "Settings",
         new QLabel("Settings Dock Widget")},
        {QtUtility::media::recolor(QIcon(":/information.png"), constants::COLOR_SECONDARY),
         "Information",
         new QLabel("Information Dock Widget")},
    };

    auto toolBar = new QToolBar("Primary Side Bar");
    toolBar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, toolBar);

    auto primarySideBarGroup = new QActionGroup(this);
    primarySideBarGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    for (auto widgetData : widgets) {
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

void MainWindow::initLogPanel()
{
    auto dockWidget = new QDockWidget("Log Panel");
    dockWidget->setTitleBarWidget(new QLabel("Log Panel"));
    dockWidget->setWidget(new LogPanel());
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
}
