#include "data/custom_graph.hpp"

#include "ui/models/fdf_block_model.hpp"
#include "ui/models/io_models.hpp"
#include "ui/models/processor_models.hpp"
#include <QAbstractButton>
#include <QMessageBox>
#include <QMetaObject>

namespace {

template<typename MapType>
void removeByValue(MapType &map, const typename MapType::mapped_type &valueToRemove)
{
    for (auto it = map.begin(); it != map.end();)
        if (it->second == valueToRemove)
            it = map.erase(it);
        else
            ++it;
}

template<typename MapType>
void removeByPairFirst(MapType &map,
                       const typename MapType::mapped_type::first_type &firstValueToRemove)
{
    for (auto it = map.begin(); it != map.end();)
        if (it->second.first == firstValueToRemove)
            it = map.erase(it);
        else
            ++it;
}

} // namespace

CustomGraph::CustomGraph(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
    : DirectedAcyclicGraphModel(registry)
{
    connect(this, &CustomGraph::nodeDeleted, this, &CustomGraph::onNodeDeleted);
}

std::vector<DataSourceModel *> CustomGraph::getDataSourceModels() const
{
    std::vector<DataSourceModel *> result;
    for (auto &id : m_dataSourceNodes)
        result.push_back(delegateModel<DataSourceModel>(id));
    return result;
}

std::vector<FuncOutModel *> CustomGraph::getFuncOutModels() const
{
    std::vector<FuncOutModel *> result;
    for (auto &id : m_funcOutNodes)
        result.push_back(delegateModel<FuncOutModel>(id));
    return result;
}

void CustomGraph::showWarning(QtNodes::ConnectionId connectionId)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("Invalid Connection");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.button(QMessageBox::Ignore)->setText("Override");
    QString inBlockCap = "in blk", outBlockCap = "out blk", inPortName = "in port",
            outPortName = "out port";
    if (auto inBlock = delegateModel<FdfBlockModel>(getNodeId(PortType::In, connectionId))) {
        if (auto dataIn = std::dynamic_pointer_cast<NamedNode>(
                inBlock->outData(getPortIndex(PortType::In, connectionId)))) {
            inPortName = dataIn->name();
        }
        inBlockCap = inBlock->caption();
    }
    if (auto outBlock = delegateModel<FdfBlockModel>(getNodeId(PortType::Out, connectionId))) {
        if (auto dataOut = std::dynamic_pointer_cast<NamedNode>(
                outBlock->outData(getPortIndex(PortType::Out, connectionId)))) {
            outPortName = dataOut->name();
        }
        outBlockCap = outBlock->caption();
    }
    msgBox.setInformativeText(QString("Cannot connect %1:%2 to %3:%4 ")
                                  .arg(outBlockCap)
                                  .arg(outPortName)
                                  .arg(inBlockCap)
                                  .arg(inPortName));
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ignore) {
        // TODO implement backpropagation
    }
}

void CustomGraph::warnInvalidConnection(QtNodes::ConnectionId const connectionId) const
{
    QMetaObject::invokeMethod(const_cast<CustomGraph *>(this),
                              "showWarning",
                              Qt::QueuedConnection,
                              Q_ARG(QtNodes::ConnectionId, connectionId));
}

bool CustomGraph::connectionPossible(QtNodes::ConnectionId const connectionId) const
{
    if (auto block = delegateModel<FdfBlockModel>(getNodeId(PortType::In, connectionId)))
        if (auto processorBlock = dynamic_cast<ExternalProcessorModel *>(block)) {
            QUuid outTypeId;
            auto outBlock = delegateModel<FdfBlockModel>(getNodeId(PortType::Out, connectionId));
            if (auto data = std::dynamic_pointer_cast<DataNode>(
                    outBlock->outData(getPortIndex(PortType::Out, connectionId))))
                outTypeId = data->typeId();
            auto result = processorBlock->canConnect(PortType::In,
                                                     getPortIndex(PortType::In, connectionId),
                                                     outTypeId);
            if (!result) {
                warnInvalidConnection(connectionId);
                return false;
            }
        }
    return DirectedAcyclicGraphModel::connectionPossible(connectionId);
}

void CustomGraph::initBlockConnections(const QtNodes::NodeId nodeId, FdfBlockModel *block)
{
    connect(block, &FdfBlockModel::captionUpdated, this, [this, block, nodeId]() {
        makeCaptionUnique(nodeId, block);
    });
    connect(block,
            &FdfBlockModel::outPortCaptionUpdated,
            this,
            [this, block, nodeId](const PortIndex &index) {
                makeOutPortsUnique(nodeId, block, index);
            });
    connect(block, &FdfBlockModel::outPortInserted, this, [nodeId, this](const PortIndex index) {
        onOutPortInserted(nodeId, index);
    });
    connect(block, &FdfBlockModel::outPortDeleted, this, [nodeId, this](const PortIndex index) {
        onOutPortDeleted(nodeId, index);
    });
}

