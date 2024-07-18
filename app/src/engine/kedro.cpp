#include "engine/kedro.hpp"

#include <QtNodes/DirectedAcyclicGraphModel>

using QtNodes::DirectedAcyclicGraphModel;

Kedro::Kedro()
{
    qDebug() << "Kedro engine is initiated";
}

bool Kedro::execute(QtNodes::DirectedAcyclicGraphModel *model)
{
    qInfo() << "Running...";
    if (!validityCheck(model))
        return false;
    qDebug() << "TOPOLOGICAL ORDER: " << model->topologicalOrder();
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
