#include "engine/kedro.hpp"

#include <QtNodes/DirectedAcyclicGraphModel>

#include "ui/models/fdf_block_model.hpp"

#include <iostream>

using QtNodes::DirectedAcyclicGraphModel;

namespace
{
    using FdfType = FdfBlockModel::FdfType;
    const std::unordered_set<FdfType> EXCLUDED_TYPES = {FdfType::Data, FdfType::Output};
}

namespace
{
    QStringList getPortList(const FdfBlockModel &block, const PortType &type)
    {
        QStringList result;
        for (uint i = 0; i < block.nPorts(type); ++i)
            if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(block.portData(type, i)))
                result.append(QString("\"%1\"").arg(namedNode->name()));
        return result;
    }

    QString toString(const FdfBlockModel &block)
    {
        QString result = block.typeAsString() + '(';
        if (!block.functionName().isEmpty())
            result += QString("func=%1,").arg(block.functionName());
        result += QString("name=\"%1\",").arg(block.name());
        QStringList inputs = getPortList(block, PortType::In);
        if (inputs.size() == 1)
            result += QString("inputs=%1").arg(inputs.at(0));
        else if (inputs.size() > 1)
            result += QString("inputs=[%1]").arg(inputs.join(','));
        QStringList outputs = getPortList(block, PortType::Out);
        if (outputs.size() == 1)
            result += QString("outputs=%1").arg(outputs.at(0));
        else if (outputs.size() > 1)
            result += QString("outputs=[%1]").arg(outputs.join(','));
        result += ')';
        return result;
    }
}

Kedro::Kedro()
{
    qDebug() << "Kedro engine is initiated";
}

bool Kedro::execute(QtNodes::DirectedAcyclicGraphModel *graph)
{
    qInfo() << "Running...";
    if (!validityCheck(graph))
        return false;
    QStringList serializedObjects;
    for (const auto &id : graph->topologicalOrder())
        if (auto block = graph->delegateModel<FdfBlockModel>(id))
            if (EXCLUDED_TYPES.count(block->type()) < 1)
                serializedObjects.append(serializeNode(id, graph));
    qDebug().noquote() << serializedObjects.join(",\n");
    // TODO: use the serialized objects to call Kedro python scripts
    return true;
}

bool Kedro::validityCheck(QtNodes::DirectedAcyclicGraphModel *graph)
{
    qInfo() << "Checking validity...";
    if (graph->isEmpty())
    {
        qWarning() << "There is no blocks in the graph to execute";
        return false;
    }
    if (!graph->isConnected())
    {
        qWarning() << "The blocks in the graph are not connected";
        return false;
    }
    qInfo() << "Passed validity checks!";
    return true;
}

QVariant Kedro::getNodeOutput(QtNodes::DirectedAcyclicGraphModel *graph, QtNodes::NodeId id)
{
    return graph->nodeData(id, QtNodes::NodeRole::InternalData);
}

QString Kedro::serializeNode(const QtNodes::NodeId &id, QtNodes::DirectedAcyclicGraphModel *graph) const
{
    return toString(*graph->delegateModel<FdfBlockModel>(id));
}
