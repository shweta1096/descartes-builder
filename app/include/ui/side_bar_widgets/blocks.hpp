#pragma once

#include <QWidget>

class QLabel;
class QSplitter;
class BlockManager;
class TabManager;

#include <QtNodes/Definitions>

class Blocks : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QtNodes::NodeId nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
public:
    Blocks(std::shared_ptr<BlockManager> blockManager,
           std::shared_ptr<TabManager> tabManager,
           QWidget *parent = nullptr);

    QtNodes::NodeId nodeId() const { return m_nodeId; }
    void setNodeId(QtNodes::NodeId id);

signals:
    void nodeIdChanged(QtNodes::NodeId id);

public slots:
    void onNodeSelected(QtNodes::NodeId id);

private slots:
    void updateFields();

private:
    void initUi();
    void initViewer();
    void initLibrary();

    std::shared_ptr<BlockManager> m_blockManager;
    std::shared_ptr<TabManager> m_tabManager;
    QtNodes::NodeId m_nodeId;

    QSplitter *m_splitter;
    QWidget *m_blockViewer;
    QLabel *m_viewerLabel;
    QWidget *m_library;
};
