#pragma once

#include <QtNodes/DirectedAcyclicGraphModel>

class FdfBlockModel;
class DataSourceModel;
class FuncOutModel;

class CustomGraph : public QtNodes::DirectedAcyclicGraphModel
{
    Q_OBJECT
public:
    CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);
    std::vector<DataSourceModel *> getDataSourceModels() const;
    std::vector<FuncOutModel *> getFuncOutModels() const;

signals:
    void dataSourceModelImportClicked(const QtNodes::NodeId nodeId);

protected:
    void initBlockConnections(const QtNodes::NodeId nodeId, FdfBlockModel *block);
    virtual void onNodeCreated(const QtNodes::NodeId nodeId) override;
    virtual void onNodeDeleted(const QtNodes::NodeId nodeId);
    void onOutPortInserted(const QtNodes::NodeId nodeId, const QtNodes::PortIndex index);
    void onOutPortDeleted(const QtNodes::NodeId nodeId, const QtNodes::PortIndex index);
    // rename captions that are duplicates
    void makeCaptionUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *block);
    // rename out ports that are duplicates
    void makeOutPortsUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *block);
    void makeOutPortsUnique(const QtNodes::NodeId &nodeId,
                            FdfBlockModel *block,
                            const QtNodes::PortIndex &index);

    // tracks node captions for uniqueness
    std::unordered_map<QString, QtNodes::NodeId> m_usedNodeCaptions;
    std::unordered_set<QtNodes::NodeId> m_trackedNodes;
    // tracks out port names for uniqueness, index is necessary for uniqueness amongst the node itself
    std::unordered_map<QString, std::pair<QtNodes::NodeId, QtNodes::PortIndex>> m_usedOutPortCaptions;
    std::unordered_set<QtNodes::NodeId> m_dataSourceNodes;
    std::unordered_set<QtNodes::NodeId> m_funcOutNodes;
};
