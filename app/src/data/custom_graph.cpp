#include "data/custom_graph.hpp"

#include "ui/models/fdf_block_model.hpp"

namespace {

template<typename MapType>
void removeByValue(MapType &map, const typename MapType::mapped_type &valueToRemove)
{
    for (auto it = map.begin(); it != map.end();) {
        if (it->second == valueToRemove)
            it = map.erase(it);
        else
            ++it;
    }
}

} // namespace

CustomGraph::CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
    : DirectedAcyclicGraphModel(registry)
{
    connect(this, &CustomGraph::nodeDeleted, this, &CustomGraph::onNodeDeleted);
}

void CustomGraph::onNodeCreated(const QtNodes::NodeId nodeId)
{
    DirectedAcyclicGraphModel::onNodeCreated(nodeId);
    auto model = delegateModel<FdfBlockModel>(nodeId);
    if (!model)
        return;
    makeCaptionUnique(nodeId, model);
    makeOutPortsUnique(nodeId, model);
}

void CustomGraph::onNodeDeleted(const QtNodes::NodeId nodeId)
{
    removeByValue(m_usedNodeCaptions, nodeId);
    removeByValue(m_usedOutPortCaptions, nodeId);
}

void CustomGraph::makeCaptionUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model)
{
    QString uniqueCaption = model->caption();
    uint counter = 1;
    while (m_usedNodeCaptions.count(uniqueCaption) > 0
           && m_usedNodeCaptions.at(uniqueCaption) != nodeId) {
        uniqueCaption = QString("%1 %2").arg(model->caption(), QString::number(++counter));
    }
    m_usedNodeCaptions[uniqueCaption] = nodeId;
    m_usedNodeCaptions.insert({uniqueCaption, nodeId});
    if (model->caption() != uniqueCaption)
        model->setCaption(uniqueCaption);
}

void CustomGraph::makeOutPortsUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model)
{
    auto portType = QtNodes::PortType::Out;
    for (uint i = 0; i < model->nPorts(portType); ++i) {
        const auto ORIGINAL_NAME = model->portCaption(portType, i);
        auto uniqueName = model->portCaption(portType, i);
        uint counter = 1;
        while (m_usedOutPortCaptions.count(uniqueName) > 0
               && m_usedOutPortCaptions.at(uniqueName) != nodeId) {
            uniqueName = QString("%1 %2").arg(ORIGINAL_NAME, QString::number(++counter));
        }
        m_usedOutPortCaptions[uniqueName] = nodeId;
        if (ORIGINAL_NAME != uniqueName) {
            model->setPortDefaultCaption(portType, i, uniqueName);
            model->resetPortCaption(portType, i);
        }
    }
}
