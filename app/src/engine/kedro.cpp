#include "engine/kedro.hpp"

#include <QApplication>
#include <QProcess>
#include <QStandardPaths>

#include <QtNodes/DirectedAcyclicGraphModel>

#include <QtUtility/file/file.hpp>

#include <quazip/JlCompress.h>

#include "data/constants.hpp"
#include "data/custom_graph.hpp"
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

enum CatalogType { Pickle, Csv, H5 };

std::unordered_map<CatalogType, QString> catalogString = {
    {CatalogType::Pickle, "pickle.PickleDataSet"},
    {CatalogType::Csv, "pandas.CSVDataSet"},
    {CatalogType::H5, "kedro_umbrella.library.H5Dataset"},
};

} // namespace

namespace {
QStringList getPortList(const FdfBlockModel &block, const PortType &type)
{
    QStringList result;
    for (PortIndex i = 0; i < block.nPorts(type); ++i) {
        auto port = block.portData(type, i);
        result.append(QString("\"%1\"").arg(port->type().name.replace(' ', '_')));
    }
    return result;
}

QString toString(const FdfBlockModel &block)
{
    QString result = block.typeAsString() + '(';
    if (!block.functionName().isEmpty())
        result += QString("func=%1,").arg(block.functionName());
    result += QString("name=\"%1\"").arg(block.caption().replace(' ', '_'));
    QStringList inputs = getPortList(block, PortType::In);
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

bool Kedro::execute(CustomGraph *graph, const QString &name)
{
    if (!validityCheck(graph))
        return false;
    if (!m_setup) {
        qCritical() << "Kedro is not setup yet, please setup kedro before executing";
        return false;
    }
    // Temp dir will auto delete when out of scope
    auto workspace = initNewWorkspace(name);
    QDir kedroProject(workspace->path() + QDir::separator() + name);
    if (!generateParametersYml(kedroProject))
        return false;
    if (!generateCatalogYml(kedroProject, graph))
        return false;
    if (!generatePipelinePy(kedroProject, graph))
        return false;

    // call kedro run
    QProcess run;
    run.setWorkingDirectory(kedroProject.absolutePath());
    run.startCommand(QString("%1 -m kedro run").arg(quote(m_VENV_PYTHON)));
    if (!run.waitForFinished()) {
        qCritical() << "Failed to run kedro";
    }

    // compress dir to zip and cache to runtime dir
    auto zip = QtUtility::file::getUniqueFile(QFileInfo(m_runtimeCache.filePath(name + ".zip")));
    if (JlCompress::compressDir(zip.absoluteFilePath(), workspace->path()))
        qDebug() << "Kedro execution cached to: " << zip.absoluteFilePath();
    return true;
}

bool Kedro::validityCheck(CustomGraph *graph)
{
    qInfo() << "Checking validity...";
    if (graph->isEmpty()) {
        qWarning() << "There is no blocks in the graph to execute";
        return false;
    }
    if (!graph->isConnected()) {
        qWarning() << "The blocks in the graph are not connected";
        return false;
    }
    qInfo() << "Passed validity checks!";
    return true;
}

QVariant Kedro::getNodeOutput(CustomGraph *graph, QtNodes::NodeId id)
{
    return graph->nodeData(id, QtNodes::NodeRole::InternalData);
}

std::unique_ptr<QTemporaryDir> Kedro::initNewWorkspace(const QString &name)
{
    std::unique_ptr<QTemporaryDir> dir = std::make_unique<QTemporaryDir>();
    if (!dir->isValid())
        qCritical() << "Temporary dir is invalid";
    QProcess workspaceProcess;
    workspaceProcess.setWorkingDirectory(dir->path());
    QString command = QString("bash -c \"echo %1 | %2 -m kedro new -s %3\"")
                          .arg(singleQuote(name),
                               singleQuote(m_VENV_PYTHON),
                               singleQuote(m_DEFAULT_TEMPLATE));
    workspaceProcess.startCommand(command);
    if (!workspaceProcess.waitForFinished()) {
        qCritical() << "Failed to create workspace " << name;
        return std::unique_ptr<QTemporaryDir>();
    }
    return dir;
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
    m_venvPython = "venv\\Scripts\\python.exe";
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

bool Kedro::generateParametersYml(const QDir &kedroProject)
{
    // TODO: rework system for a parameters.yml file
    // every graph will need their own parameters.yml which store vars about blocks
    // or add properties to the blocks that are used to generate the parameters yml
    QDir conf(kedroProject.absoluteFilePath(constants::kedro::CONF_PATH));
    qDebug() << "Generated parameters to: " << conf.absolutePath();
    return true;
}

bool Kedro::generateCatalogYml(const QDir &kedroProject, CustomGraph *graph)
{
    QDir conf(kedroProject.absoluteFilePath(constants::kedro::CONF_PATH));
    auto dataSources = graph->getDataSourceModels();
    for (auto data : dataSources)
        qDebug() << data->fileName();
    qDebug() << "Generated catalog to: " << conf.absolutePath();
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
    qDebug() << "Generated pipeline to: " << source.absolutePath();
    return true;
}
