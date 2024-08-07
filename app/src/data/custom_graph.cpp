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
    // rename out ports that are duplicates
    auto portType = QtNodes::PortType::Out;
    for (uint i = 0; i < model->nPorts(portType); ++i) {
        auto originalName = model->portCaption(portType, i);
        if (m_usedOutPortCaptions.count(originalName) < 1) {
            m_usedOutPortCaptions.insert({originalName, nodeId});
            continue;
        }
        int counter = 1;
        auto newName = QString("%1 %2").arg(originalName, QString::number(++counter));
        while (m_usedOutPortCaptions.count(newName) > 0) {
            newName = QString("%1 %2").arg(originalName, QString::number(++counter));
        }
        model->setPortDefaultCaption(portType, i, newName);
        model->resetPortCaption(portType, i);
        m_usedOutPortCaptions.insert({newName, nodeId});
    }
}
