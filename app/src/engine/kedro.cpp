#include "engine/kedro.hpp"

#include <QtNodes/DirectedAcyclicGraphModel>

#include "ui/models/fdf_block_model.hpp"

#include <iostream>

using QtNodes::DirectedAcyclicGraphModel;

namespace
{
    QStringList getPortList(const FdfBlockModel &model, const PortType &type)
    {
        QStringList result;
        for (uint i = 0; i < model.nPorts(type); ++i)
            if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(model.portData(type, i)))
                result.append(QString("\"%1\"").arg(namedNode->name()));
        return result;
    }

    QString toString(const FdfBlockModel &model)
    {
        QString result = model.typeAsString() + '(';
        if (!model.functionName().isEmpty())
            result += QString("func=%1,").arg(model.functionName());
        result += QString("name=\"%1\",").arg(model.name());
        QStringList inputs = getPortList(model, PortType::In);
        if (inputs.size() == 1)
            result += QString("inputs=%1").arg(inputs.at(0));
        else if (inputs.size() > 1)
            result += QString("inputs=[%1]").arg(inputs.join(','));
        QStringList outputs = getPortList(model, PortType::Out);
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

bool Kedro::execute(QtNodes::DirectedAcyclicGraphModel *model)
{
    qInfo() << "Running...";
    if (!validityCheck(model))
        return false;
    QStringList serializedObjects;
    for (const auto &id : model->topologicalOrder())
        if (model->nodeData(id, QtNodes::NodeRole::Type) != "data_source") // ignore data source for kedro
            serializedObjects.append(serializeNode(id, model));
    qDebug().noquote() << serializedObjects.join(',');
    return true;
}

bool Kedro::validityCheck(QtNodes::DirectedAcyclicGraphModel *model)
{
    qInfo() << "Checking validity...";
    if (model->isEmpty())
    {
        qWarning() << "There is no blocks in the graph to execute";
        return false;
    }
    if (!model->isConnected())
    {
        qWarning() << "The blocks in the graph are not connected";
        return false;
    }
    qInfo() << "Passed validity checks!";
    return true;
}

QVariant Kedro::getNodeOutput(QtNodes::DirectedAcyclicGraphModel *model, QtNodes::NodeId id)
{
    return model->nodeData(id, QtNodes::NodeRole::InternalData);
}

QString Kedro::serializeNode(const QtNodes::NodeId &id, QtNodes::DirectedAcyclicGraphModel *model) const
{
    return toString(*model->delegateModel<FdfBlockModel>(id));
}
