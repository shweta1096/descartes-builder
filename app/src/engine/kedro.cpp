#include "engine/kedro.hpp"

#include <QtNodes/DataFlowGraphModel>

using QtNodes::DataFlowGraphModel;

Kedro::Kedro()
{
    qDebug() << "Kedro engine init";
}

bool Kedro::execute(QtNodes::DataFlowGraphModel *model)
{
    qDebug() << "called";
    if (!validityCheck(model))
        return false;
    return true;
}

bool Kedro::validityCheck(QtNodes::DataFlowGraphModel *model)
{
    if (model->isEmpty())
        return false;
    return true;
}

QVariant Kedro::getNodeOutput(QtNodes::DataFlowGraphModel *model, QtNodes::NodeId id)
{
    return model->nodeData(id, QtNodes::NodeRole::InternalData);
}
