#pragma once

#include "abstract_engine.hpp"

#include <QDir>

class QProcess;
class QTemporaryDir;

class Kedro : public AbstractEngine
{
public:
    Kedro();
    ~Kedro();
    virtual bool execute(CustomGraph *graph, const QString &name) override;
    virtual bool validityCheck(CustomGraph *graph) override;
    virtual QVariant getNodeOutput(CustomGraph *graph, QtNodes::NodeId id) override;
    std::unique_ptr<QTemporaryDir> initNewWorkspace(const QString &name);

private:
    QString serializeNode(const QtNodes::NodeId &id, CustomGraph *graph) const;
    void firstTimeSetup();
    void verifySetup();

    const bool m_WINDOWS;
    bool m_setup;
    const QDir m_KEDRO_DIR;
    std::unique_ptr<QProcess> m_process;
    const QString m_VENV_PYTHON;
    const QString m_DEFAULT_TEMPLATE;
};
