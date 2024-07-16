#include "engine/kedro.hpp"

#include <QtNodes/DagGraphModel>

using QtNodes::DagGraphModel;

Kedro::Kedro()
{
    qDebug() << "Kedro engine is initiated";
}

bool Kedro::execute(QtNodes::DagGraphModel *model)
{
    qInfo() << "Running...";
    if (!validityCheck(model))
        return false;
    return true;
}

bool Kedro::validityCheck(QtNodes::DagGraphModel *model)
{
    qInfo() << "Checking validity...";
    if (model->isEmpty())
        return false;
    for (auto id : model->allNodeIds())
        qDebug() << getNodeOutput(model, id);
    qInfo() << "Passed validity checks!";
    return true;
}

QVariant Kedro::getNodeOutput(QtNodes::DagGraphModel *model, QtNodes::NodeId id)
{
    return model->nodeData(id, QtNodes::NodeRole::InternalData);
}
