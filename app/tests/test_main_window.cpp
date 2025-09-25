#include "data/constants.hpp"
#include "data/custom_graph.hpp"
#include "data/tab_manager.hpp"
#include "ui/main_window.hpp"
#include "ui/models/fdf_block_model.hpp"
#include "ui/models/io_models.hpp"
#include "ui/models/uid_manager.hpp"
#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>

class MainWindowTest : public ::testing::Test
{
protected:
    void SetUp() override { mainWindow = new MainWindow(); }

    void TearDown() override { delete mainWindow; }

    MainWindow *mainWindow = nullptr;
};

TEST_F(MainWindowTest, WindowIsCreated)
{
    ASSERT_NE(mainWindow, nullptr) << "MainWindow should be created.";
}

TEST_F(MainWindowTest, TypePropagation)
{
    QFileInfo testFileInfo(__FILE__);
    QString baseDir = testFileInfo.absolutePath();
    QDir dcbDir(baseDir + "/../../examples/tests");
    QString dcbPath = dcbDir.absoluteFilePath("test_propagation.dcb");

    ASSERT_TRUE(mainWindow->openDCB(dcbPath))
        << "openDCB failed for path: " << dcbPath.toStdString();
    CustomGraph *graph = mainWindow->getTabManager()->getCurrentTab()->getGraph();
    ASSERT_TRUE(graph != nullptr) << "Graph should not be null after opening DCB.";

    auto dataSourceModels = graph->getDataSourceModels();
    ASSERT_TRUE(dataSourceModels.size() == 1)
        << "Expected 1 data source model, but found: " << dataSourceModels.size();

    auto dataSourceBlock = dataSourceModels[0];
    dataSourceBlock->setPortTagAndAnnotation(PortType::Out, 0, "upd_tag", "");

    auto targetBlock = graph->getBlockByCaption("process_2");
    ASSERT_TRUE(targetBlock != nullptr) << "Target model should not be null.";

    auto dataSourcePort = dataSourceBlock->allOutData<DataNode>()[0];
    auto targetPort = targetBlock->allOutData<DataNode>()[0];
    ASSERT_TRUE(targetPort->typeTagName() == "upd_tag"
                && targetPort->typeId() == dataSourcePort->typeId())
        << "Type tag and ID should match after propagation.";

    ASSERT_EQ(targetPort->typeTagName(), "upd_tag") << "Type tag should match after propagation.";
    ASSERT_EQ(targetPort->typeId(), dataSourcePort->typeId())
        << "Type ID should match after propagation.";
}