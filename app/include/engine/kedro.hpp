#pragma once

#include "abstract_engine.hpp"

class Kedro : public AbstractEngine
{
public:
    Kedro();
    virtual bool execute(QtNodes::DataFlowGraphModel *model) override;
    virtual bool validityCheck(QtNodes::DataFlowGraphModel *model) override;
    virtual QVariant getNodeOutput(QtNodes::DataFlowGraphModel *model, QtNodes::NodeId id) override;
};
