#include "engine/kedro.hpp"

#include <QApplication>
#include <QProcess>
#include <QStandardPaths>

#include <QtNodes/DirectedAcyclicGraphModel>

#include <QtUtility/file/file.hpp>

#include <quazip/JlCompress.h>

#include "data/constants.hpp"
#include "data/custom_graph.hpp"
#include "data/tab_components.hpp"
#include "ui/models/fdf_block_model.hpp"
#include "ui/models/io_models.hpp"

#include <iostream>

#ifdef Q_OS_WIN
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif

namespace {

using FdfType = FdfBlockModel::FdfType;
const std::unordered_set<FdfType> EXCLUDED_TYPES = {FdfType::Data, FdfType::Output};

QString singleQuote(const QString &string)
{
    return '\'' + string + '\'';
}

QString quote(const QString &string)
{
    return '\"' + string + '\"';
}

} // namespace

namespace {
QStringList getPortList(const FdfBlockModel &block, const PortType &type)
{
    QStringList result;
    for (PortIndex i = 0; i < block.nPorts(type); ++i) {
        // if an in port is not connected it is null, the 'if' can be removed after the validity check handles it
        if (auto port = block.portData(type, i))
            result.append(quote(port->type().name.replace(' ', '_')));
    }
    return result;
}

QString toString(const FdfBlockModel &block)
{
    QString result = block.typeAsString() + '(';
    if (!block.functionName().isEmpty())
        result += QString("func=%1,").arg(block.functionName());
    result += QString("name=%1").arg(quote(block.caption().replace(' ', '_')));
    QStringList inputs = getPortList(block, PortType::In);
    if (block.hasParameters())
        inputs << quote(QString("params:%1").arg(block.caption().replace(' ', '_')));
    if (inputs.size() == 1)
        result += QString(",inputs=%1").arg(inputs.at(0));
    else if (inputs.size() > 1)
        result += QString(",inputs=[%1]").arg(inputs.join(','));
    QStringList outputs = getPortList(block, PortType::Out);
    if (outputs.size() == 1)
        result += QString(",outputs=%1").arg(outputs.at(0));
    else if (outputs.size() > 1)
        result += QString(",outputs=[%1]").arg(outputs.join(','));
    result += ')';
    return result;
}
} // namespace

