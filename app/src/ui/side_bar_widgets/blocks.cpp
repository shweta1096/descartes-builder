#include "ui/side_bar_widgets/blocks.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLayout>

#include "data/block_manager.hpp"

Blocks::Blocks(std::shared_ptr<BlockManager> blockManager, QWidget *parent)
    : QWidget(parent)
    , m_blockManager(blockManager)
    , m_nodeId(QtNodes::InvalidNodeId)
    , m_nodeDataLabel(new QLabel())
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(m_nodeDataLabel);
    setLayout(layout);

    updateFields();
    connect(this, &Blocks::nodeIdChanged, this, &Blocks::updateFields);
}

void Blocks::setNodeId(QtNodes::NodeId id)
{
    if (m_nodeId == id)
        return;
    m_nodeId = id;
    emit nodeIdChanged(id);
}

void Blocks::onNodeSelected(QtNodes::NodeId id)
{
    setNodeId(id);
}

void Blocks::updateFields()
{
    auto jsonObject = m_blockManager->getJson(m_nodeId);
    if (jsonObject.isEmpty())
        m_nodeDataLabel->setText("No block selected");
    else
        m_nodeDataLabel->setText(QJsonDocument(jsonObject).toJson(QJsonDocument::Indented));
}