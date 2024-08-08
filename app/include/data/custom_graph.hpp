#pragma once

#include <QtNodes/DirectedAcyclicGraphModel>

class FdfBlockModel;

class CustomGraph : public QtNodes::DirectedAcyclicGraphModel
{
public:
    CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);
    virtual void onNodeCreated(const QtNodes::NodeId nodeId) override;

private:
    // rename captions that are duplicates
    void makeCaptionUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model);
    // rename out ports that are duplicates
    void makeOutPortsUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model);

    // tracks node captions for uniqueness
    std::unordered_map<QString, QtNodes::NodeId> m_usedNodeCaptions;
    // tracks out port names for uniqueness
    std::unordered_map<QString, QtNodes::NodeId> m_usedOutPortCaptions;
};
