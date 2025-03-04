#include "ui/models/uid_manager.hpp"
#include "ui/models/fdf_block_model.hpp"
#include "ui/models/nodes.hpp"
#include <QDebug>

const FdfUID UIDManager::NONE_ID = -1;
const QString UIDManager::NONE_TAG = "type_none";
const bool UIDManager::USE_XFORM_MAP = true;
const bool UIDManager::USE_REDUCE_MAP = false;

UIDManager::UIDManager() {}

FdfUID UIDManager::createUID()
{
    FdfUID uid = 0;
    if (!uidToTag.empty()) {
        // find biggest uid in the map, and increment 1 to create new uid
        uid = uidToTag.rbegin()->first + 1;
    }
    QString tag = QString("type_%1").arg(uid);
    updateMap(uid, tag);
    return uid;
}

FdfUID UIDManager::getUid(const QString &tag) const
{
    auto elem = tagToUid.find(tag);
    if (elem != tagToUid.end())
        return elem->second;
    return NONE_ID;
}

QString UIDManager::getTag(const FdfUID &uid) const
{
    auto elem = uidToTag.find(uid);
    if (elem != uidToTag.end())
        return elem->second;
    return NONE_TAG;
}

void UIDManager::updateMap(FdfUID &uid, QString &tag)
{
    if (uid == NONE_ID || tag == NONE_TAG)
        return;
    tag.replace(" ", "");
    if (uidToTag.find(uid) == uidToTag.end()) {
        // UID is created afresh, triggered from createUid()
        uidToTag[uid] = tag;
        tagToUid[tag] = uid;
    } else {
        if (tagToUid.find(tag) == tagToUid.end()) // user changes the type tag on UI for readability
        {
            tagToUid.erase(getTag(uid)); // remove the current entry from tagToId
            // Update both maps
            tagToUid[tag] = uid;
            uidToTag[uid] = tag;
        } else // user chooses to override a type mismatch, setting a type to another existent type
        {
            FdfUID removeId = std::max(uid, getUid(tag));
            FdfUID keepId = std::min(uid, getUid(tag));
            // update the graph to replace all instances of removeId with keepId
            overrideType(removeId, keepId);
            tagToUid.erase(getTag(removeId));
            uidToTag.erase(removeId);
        }
    }
    // displayMaps();
}

void UIDManager::displayMaps() const
{
    qInfo() << "UID to Tag Map:";
    for (const auto &pair : uidToTag) {
        qInfo() << "UID:" << pair.first << "-> Tag:" << pair.second;
    }

    qInfo() << "Tag to UID Map:";
    for (const auto &pair : tagToUid) {
        qInfo() << "Tag:" << pair.first << "-> UID:" << pair.second;
    }
}

void UIDManager::overrideType(FdfUID removeType, FdfUID keepType)
{
    if (!graph) {
        qWarning() << "UID Manager does not have an associated graph!";
        return;
    }
    // Step 1: Update types in the graph
    for (const auto &id : graph->allNodeIds()) {
        auto block = graph->delegateModel<FdfBlockModel>(id);
        if (!block)
            continue;

        bool updated = false;

        for (int i = 0; i < block->nPorts(PortType::Out); i++) {
            auto dataPort = std::dynamic_pointer_cast<DataNode>(block->outData(i));
            if (dataPort && dataPort->typeId() == removeType) {
                dataPort->setTypeId(keepType);
                updated = true;
                continue;
            }

            auto functionPort = std::dynamic_pointer_cast<FunctionNode>(block->outData(i));
            if (!functionPort)
                continue;

            FunctionNode::Signature signature = functionPort->signature();

            // Replace type in input and output signatures
            updated = replaceTypesInUIDVector(signature.inputs, keepType, removeType);
            updated = replaceTypesInUIDVector(signature.outputs, keepType, removeType);

            if (updated)
                functionPort->setSignature(signature);
        }

        if (updated)
            block->propagateUpdate();
    }
    // Step 2: Update types in the transform and reduce typemaps
    replaceTypesInCoderMap(keepType, removeType, USE_XFORM_MAP);  // for transform map
    replaceTypesInCoderMap(keepType, removeType, USE_REDUCE_MAP); // for reduce map
}

void UIDManager::replaceTypesInCoderMap(FdfUID keepType, FdfUID removeType, bool useTransformMap)
{
    // Update the reduce/xform maps to reflect any type overides
    // A new map (updatedMap) is created, and the current map is overwritten at the end
    std::map<std::vector<FdfUID>, FdfUID> updatedMap;

    auto &currentMap = useTransformMap ? transform_typeIdMap : reduce_typeIdMap;
    for (auto const &elem : currentMap) {
        std::vector<FdfUID> newInputTypeIds = elem.first;
        FdfUID newOutputType = elem.second;

        // If an transformed/reduced type was overridden
        if (elem.second == removeType)
            newOutputType = keepType;
        // If any of the input types of reduce/transform were overridden
        else if (std::find(elem.first.begin(), elem.first.end(), removeType) != elem.first.end()) {
            replaceTypesInUIDVector(newInputTypeIds, keepType, removeType);
        }
        // Assign the new vector and id to the updatedMap
        updatedMap[newInputTypeIds] = newOutputType;
    }
    currentMap = updatedMap;
}

bool UIDManager::replaceTypesInUIDVector(std::vector<FdfUID> &vec,
                                         FdfUID keepType,
                                         FdfUID removeType)
{
    bool updated = false;
    for (FdfUID &item : vec)
        if (item == removeType) {
            item = keepType;
            updated = true;
        }
    return updated;
}

FdfUID UIDManager::createOrFetchTransformUid(std::vector<FdfUID> inputTypeIds)
{
    return createOrFetchCoderUid(inputTypeIds, USE_XFORM_MAP);
}

FdfUID UIDManager::createOrFetchReduceUid(std::vector<FdfUID> inputTypeIds)
{
    return createOrFetchCoderUid(inputTypeIds, USE_REDUCE_MAP);
}

FdfUID UIDManager::createOrFetchCoderUid(std::vector<FdfUID> inputTypeIds, bool useTransformMap)
{
    auto &coderMap = useTransformMap ? transform_typeIdMap : reduce_typeIdMap;
    if (coderMap.count(inputTypeIds) > 0) {
        return coderMap.at(inputTypeIds);
    } else {
        FdfUID outputType = createUID();
        coderMap[inputTypeIds] = outputType;
        return outputType;
    }
}