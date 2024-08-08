#include "data/custom_graph.hpp"

#include "ui/models/fdf_block_model.hpp"

CustomGraph::CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
    : DirectedAcyclicGraphModel(registry)
{}

void CustomGraph::onNodeCreated(const QtNodes::NodeId nodeId)
{
    DirectedAcyclicGraphModel::onNodeCreated(nodeId);
    auto model = delegateModel<FdfBlockModel>(nodeId);
    if (!model)
        return;
    makeCaptionUnique(nodeId, model);
    makeOutPortsUnique(nodeId, model);
}

void CustomGraph::makeCaptionUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model)
{
    QString uniqueCaption = model->caption();
    uint counter = 1;
    while (m_usedNodeCaptions.count(uniqueCaption) > 0) {
        uniqueCaption = QString("%1 %2").arg(model->caption(), QString::number(++counter));
    }
    m_usedNodeCaptions.insert({uniqueCaption, nodeId});
    if (counter > 1) // if we need to update the name
        model->setCaption(uniqueCaption);
}

void CustomGraph::makeOutPortsUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model)
{
    auto portType = QtNodes::PortType::Out;
    for (uint i = 0; i < model->nPorts(portType); ++i) {
        const auto ORIGINAL_NAME = model->portCaption(portType, i);
        auto uniqueName = model->portCaption(portType, i);
        uint counter = 1;
        while (m_usedOutPortCaptions.count(uniqueName) > 0) {
            uniqueName = QString("%1 %2").arg(ORIGINAL_NAME, QString::number(++counter));
        }
        m_usedOutPortCaptions.insert({uniqueName, nodeId});
        if (counter > 1) { // if we need to update the name
            model->setPortDefaultCaption(portType, i, uniqueName);
            model->resetPortCaption(portType, i);
        }
    }
}
