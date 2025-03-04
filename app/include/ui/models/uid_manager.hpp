#pragma once

#include "data/custom_graph.hpp"
#include <map>
#include <QString>

typedef int FdfUID;
class UIDManager
{
public:
    static const FdfUID NONE_ID;
    static const QString NONE_TAG;
    static const bool USE_XFORM_MAP;
    static const bool USE_REDUCE_MAP;
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

private:
    CustomGraph *graph;
    // The maps from type id to human-readable tag (one-to-one, both are unique)
    std::map<FdfUID, QString> uidToTag = {{NONE_ID, NONE_TAG}};
    std::map<QString, FdfUID> tagToUid = {{NONE_TAG, NONE_ID}};
    // Helper functions to override and display the map
    void overrideType(FdfUID removeType, FdfUID keepType);
    bool replaceTypesInUIDVector(std::vector<FdfUID> &vec, FdfUID keepType, FdfUID removeType);
    void displayMaps() const;
    // The following maps to keep track of coder model generated types:
    std::map<std::vector<FdfUID>, FdfUID> transform_typeIdMap;
    std::map<std::vector<FdfUID>, FdfUID> reduce_typeIdMap;
    FdfUID createOrFetchCoderUid(std::vector<FdfUID> inputTypeIds, bool useTransformMap);
    void replaceTypesInCoderMap(FdfUID keepType, FdfUID removeType, bool useTransformMap);
};