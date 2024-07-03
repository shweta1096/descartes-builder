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
    initConnectionStyle();
    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    auto mainWidget = new QWidget();
    setCentralWidget(mainWidget);

    auto menuBar = new QMenuBar();
    QMenu *menu = menuBar->addMenu("File");

    auto saveAction = menu->addAction("Save Scene");
    saveAction->setShortcut(QKeySequence::Save);

    auto loadAction = menu->addAction("Load Scene");
    loadAction->setShortcut(QKeySequence::Open);

    QVBoxLayout *l = new QVBoxLayout(mainWidget);

    // this model is referenced by DataFlowGraphicsScene and needs to be kept
    // for the lifetime of mainWidget
    auto dataFlowGraphModel = new DataFlowGraphModel(registry);
    dataFlowGraphModel->setParent(mainWidget);

    l->addWidget(menuBar);
    auto scene = new DataFlowGraphicsScene(*dataFlowGraphModel, mainWidget);

    auto view = new GraphicsView(scene);
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    QObject::connect(saveAction, &QAction::triggered, scene, [scene, mainWidget]()
                     {
        if (scene->save())
            mainWidget->setWindowModified(false); });

    QObject::connect(loadAction, &QAction::triggered, scene, &DataFlowGraphicsScene::load);

    QObject::connect(scene, &DataFlowGraphicsScene::sceneLoaded, view, &GraphicsView::centerScene);

    QObject::connect(scene, &DataFlowGraphicsScene::modified, mainWidget, [mainWidget]()
                     { mainWidget->setWindowModified(true); });

    setWindowTitle("[*]Data Flow: simplest calculator");
    resize(800, 600);
    move(QApplication::primaryScreen()->availableGeometry().center() - rect().center());
}

MainWindow::~MainWindow()
{
}
