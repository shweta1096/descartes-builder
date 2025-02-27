#pragma once

#include "abstract_engine.hpp"

#include <QProcess>
#include <QTemporaryDir>
#include <QTimer>

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

private slots:
    void onExecutionFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onTimeOut();

private:
    QString serializeNode(const QtNodes::NodeId &id, CustomGraph *graph) const;
    // setups a python venv and install kedro-umbrella to AppDataLocation
    void firstTimeSetup();
    void verifySetup();
    bool generateParametersYml(const QDir &kedroProject, CustomGraph *graph);
    bool generateCatalogYml(const QDir &kedroProject, std::shared_ptr<TabComponents> tab);
    bool generatePipelinePy(const QDir &kedroProject, CustomGraph *graph);
    void postExecutionProcess();
    void postScoreModel(CustomGraph *graph, const QtNodes::NodeId &id);
    void postSensitivityAnalysisModel(CustomGraph *graph, const QtNodes::NodeId &id);
    void releaseExecution();

    const bool m_WINDOWS;
    bool m_setup;
    const QDir m_KEDRO_DIR;
    QTemporaryDir m_runtimeCache;
    // the working dir for this process is the kedro dir
    std::unique_ptr<QProcess> m_setupProcess;
    struct ExecutionBundle
    {
        bool inProgress = false;
        QTimer timer;
        QProcess process;
        QDir project;
        std::shared_ptr<TabComponents> tab;
    };
    std::unique_ptr<ExecutionBundle> m_execution;
    const QString m_DEFAULT_TEMPLATE;
};
