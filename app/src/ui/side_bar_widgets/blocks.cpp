#include "ui/side_bar_widgets/blocks.hpp"

#include <QLabel>
#include <QLayout>

Blocks::Blocks(QWidget *parent)
    : QWidget(parent)
    , m_nodeId(QtNodes::InvalidNodeId)
    , m_nodeIdLabel(new QLabel())
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(m_nodeIdLabel);
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
    if (m_nodeId == QtNodes::InvalidNodeId)
        m_nodeIdLabel->setText("Id:\t-");
    else
        m_nodeIdLabel->setText("Id:\t" + QString::number(m_nodeId));
}