void CustomGraph::onNodeCreated(const QtNodes::NodeId nodeId)
{
    DirectedAcyclicGraphModel::onNodeCreated(nodeId);
    auto block = delegateModel<FdfBlockModel>(nodeId);
    if (!block)
        return;
    initBlockConnections(nodeId, block);
    makeCaptionUnique(nodeId, block);
    makeOutPortsUnique(nodeId, block);

    if (block->name() == io_names::DATA_SOURCE) {
        m_dataSourceNodes.insert(nodeId);
        auto dataSourceModel = dynamic_cast<DataSourceModel *>(block);
        connect(dataSourceModel, &DataSourceModel::importClicked, this, [nodeId, this]() {
            emit dataSourceModelImportClicked(nodeId);
        });
    } else if (block->name() == io_names::FUNC_OUT)
        m_funcOutNodes.insert(nodeId);
}

void CustomGraph::onNodeDeleted(const QtNodes::NodeId nodeId)
{
    removeByValue(m_usedNodeCaptions, nodeId);
    removeByPairFirst(m_usedOutPortCaptions, nodeId);

    auto block = delegateModel<FdfBlockModel>(nodeId);
    if (!block)
        return;
    if (block->name() == io_names::DATA_SOURCE)
        m_dataSourceNodes.erase(nodeId);
    else if (block->name() == io_names::FUNC_OUT)
        m_funcOutNodes.erase(nodeId);
    m_trackedNodes.erase(nodeId);
}

void CustomGraph::onOutPortInserted(const QtNodes::NodeId nodeId, const QtNodes::PortIndex oldIndex)
{
    auto block = delegateModel<FdfBlockModel>(nodeId);
    if (!block)
        return;
    makeOutPortsUnique(nodeId, block, oldIndex);
}

void CustomGraph::onOutPortDeleted(const QtNodes::NodeId nodeId, const QtNodes::PortIndex oldIndex)
{
    removeByValue(m_usedOutPortCaptions, std::make_pair(nodeId, oldIndex));
    auto block = delegateModel<FdfBlockModel>(nodeId);
    if (!block)
        return;
    if (block->nPorts(PortType::Out) > oldIndex)
        // shift index of ports after the deleted one
        for (int i = oldIndex; i < block->nPorts(PortType::Out); ++i) {
            auto caption = block->portCaption(PortType::Out, i);
            --m_usedOutPortCaptions.at(caption).second;
        }
}

void CustomGraph::makeCaptionUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *block)
{
    QString uniqueCaption = block->caption();
    uint counter = 1;
    if (m_trackedNodes.count(nodeId) > 0) { // if node is already tracked
        if (m_usedNodeCaptions.count(uniqueCaption) > 0
            && m_usedNodeCaptions.at(uniqueCaption) == nodeId)
            return;
        // if caption is different, remove old caption
        removeByValue(m_usedNodeCaptions, nodeId);
    }
    while (m_usedNodeCaptions.count(uniqueCaption) > 0)
        uniqueCaption = QString("%1 %2").arg(block->caption(), QString::number(++counter));
    m_usedNodeCaptions[uniqueCaption] = nodeId;
    m_trackedNodes.insert(nodeId);
    if (block->caption() != uniqueCaption)
        block->setCaption(uniqueCaption);
}

void CustomGraph::makeOutPortsUnique(const QtNodes::NodeId &nodeId, FdfBlockModel *block)
{
    auto portType = QtNodes::PortType::Out;
    for (uint i = 0; i < block->nPorts(portType); ++i) {
        makeOutPortsUnique(nodeId, block, i);
    }
}

void CustomGraph::makeOutPortsUnique(const QtNodes::NodeId &nodeId,
                                     FdfBlockModel *block,
                                     const PortIndex &index)
{
    auto portType = QtNodes::PortType::Out;
    const auto ORIGINAL_NAME = block->defaultPortCaption(portType, index);
    uint counter = 1;

    if (m_trackedNodes.count(nodeId) > 0) {
        if (m_usedOutPortCaptions.count(ORIGINAL_NAME) > 0
            && m_usedOutPortCaptions.at(ORIGINAL_NAME) == std::make_pair(nodeId, index))
            return;
        // if caption is different, remove old caption
        removeByValue(m_usedOutPortCaptions, std::make_pair(nodeId, index));
    }
    auto uniqueName = ORIGINAL_NAME;
    while (m_usedOutPortCaptions.count(uniqueName) > 0) {
        uniqueName = QString("%1 %2").arg(ORIGINAL_NAME, QString::number(++counter));
    }
    m_usedOutPortCaptions[uniqueName] = std::make_pair(nodeId, index);
    if (ORIGINAL_NAME != uniqueName) {
        block->setPortDefaultCaption(portType, index, uniqueName);
        block->resetPortCaption(portType, index);
    }
}
