#pragma once

#include "abstract_engine.hpp"

#include <QDir>

class QProcess;

class Kedro : public AbstractEngine
{
public:
    Kedro();
    ~Kedro();
    virtual bool execute(QtNodes::DirectedAcyclicGraphModel *graph) override;
    virtual bool validityCheck(QtNodes::DirectedAcyclicGraphModel *graph) override;
    virtual QVariant getNodeOutput(QtNodes::DirectedAcyclicGraphModel *graph,
                                   QtNodes::NodeId id) override;
    QDir initNewWorkspace();

private:
    QString serializeNode(const QtNodes::NodeId &id,
                          QtNodes::DirectedAcyclicGraphModel *graph) const;
    void firstTimeSetup();
    void verifySetup();

    bool m_setup;
    QDir m_kedroDir;
    std::unique_ptr<QProcess> m_process;
};
