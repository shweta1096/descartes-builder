#pragma once

#include <map>
#include <QString>
#include <QtNodes/Definitions>
#include <QtNodes/DirectedAcyclicGraphModel>

using QtNodes::NodeId;
using QtNodes::PortIndex;

// UID type for data nodes
using FdfUID = int;

// UID type struct for function nodes
struct Signature
{
    std::vector<FdfUID> inputs;
    std::vector<FdfUID> outputs;
    std::pair<size_t, size_t> size() const { return {inputs.size(), outputs.size()}; }
    void update(unsigned int port, FdfUID typeId)
    {
        if (port < inputs.size())
            inputs.at(port) = typeId;
        else
            outputs.at(port - inputs.size()) = typeId;
    }
    void inverse() { std::swap(inputs, outputs); }
    bool isEmpty() const { return inputs.empty() && outputs.empty(); }
    void print() const { qDebug() << inputs << " => " << outputs; }
};

// struct to maintain connection information, to pass to fdf blocks
struct ConnectionInfo
{
    FdfUID expectedInType;
    FdfUID receivedOutType;
    std::shared_ptr<Signature> receivedSignature;
    PortIndex inIndex;
    PortIndex outIndex;
    NodeId inNodeId;
    NodeId outNodeId;
    QString inBlockCaption;
    QString outBlockCaption;

    ConnectionInfo(FdfUID expectedIn = -1,
                   FdfUID receivedOut = -1,
                   std::shared_ptr<Signature> signature = nullptr,
                   PortIndex inIdx = -1,
                   PortIndex outIdx = -1,
                   NodeId inNode = -1,
                   NodeId outNode = -1,
                   QString inCaption = QString(),
                   QString outCaption = QString())
        : expectedInType(expectedIn)
        , receivedOutType(receivedOut)
        , receivedSignature(signature)
        , inIndex(inIdx)
        , outIndex(outIdx)
        , inNodeId(inNode)
        , outNodeId(outNode)
        , inBlockCaption(inCaption)
        , outBlockCaption(outCaption)
    {}
};

class CustomGraph;
class UIDManager
{
public:
    static const FdfUID NONE_ID;
    static const QString NONE_TAG;
    UIDManager();
    void setGraph(CustomGraph *tab_graph)
    {
        if (tab_graph)
            graph = tab_graph;
    }
    FdfUID createUID();
    FdfUID getUid(const QString &tag) const;
    QString getTag(const FdfUID &uid) const;
    void updateMap(FdfUID &uid, QString &tag);
    FdfUID createOrFetchTransformUid(std::vector<FdfUID> inputTypeIds);
    FdfUID createOrFetchReduceUid(std::vector<FdfUID> inputTypeIds);
    void getConnectionInfo(QtNodes::ConnectionId const connectionId, ConnectionInfo &connInfo) const;

private:
    CustomGraph *graph;
    // The maps from type id to human-readable tag (one-to-one, both are unique)
    std::map<FdfUID, QString> uidToTag = {{NONE_ID, NONE_TAG}};
    std::map<QString, FdfUID> tagToUid = {{NONE_TAG, NONE_ID}};
    // Helper functions to override and display the map
    void overrideType(FdfUID removeType, FdfUID keepType);
    bool replaceTypesInUIDVector(std::vector<FdfUID> &vec, FdfUID keepType, FdfUID removeType);
    void displayMaps() const;
    // The following maps and functions to handle coder models generated types:
    std::map<std::vector<FdfUID>, FdfUID> transform_typeIdMap;
    std::map<std::vector<FdfUID>, FdfUID> reduce_typeIdMap;
    FdfUID createOrFetchCoderUid(std::vector<FdfUID> inputTypeIds,
                                 std::map<std::vector<FdfUID>, FdfUID> &coderMap);
    void replaceTypesInCoderMap(FdfUID keepType, FdfUID removeType);
};