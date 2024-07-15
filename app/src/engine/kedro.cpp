#include "engine/kedro.hpp"

#include <QtNodes/DataFlowGraphModel>

using QtNodes::DataFlowGraphModel;

Kedro::Kedro()
{
}

bool Kedro::execute(QtNodes::DataFlowGraphModel *model)
{
    return false;
}

bool Kedro::validityCheck(QtNodes::DataFlowGraphModel *model)
{
    return false;
}

QVariant Kedro::getNodeOutput(QtNodes::NodeId id)
{
    return QVariant();
}
