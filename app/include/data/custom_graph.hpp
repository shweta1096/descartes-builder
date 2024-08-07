#pragma once

#include <QtNodes/DirectedAcyclicGraphModel>

class CustomGraph : public QtNodes::DirectedAcyclicGraphModel
{
public:
    CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);
    virtual void onNodeCreated(const QtNodes::NodeId nodeId) override;

private:
    // tracks used out port names when we need uniqueness
    std::unordered_map<QString, QtNodes::NodeId> m_usedOutPortCaptions;
};
