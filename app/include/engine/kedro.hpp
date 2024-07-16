#pragma once

#include "abstract_engine.hpp"

class Kedro : public AbstractEngine
{
public:
    Kedro();
    virtual bool execute(QtNodes::DagGraphModel *model) override;
    virtual bool validityCheck(QtNodes::DagGraphModel *model) override;
    virtual QVariant getNodeOutput(QtNodes::DagGraphModel *model, QtNodes::NodeId id) override;
};
