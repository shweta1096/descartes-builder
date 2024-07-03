#include "main_window.hpp"

#include <QScreen>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QScreen>
#include <QApplication>

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

#include "ui/models/AdditionModel.hpp"
#include "ui/models/DivisionModel.hpp"
#include "ui/models/MultiplicationModel.hpp"
#include "ui/models/NumberDisplayDataModel.hpp"
#include "ui/models/NumberSourceDataModel.hpp"
#include "ui/models/SubtractionModel.hpp"

namespace
{
    static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
    {
        auto ret = std::make_shared<NodeDelegateModelRegistry>();
        ret->registerModel<NumberSourceDataModel>("Sources");

        ret->registerModel<NumberDisplayDataModel>("Displays");

        ret->registerModel<AdditionModel>("Operators");

        ret->registerModel<SubtractionModel>("Operators");

        ret->registerModel<MultiplicationModel>("Operators");

        ret->registerModel<DivisionModel>("Operators");

        return ret;
    }

    static void initConnectionStyle()
    {
        ConnectionStyle::setConnectionStyle(
            R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
    }
}

MainWindow::MainWindow()
{
    initScene();
    initMenu();

    setWindowTitle("DesCartes Builder");
    setGeometry(QApplication::primaryScreen()->availableGeometry());
}

MainWindow::~MainWindow()
{
}

void MainWindow::initScene()
{
    initConnectionStyle();
    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

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

void MainWindow::initMenu()
{
    auto menuBar = new QMenuBar();
    setMenuBar(menuBar);
    QMenu *menu = menuBar->addMenu("File");

    auto saveAction = menu->addAction("Save Scene");
    saveAction->setShortcut(QKeySequence::Save);

    auto loadAction = menu->addAction("Load Scene");
    loadAction->setShortcut(QKeySequence::Open);

    QObject::connect(saveAction, &QAction::triggered, m_scene, [this]()
                     {
        if (m_scene->save())
            m_centralWidget->setWindowModified(false); });
    QObject::connect(loadAction, &QAction::triggered, m_scene, &DataFlowGraphicsScene::load);
}
