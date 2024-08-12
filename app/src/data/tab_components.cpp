#include "data/tab_components.hpp"

#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>

#include <QtNodes/DagGraphicsScene>
#include <QtNodes/GraphicsView>

#include "data/block_manager.hpp"
#include "data/custom_graph.hpp"
#include "ui/models/io_models.hpp"

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
    // Qt bug for MacOS throws warnings when using touch pad with graphics view
    // touch pad seems to trigger touch events, so touch events are disabled to supress the bug
    m_view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    connect(m_scene, &DagGraphicsScene::sceneLoaded, m_view, &GraphicsView::centerScene);
    if (parent)
        QObject::connect(m_scene, &DagGraphicsScene::modified, parent, [parent]() {
            parent->setWindowModified(true);
        });
    connect(m_graph,
            &CustomGraph::dataSourceModelImportClicked,
            this,
            &TabComponents::onDataSourceImportClicked);
}

TabComponents::~TabComponents()
{
    m_view->deleteLater();
    m_scene->deleteLater();
    m_graph->deleteLater();
}

void TabComponents::onDataSourceImportClicked(const QtNodes::NodeId nodeId)
{
    QFileInfo originalFile(
        QFileDialog::getOpenFileName(nullptr,
                                     tr("Import Data Source"),
                                     QStandardPaths::writableLocation(
                                         QStandardPaths::DocumentsLocation),
                                     tr("data (*%1)").arg(DataSourceModel::fileFilter())));
    QFileInfo newFile(m_dir->filePath(originalFile.fileName()));
    // move to temp dir
    QFile::copy(originalFile.absoluteFilePath(), newFile.absoluteFilePath());
    auto dataSource = m_graph->delegateModel<DataSourceModel>(nodeId);
    QFileInfo oldFile(m_dir->filePath(dataSource->file().fileName()));
    if (oldFile.exists())
        QFile::remove(oldFile.absoluteFilePath());
    dataSource->setFile(newFile);
}