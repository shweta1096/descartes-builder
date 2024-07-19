#include "engine/kedro.hpp"

#include <QtNodes/DirectedAcyclicGraphModel>

#include "ui/models/fdf_block_model.hpp"

#include <iostream>

using QtNodes::DirectedAcyclicGraphModel;

namespace
{
    QString toString(FdfBlockModel *model)
    {
        if (model->functionName().isEmpty())
            return QString("%1(name=\"%2\",inputs=%3,outputs=%4,)")
                .arg(model->typeAsString(),
                     model->name(),
                     "[]",
                     "[]");
        return QString("%1(func=%2,name=\"%3\",inputs=%4,outputs=%5,)")
            .arg(model->typeAsString(),
                 model->functionName(),
                 model->name(),
                 "[]",
                 "[]");
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
        serializedObjects.append(serialNode(id, model));
    qDebug() << "Serialized objects: " << serializedObjects;
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

QString Kedro::serialNode(const QtNodes::NodeId &id, QtNodes::DirectedAcyclicGraphModel *model) const
{
    auto data = model->delegateModel<FdfBlockModel>(id);
    return toString(data);
}
