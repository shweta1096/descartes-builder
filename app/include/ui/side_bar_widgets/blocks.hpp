#pragma once

#include <QWidget>

class QLabel;
class BlockManager;

#include <QtNodes/Definitions>

class Blocks : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QtNodes::NodeId nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
public:
    Blocks(std::shared_ptr<BlockManager> blockManager, QWidget *parent = nullptr);

    QtNodes::NodeId nodeId() const { return m_nodeId; }
    void setNodeId(QtNodes::NodeId id);

signals:
    void nodeIdChanged(QtNodes::NodeId id);

public slots:
    void onNodeSelected(QtNodes::NodeId id);

private slots:
    void updateFields();

private:
    std::shared_ptr<BlockManager> m_blockManager;
    QtNodes::NodeId m_nodeId;

    QLabel *m_nodeDataLabel;
};
