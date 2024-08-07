#include "engine/kedro.hpp"

#include <QApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <QtNodes/DirectedAcyclicGraphModel>

#include <QtUtility/file/file.hpp>

#include <quazip/JlCompress.h>

#include "ui/models/fdf_block_model.hpp"

#include <iostream>

using QtNodes::DirectedAcyclicGraphModel;

#ifdef Q_OS_WIN
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif

namespace {

using FdfType = FdfBlockModel::FdfType;
const std::unordered_set<FdfType> EXCLUDED_TYPES = {FdfType::Data, FdfType::Output};

} // namespace

namespace {
QStringList getPortList(const FdfBlockModel &block, const PortType &type)
{
    QStringList result;
    for (auto &port : block.connectedPortData(type))
        result.append(QString("\"%1\"").arg(port->type().name));
    return result;
}

QString toString(const FdfBlockModel &block)
{
    QString result = block.typeAsString() + '(';
    if (!block.functionName().isEmpty())
        result += QString("func=%1,").arg(block.functionName());
    result += QString("name=\"%1\"").arg(block.name());
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
    m_process->setWorkingDirectory(m_KEDRO_DIR.absolutePath());
    if (m_KEDRO_DIR.isEmpty())
        firstTimeSetup();
    else
        verifySetup();
}

Kedro::~Kedro() {}

bool Kedro::execute(QtNodes::DirectedAcyclicGraphModel *graph, const QString &name)
{
    if (!validityCheck(graph))
        return false;
    if (!m_setup) {
        qCritical() << "Kedro is not setup yet, please setup kedro before executing";
        return false;
    }
    QStringList serializedObjects;
    for (const auto &id : graph->topologicalOrder())
        if (auto block = graph->delegateModel<FdfBlockModel>(id))
            if (EXCLUDED_TYPES.count(block->type()) < 1)
                serializedObjects.append(serializeNode(id, graph));
    qDebug().noquote() << serializedObjects.join(",\n");

    // TODO: use the serialized objects to call Kedro python scripts
    // Temp dir will auto delete when out of scope
    auto workspace = initNewWorkspace(name);
    // create catalog.yml
    // create paramters.yml
    // create pipeline.py
    return true;
}

bool Kedro::validityCheck(QtNodes::DirectedAcyclicGraphModel *graph)
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

QVariant Kedro::getNodeOutput(QtNodes::DirectedAcyclicGraphModel *graph, QtNodes::NodeId id)
{
    return graph->nodeData(id, QtNodes::NodeRole::InternalData);
}

std::unique_ptr<QTemporaryDir> Kedro::initNewWorkspace(const QString &name)
{
    std::unique_ptr<QTemporaryDir> dir = std::make_unique<QTemporaryDir>();
    if (!dir->isValid())
        qCritical() << "Temporary dir is invalid";
    qDebug() << dir->path();
    QProcess workspaceProcess;
    workspaceProcess.setWorkingDirectory(dir->path());
    QString command = QString("bash -c \"echo \'%1\' | \'%2\' -m kedro new -s \'%3\'\"")
                          .arg(name, m_VENV_PYTHON, m_DEFAULT_TEMPLATE);
    workspaceProcess.startCommand(command);
    if (!workspaceProcess.waitForFinished()) {
        qCritical() << "Failed to create workspace " << name;
        return std::unique_ptr<QTemporaryDir>();
    }
    return dir;
}

QString Kedro::serializeNode(const QtNodes::NodeId &id,
                             QtNodes::DirectedAcyclicGraphModel *graph) const
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
