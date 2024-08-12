#pragma once

#include <QtNodes/DirectedAcyclicGraphModel>

class FdfBlockModel;
class DataSourceModel;
class FuncOutModel;

class CustomGraph : public QtNodes::DirectedAcyclicGraphModel
{
public:
    CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);
    std::vector<DataSourceModel *> getDataSourceModels() const;
    std::vector<FuncOutModel *> getFuncOutModels() const;

protected:
    virtual void onNodeCreated(const QtNodes::NodeId nodeId) override;
    virtual void onNodeDeleted(const QtNodes::NodeId nodeId);
    // rename captions that are duplicates
    void makeCaptionUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model);
    // rename out ports that are duplicates
    void makeOutPortsUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *model);

    // tracks node captions for uniqueness
    std::unordered_map<QString, QtNodes::NodeId> m_usedNodeCaptions;
    // tracks out port names for uniqueness
    std::unordered_map<QString, QtNodes::NodeId> m_usedOutPortCaptions;
    std::unordered_set<QtNodes::NodeId> m_dataSourceNodes;
    std::unordered_set<QtNodes::NodeId> m_funcOutNodes;
};
