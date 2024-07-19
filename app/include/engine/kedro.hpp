#pragma once

#include "abstract_engine.hpp"

class Kedro : public AbstractEngine
{
public:
    Kedro();
    virtual bool execute(QtNodes::DirectedAcyclicGraphModel *model) override;
    virtual bool validityCheck(QtNodes::DirectedAcyclicGraphModel *model) override;
    virtual QVariant getNodeOutput(QtNodes::DirectedAcyclicGraphModel *model, QtNodes::NodeId id) override;

private:
    QString serialNode(const QtNodes::NodeId &id, QtNodes::DirectedAcyclicGraphModel *model) const;
};
