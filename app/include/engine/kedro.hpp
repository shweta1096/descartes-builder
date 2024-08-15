#pragma once

#include "abstract_engine.hpp"

#include <QTemporaryDir>

class QProcess;
class CustomGraph;

class Kedro : public AbstractEngine
{
    Q_OBJECT
public:
    Kedro();
    ~Kedro();
    virtual bool execute(std::shared_ptr<TabComponents> tab) override;
    virtual bool validityCheck(std::shared_ptr<TabComponents> tab) override;
    QDir initWorkspace(std::shared_ptr<TabComponents> tab);

private:
    QString serializeNode(const QtNodes::NodeId &id, CustomGraph *graph) const;
    // setups a python venv and install kedro-umbrella to AppDataLocation
    void firstTimeSetup();
    void verifySetup();
    bool generateParametersYml(const QDir &kedroProject, CustomGraph *graph);
    bool generateCatalogYml(const QDir &kedroProject, std::shared_ptr<TabComponents> tab);
    bool generatePipelinePy(const QDir &kedroProject, CustomGraph *graph);

    const bool m_WINDOWS;
    bool m_setup;
    const QDir m_KEDRO_DIR;
    QTemporaryDir m_runtimeCache;
    // the working dir for this process is the kedro dir
    std::unique_ptr<QProcess> m_process;
    const QString m_VENV_PYTHON;
    const QString m_DEFAULT_TEMPLATE;
};
