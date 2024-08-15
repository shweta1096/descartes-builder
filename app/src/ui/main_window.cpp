#include "ui/main_window.hpp"

#include <QActionGroup>
#include <QApplication>
#include <QDir>
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

#include "data/block_manager.hpp"
#include "data/constants.hpp"
#include "data/tab_manager.hpp"
#include "engine/engine_starter.hpp"
#include "temp.hpp"
#include "ui/graphics_scene_tab_widget.hpp"
#include "ui/log_panel.hpp"
#include "ui/side_bar_widgets/blocks.hpp"

MainWindow::MainWindow()
    : m_engine(EngineStarter::init())
    , m_tabManager(std::make_shared<TabManager>())
    , m_blockManager(std::make_shared<BlockManager>())
    , m_temp(new Temp(this))
{
    initManagers();
    initScene();
    initMenuBar();
    initPrimarySideBar();
    initLogPanel();

    setWindowTitle("DesCartes Builder");
    setGeometry(QApplication::primaryScreen()->availableGeometry());

    qInfo() << "Welcome to DesCartes Builder";
}

MainWindow::~MainWindow()
{
    m_tabManager->clear();
    qInfo() << "Program has finished.";
}

bool MainWindow::callExecute()
{
    return m_engine->execute(m_tabManager->getCurrentTab());
}

void MainWindow::initManagers()
{
    m_blockManager->setTabManager(m_tabManager);
}

void MainWindow::initScene()
{
    ConnectionStyle::setConnectionStyle(constants::CONNECTION_STYLE);
    GraphicsViewStyle::setStyle(constants::GRAPHICS_VIEW_STYLE);
    NodeStyle::setNodeStyle(constants::NODE_STYLE);

    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);
    m_tabManager->setTabParent(m_centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(m_centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_graphicsSceneTabWidget = new GraphicsSceneTabWidget(m_tabManager, m_centralWidget);
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
    fileMenu->addSeparator();
    auto closeAction = fileMenu->addAction("Close current tab");
    auto nextTabAction = fileMenu->addAction("Next tab");
    auto previousTabAction = fileMenu->addAction("Previous tab");
    closeAction->setDisabled(true);
    nextTabAction->setDisabled(true);
    previousTabAction->setDisabled(true);
    fileMenu->addSeparator();
    auto runAction = fileMenu->addAction("Run");

    newAction->setShortcuts({QKeySequence::New, QKeySequence::AddTab});
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    openAction->setShortcut(QKeySequence::Open);
    closeAction->setShortcut(QKeySequence::Close);
    nextTabAction->setShortcut(QKeyCombination(Qt::MetaModifier, Qt::Key_Tab));
    previousTabAction->setShortcut(
        QKeyCombination(Qt::MetaModifier | Qt::ShiftModifier, Qt::Key_Tab));
    runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    connect(newAction, &QAction::triggered, m_tabManager.get(), &TabManager::newTab);
    connect(saveAction, &QAction::triggered, m_tabManager.get(), &TabManager::save);
    connect(saveAsAction, &QAction::triggered, m_tabManager.get(), &TabManager::saveAs);
    connect(openAction, &QAction::triggered, m_tabManager.get(), &TabManager::open);
    connect(closeAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::closeCurrentTab);
    connect(nextTabAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::nextTab);
    connect(previousTabAction,
            &QAction::triggered,
            m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::previousTab);
    connect(m_graphicsSceneTabWidget,
            &GraphicsSceneTabWidget::countChanged,
            fileMenu,
            [closeAction, nextTabAction, previousTabAction](int count) {
                const bool MORE_THAN_ONE = count > 1;
                closeAction->setEnabled(MORE_THAN_ONE);
                nextTabAction->setEnabled(MORE_THAN_ONE);
                previousTabAction->setEnabled(MORE_THAN_ONE);
            });
    connect(runAction, &QAction::triggered, this, &MainWindow::callExecute);

    { // temp menu for testing code
        QMenu *tempMenu = menuBar->addMenu("Temp");
        auto pythonAction = tempMenu->addAction("Run Python");
        auto infoAction = tempMenu->addAction("print info");
        auto debugAction = tempMenu->addAction("print debug");
        auto errorAction = tempMenu->addAction("print error");
        auto openExample = tempMenu->addAction("Open Example");
        connect(pythonAction, &QAction::triggered, m_temp, &Temp::runPython);
        connect(infoAction, &QAction::triggered, m_temp, &Temp::printInfo);
        connect(debugAction, &QAction::triggered, m_temp, &Temp::printDebug);
        connect(errorAction, &QAction::triggered, m_temp, &Temp::printError);
        connect(openExample, &QAction::triggered, m_tabManager.get(), [this]() {
            QDir dir(QApplication::applicationDirPath());
            for (int i = 0; i < 6; ++i) // mac only due to bundle dir
                dir.cdUp();
            dir.cd("examples");
            m_tabManager->openFrom(dir.absoluteFilePath("pipe-deformation.dcb"));
        });
        openExample->setShortcut(
            QKeyCombination(Qt::ControlModifier | Qt::ShiftModifier, Qt::Key_E));
    }
}

void MainWindow::initPrimarySideBar()
{
    // init widgets
    auto blockWidget = new Blocks(m_blockManager, m_tabManager);
    connect(m_blockManager.get(), &BlockManager::nodeSelected, blockWidget, &Blocks::onNodeSelected);

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
        dockWidget->setMinimumWidth(constants::SIDE_BAR_MINIMUM_WIDTH);
        dockWidget->setTitleBarWidget(new QLabel(widgetData.title));
        dockWidget->setWidget(widgetData.widget);
        dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
        dockWidget->hide();

        connect(action, &QAction::toggled, dockWidget, &QDockWidget::setVisible);
    }
    if (auto firstAction = toolBar->actions().first())
        firstAction->trigger();
}

void MainWindow::initLogPanel()
{
    auto dockWidget = new QDockWidget("Log Panel");
    dockWidget->setTitleBarWidget(new QLabel("Log Panel"));
    dockWidget->setWidget(new LogPanel());
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
}