Kedro::Kedro()
    : m_WINDOWS(IS_WINDOWS)
    , m_setup(false)
    , m_KEDRO_DIR(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/kedro/")
    , m_process(std::make_unique<QProcess>())
    , m_VENV_PYTHON(
          m_KEDRO_DIR.absoluteFilePath(m_WINDOWS ? "venv\\bin\\python.exe" : "venv/bin/python"))
    , m_DEFAULT_TEMPLATE(m_KEDRO_DIR.absoluteFilePath("templates/builder-spring/"))
{
    if (!m_KEDRO_DIR.exists())
        m_KEDRO_DIR.mkpath(".");
    if (!m_runtimeCache.isValid())
        qCritical() << "Temporary dir failed to setup";
    m_process->setWorkingDirectory(m_KEDRO_DIR.absolutePath());
    if (m_KEDRO_DIR.isEmpty())
        firstTimeSetup();
    else
        verifySetup();
}

Kedro::~Kedro() {}

bool Kedro::execute(std::shared_ptr<TabComponents> tab)
{
    qDebug() << "Kedro is executing...";
    if (!validityCheck(tab))
        return false;
    if (!m_setup) {
        qCritical() << "Kedro is not setup yet, please setup kedro before executing";
        return false;
    }
    // Temp dir will auto delete when out of scope
    auto kedroProject = initWorkspace(tab);
    if (!generateParametersYml(kedroProject, tab->getGraph()))
        return false;
    if (!generateCatalogYml(kedroProject, tab))
        return false;
    if (!generatePipelinePy(kedroProject, tab->getGraph()))
        return false;

    // call kedro run
    QProcess run;
    run.setWorkingDirectory(kedroProject.absolutePath());
    run.startCommand(QString("%1 -m kedro run").arg(quote(m_VENV_PYTHON)));
    if (!run.waitForFinished()) {
        qCritical() << "Failed to run kedro";
    }
    auto output = QString::fromUtf8(run.readAllStandardOutput());

    // compress dir to zip and cache to runtime dir
    auto zip = QtUtility::file::getUniqueFile(
        QFileInfo(m_runtimeCache.filePath(tab->getFileInfo().baseName() + ".zip")));
    if (JlCompress::compressDir(zip.absoluteFilePath(), kedroProject.absolutePath()))
        qDebug() << "Kedro executed, result is cached to: " << zip.absoluteFilePath();
    emit executed(output);
    return true;
}

bool Kedro::validityCheck(std::shared_ptr<TabComponents> tab)
{
    auto graph = tab->getGraph();
    qInfo() << "Checking graph validity...";
    if (graph->isEmpty()) {
        qWarning() << "There is no blocks in the graph to execute";
        return false;
    }
    if (!graph->isConnected()) {
        qWarning() << "The blocks in the graph are not connected";
        return false;
    }
    // TODO: add check that every node input is connected
    // TODO: add check that every node is "ready" e.g. data source has imported something
    qInfo() << "Passed validity checks!";
    return true;
}

QDir Kedro::initWorkspace(std::shared_ptr<TabComponents> tab)
{
    auto name = tab->getFileInfo().baseName();
    // kedro dir inside of temp dir, to avoid cases where file name conflicts with existing folder
    QDir kedroDir(tab->getTempDir()->filePath("kedro"));
    kedroDir.mkpath(".");
    QProcess workspaceProcess;
    workspaceProcess.setWorkingDirectory(kedroDir.absolutePath());
    QString command = QString("bash -c \"echo %1 | %2 -m kedro new -s %3\"")
                          .arg(singleQuote(name),
                               singleQuote(m_VENV_PYTHON),
                               singleQuote(m_DEFAULT_TEMPLATE));
    workspaceProcess.startCommand(command);
    if (!workspaceProcess.waitForFinished()) {
        qCritical() << "Failed to create workspace " << name;
        return QDir();
    }
    return QDir(kedroDir.absolutePath() + QDir::separator() + name);
}

QString Kedro::serializeNode(const QtNodes::NodeId &id, CustomGraph *graph) const
{
    return toString(*graph->delegateModel<FdfBlockModel>(id));
}

void Kedro::firstTimeSetup()
{
    qInfo() << "Performing first time set up";
    // extract resources to app data
    if (!QFile::copy(":/kedro-umbrella.zip", m_KEDRO_DIR.filePath("kedro-umbrella.zip")))
        qCritical() << "Failed to copy kedro-umbrella.zip";
    if (!QFile::copy(":/templates.zip", m_KEDRO_DIR.filePath("templates.zip")))
        qCritical() << "Failed to copy templates.zip";
    JlCompress::extractDir(m_KEDRO_DIR.filePath("kedro-umbrella.zip"), m_KEDRO_DIR.absolutePath());
    JlCompress::extractDir(m_KEDRO_DIR.filePath("templates.zip"), m_KEDRO_DIR.absolutePath());
    QFile file(m_KEDRO_DIR.filePath("kedro-umbrella.zip"));

    // clean zip files
    file.remove();
    file.setFileName(m_KEDRO_DIR.filePath("templates.zip"));
    file.remove();

    // setup venv
    QStringList args;
    args << "-m" << "venv" << "venv";
    m_process->start("python", args);
    if (!m_process->waitForFinished()) {
        qWarning() << "Failed to create virtual environment:" << m_process->errorString();
        return;
    }

    // install kedro-umbrella
    QString venvPip = "venv/bin/pip";
#ifdef Q_OS_WIN
    venvPip = "venv\\Scripts\\pip.exe";
#endif
    args.clear();
    args << "install";
#ifdef Q_OS_WIN
    args << "kedro-umbrella\\";
#else
    args << "kedro-umbrella/";
#endif
    QObject::connect(m_process.get(),
                     &QProcess::finished,
                     [this](int, QProcess::ExitStatus exitStatus) {
                         if (exitStatus == QProcess::CrashExit)
                             qWarning() << "Failed to install kedro-umbrella: "
                                        << m_process->errorString();
                         else {
                             QDir dir(m_KEDRO_DIR.absoluteFilePath("kedro-umbrella"));
                             dir.removeRecursively();
                             verifySetup();
                         }
                     });
    m_process->start(venvPip, args);
}

void Kedro::verifySetup()
{
    m_setup = true;
    qInfo() << "Kedro is ready to execute!";
}

bool Kedro::generateParametersYml(const QDir &kedroProject, CustomGraph *graph)
{
    QStringList parameters;
    for (const auto &id : graph->allNodeIds())
        if (auto block = graph->delegateModel<FdfBlockModel>(id)) {
            if (!block->hasParameters())
                continue;
            parameters << block->caption().replace(' ', '_') + ':';
            for (auto &pair : block->getParameters())
                parameters << QString("  %1: %2").arg(pair.first, pair.second);
        }
    QDir conf(kedroProject.absoluteFilePath(constants::kedro::CONF_PATH));
    //generate parameters.yml
    QFile parametersYml(conf.absoluteFilePath("parameters.yml"));
    if (!parametersYml.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Cannot open parameters.yml:" << parametersYml.errorString();
        return false;
    }
    QTextStream out(&parametersYml);
    out << parameters.join("\n");
    parametersYml.close();
    return true;
}

bool Kedro::generateCatalogYml(const QDir &kedroProject, std::shared_ptr<TabComponents> tab)
{
    QDir conf(kedroProject.absoluteFilePath(constants::kedro::CONF_PATH));
    auto dataSources = tab->getGraph()->getDataSourceModels();
    QDir rawDataDir(kedroProject.absoluteFilePath(constants::kedro::RAW_DATA_PATH));
    QStringList catalogEntries;
    for (auto data : dataSources) {
        auto fileName = data->file().fileName();
        // copy data to raw data dir
        QFile::copy(tab->getDataDir().absoluteFilePath(fileName),
                    rawDataDir.absoluteFilePath(fileName));
        // add external data to catalog.yml
        catalogEntries << constants::kedro::CATALOG_YML_ENTRY.arg(data->file().baseName(),
                                                                  data->fileTypeString(),
                                                                  constants::kedro::RAW_DATA_PATH
                                                                      + fileName);
    }
    // add outputs to catalog.yml
    auto funcOuts = tab->getGraph()->getFuncOutModels();
    for (auto funcOut : funcOuts) {
        auto name = funcOut->getFileName();
        catalogEntries << constants::kedro::CATALOG_YML_ENTRY
                              .arg(name,
                                   funcOut->fileTypeString(),
                                   constants::kedro::MODELS_PATH + name + '.'
                                       + funcOut->getFileExtenstion());
    }
    //generate catalog.yml
    QFile catalogYml(conf.absoluteFilePath("catalog.yml"));
    if (!catalogYml.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Cannot open catalog.yml:" << catalogYml.errorString();
        return false;
    }
    QTextStream out(&catalogYml);
    out << catalogEntries.join("\n");
    catalogYml.close();
    return true;
}

bool Kedro::generatePipelinePy(const QDir &kedroProject, CustomGraph *graph)
{
    // for some reason dir name char '-' will convert to '_'
    QDir source(kedroProject.absoluteFilePath(
        QString(constants::kedro::SOURCE_PATH).arg(kedroProject.dirName().replace('-', '_'))));
    QStringList serializedObjects;
    for (const auto &id : graph->topologicalOrder())
        if (auto block = graph->delegateModel<FdfBlockModel>(id))
            if (EXCLUDED_TYPES.count(block->type()) < 1)
                serializedObjects.append(serializeNode(id, graph));
    QString data = constants::kedro::PIPELINE_PY.arg(serializedObjects.join(",\n"));
    QFile pipelinePy(source.absoluteFilePath("pipeline.py"));
    if (!pipelinePy.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Cannot open pipeline.py:" << pipelinePy.errorString();
        return false;
    }
    QTextStream out(&pipelinePy);
    out << data;
    pipelinePy.close();
    return true;
}
