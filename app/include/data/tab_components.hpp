#pragma once

#include <QFileInfo>
#include <QObject>
#include <QTemporaryDir>

#include <QtNodes/Definitions>

namespace QtNodes {
class DagGraphicsScene;
class GraphicsView;
} // namespace QtNodes

class CustomGraph;

class TabComponents : public QObject
{
    Q_OBJECT
public:
    TabComponents(QWidget *parent, std::optional<QFileInfo> fileInfo = std::nullopt);
    ~TabComponents();
    CustomGraph *getGraph() const { return m_graph; }
    QtNodes::DagGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }
    std::shared_ptr<QTemporaryDir> getTempDir() { return m_dir; }
    QFileInfo getFileInfo() { return m_localFile; }
    void setFileInfo(const QFileInfo &fileInfo) { m_localFile = fileInfo; }

private slots:
    void onDataSourceImportClicked(const QtNodes::NodeId nodeId);

private:
    CustomGraph *m_graph;
    QtNodes::DagGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
    // dir with .dag and imported files
    std::shared_ptr<QTemporaryDir> m_dir;
    // zipped local file to output to
    QFileInfo m_localFile;
};
