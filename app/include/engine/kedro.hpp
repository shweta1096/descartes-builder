#pragma once

#include "abstract_engine.hpp"

class Kedro : public AbstractEngine
{
public:
    Kedro();
    virtual bool execute(QtNodes::DirectedAcyclicGraphModel *graph) override;
    virtual bool validityCheck(QtNodes::DirectedAcyclicGraphModel *graph) override;
    virtual QVariant getNodeOutput(QtNodes::DirectedAcyclicGraphModel *graph,
                                   QtNodes::NodeId id) override;

private:
    QString serializeNode(const QtNodes::NodeId &id,
                          QtNodes::DirectedAcyclicGraphModel *graph) const;
};
