#include "ui/models/io_models.hpp"
#include "data/tab_manager.hpp"
#include "ui/models/function_names.hpp"
#include <quazip/JlCompress.h>

#include <QJsonArray>
#include <QLabel>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

namespace {

std::unordered_map<CatalogType, QString> CATALOG_STRING = {
    {CatalogType::Pickle, "pickle.PickleDataset"},
    {CatalogType::Csv, "pandas.CSVDataset"},
    {CatalogType::H5, "kedro_umbrella.library.H5Dataset"},
};

std::unordered_map<QString, CatalogType> CATALOG_EXTENSIONS = {
    {"csv", CatalogType::Csv},
    {"pickle", CatalogType::Pickle},
    {"pkl", CatalogType::Pickle},
    {"mat", CatalogType::H5},
    {"jld2", CatalogType::H5},
};

// Apply node style font color QtNodes::NodeStyle nodeStyle(constants::NODE_STYLE);
inline void applyNodeFontColor(QWidget *widget)
{
    QtNodes::NodeStyle nodeStyle(constants::NODE_STYLE);
    QColor fontColor = nodeStyle.FontColor;

    QPalette pal = widget->palette();
    pal.setColor(QPalette::WindowText, fontColor);
    pal.setColor(QPalette::Text, fontColor);
    widget->setPalette(pal);
    widget->setAutoFillBackground(false);
}

} // namespace

DataSourceModel::DataSourceModel()
    : FdfBlockModel(FdfType::Data, io_names::DATA_SOURCE)
    , m_widget(nullptr)
    , m_label(nullptr)
{}

QWidget *DataSourceModel::embeddedWidget()
{
    if (!m_widget) {
        m_widget = new QWidget();
        m_widget->setProperty("bg", "transparent");
        m_widget->setStyleSheet("*[bg='transparent']{ background: transparent; }");

        auto layout = new QVBoxLayout(m_widget);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        m_label = new QLabel(portCaption(PortType::Out, 0));
        applyNodeFontColor(m_label);
        layout->addWidget(m_label);

        auto button = new QPushButton("Import");
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, &DataSourceModel::importClicked);
    }

    return m_widget;
}

QJsonObject DataSourceModel::save() const
{
    QJsonObject modelJson = FdfBlockModel::save();
    modelJson["data-name"] = m_file.fileName();
    return modelJson;
}

void DataSourceModel::load(QJsonObject const &p)
{
    FdfBlockModel::load(p);
    QJsonValue value = p["data-name"];
    QString filePath = value.toString();
    if (value.isUndefined() || filePath.trimmed().isEmpty())
        return;

    setFile(QFileInfo(value.toString()));
}

QString DataSourceModel::fileTypeString() const
{
    if (m_fileType && CATALOG_STRING.count(m_fileType.value()) > 0)
        return CATALOG_STRING.at(m_fileType.value());
    return "NONE";
}

void DataSourceModel::setFile(const QFileInfo &file)
{
    if (file.fileName().trimmed().isEmpty())
        return;
    if (file == m_file)
        return;
    m_file = file;
    if (CATALOG_EXTENSIONS.count(m_file.suffix()) > 0)
        m_fileType = CATALOG_EXTENSIONS.at(m_file.suffix());
    if (m_label)
        m_label->setText(m_file.fileName());
    // Create an output data port based on the name of file imported
    auto newTag = m_file.baseName();
    addPort<DataNode>(PortType::Out, newTag);
    emit contentUpdated();
}

QString DataSourceModel::fileFilter()
{
    QStringList extensions;
    for (auto &pair : CATALOG_EXTENSIONS)
        extensions << "*." + pair.first;
    return extensions.join(' ');
}

QString DataSourceModel::outPortCaption()
{
    // convenience method to access the port caption
    if (auto outPort = castedPort<DataNode>(PortType::Out, 0))
        return outPort->name();
    return "";
}

bool DataSourceModel::checkBlockValidity() const
{
    // Check if the file is set and has a valid type
    if (m_file.fileName().isEmpty()) {
        qWarning() << "DataSourceModel: No file set.";
        return false;
    }
    return true;
}

FuncSourceModel::FuncSourceModel()
    : FdfBlockModel(FdfType::Output, io_names::FUNC_SOURCE, io_names::FUNC_SOURCE)
    , m_fileType(CatalogType::Pickle)
    , m_widget(nullptr)
    , m_label(nullptr)
{}

QWidget *FuncSourceModel::embeddedWidget()
{
    if (!m_widget) {
        m_widget = new QWidget();
        m_widget->setProperty("bg", "transparent");
        m_widget->setStyleSheet("*[bg='transparent']{ background: transparent; }");

        auto layout = new QVBoxLayout(m_widget);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        m_label = new QLabel(portCaption(PortType::Out, 0));
        applyNodeFontColor(m_label);
        layout->addWidget(m_label);

        auto button = new QPushButton("Import");
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, &FuncSourceModel::importClicked);
    }

    return m_widget;
}

QJsonObject FuncSourceModel::save() const
{
    QJsonObject modelJson = FdfBlockModel::save();
    modelJson["saved_function"] = m_file.fileName();
    return modelJson;
}

void FuncSourceModel::load(QJsonObject const &p)
{
    FdfBlockModel::load(p);
    QJsonValue value = p["saved_function"];
    QString filePath = value.toString();
    if (value.isUndefined() || filePath.trimmed().isEmpty())
        return;

    setFile(QFileInfo(value.toString()));
}

void FuncSourceModel::setFile(const QFileInfo &file)
{
    if (file.fileName().trimmed().isEmpty())
        return;
    if (file == m_file)
        return;
    m_file = file;

    auto dataDir = TabManager::instance().getCurrentTab()->getDataDir();
    auto tmpDir = TabManager::instance().getCurrentTab()->getTempDir();
    QString zipPath = dataDir.filePath(m_file.fileName());
    if (!QFile::exists(zipPath)) {
        return;
    }

    // keep the dataDir clean, as it finally is compressed into the dcb file
    // so, unzip into the tmpDir
    // extract only the light-weight json from the zip into tempUnzipDir
    // to create unique "func_unzip_filehash" folder. Here is
    // where the full zip is extracted. The tempUnzipDir is deleted
    // after full extraction to keep the tmpDir clean
    QStringList filesInZip = JlCompress::getFileList(zipPath);
    QString jsonFileName;
    for (const QString &f : filesInZip) {
        if (f.endsWith(".json")) {
            jsonFileName = f;
            break;
        }
    }
    if (jsonFileName.isEmpty()) {
        qWarning() << "FuncSourceModel: No JSON metadata found in archive.";
        return;
    }
    QString tempFolder = tmpDir->filePath(QString("func_unzip_tmp"));
    QDir tempUnzipDir(tempFolder);
    if (tempUnzipDir.exists())
        tempUnzipDir.removeRecursively();
    tempUnzipDir.mkpath(".");
    QStringList extractedJson = JlCompress::extractFiles(zipPath,
                                                         {jsonFileName},
                                                         tempUnzipDir.absolutePath());
    if (extractedJson.isEmpty()) {
        qWarning() << "FuncSourceModel: Failed to extract JSON metadata.";
        return;
    }

    QFile jsonFile(extractedJson.first());
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "FuncSourceModel: Failed to read metadata json.";
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
    if (!doc.isObject()) {
        qWarning() << "FuncSourceModel: Invalid JSON format.";
        return;
    }
    QJsonObject meta = doc.object();
    QString fileHash = meta["file_hash"].toString().trimmed();
    if (fileHash.isEmpty()) {
        qWarning() << "FuncSourceModel: file_hash missing in metadata.";
        return;
    }

    QString hashFolderPath = tmpDir->filePath(QString("func_unzip_%1").arg(fileHash));
    QDir hashDir(hashFolderPath);
    if (!hashDir.exists())
        hashDir.mkpath(".");

    QStringList unzippedFiles = JlCompress::extractDir(zipPath, hashDir.absolutePath());
    if (unzippedFiles.isEmpty()) {
        qWarning() << "FuncSourceModel: Archive extraction failed.";
        return;
    }
    QString dillPath;
    for (const QString &path : unzippedFiles) {
        if (path.endsWith(".pkl")) {
            dillPath = path;
            break;
        }
    }
    if (dillPath.isEmpty()) {
        qWarning() << "FuncSourceModel: .pkl missing in archive.";
        return;
    }
    m_dillPath = dillPath;

    QJsonObject sig = meta["function_signature"].toObject();
    if (sig.isEmpty() || sig["input"].toArray().isEmpty() || sig["output"].toArray().isEmpty()) {
        qWarning() << "FuncSourceModel: Invalid or missing function signature in metadata.";
        return;
    }

    auto uidManager = TabManager::getUIDManager();
    auto remapUID = [&](int original) -> FdfUID {
        return uidManager->getOrCreateUIDOnFuncLoad(fileHash, original);
    };
    std::vector<FdfUID> newInputs, newOutputs;
    for (const QJsonValue &val : sig["input"].toArray())
        newInputs.push_back(remapUID(val.toInt()));
    for (const QJsonValue &val : sig["output"].toArray())
        newOutputs.push_back(remapUID(val.toInt()));
    Signature signature{newInputs, newOutputs};

    addPort<FunctionNode>(PortType::Out);
    auto port = castedPort<FunctionNode>(PortType::Out, 0);
    if (!port) {
        qWarning() << "FuncSourceModel: Failed to create function port.";
        return;
    }

    // clean up temp dir
    if (tempUnzipDir.exists())
        tempUnzipDir.removeRecursively();
    port->setSignature(signature);
    port->setName(m_file.baseName());
    emit contentUpdated();
}

QString FuncSourceModel::fileTypeString() const
{
    if (CATALOG_STRING.count(m_fileType) > 0)
        return CATALOG_STRING.at(m_fileType);
    return "NONE";
}

QString FuncSourceModel::getFileName() const
{
    // the port caption is used as the file name
    return portCaption(PortType::Out, 0);
}

bool FuncSourceModel::checkBlockValidity() const
{
    // Check if the file is set and has a valid type
    if (m_file.fileName().isEmpty()) {
        qWarning() << "FunctionSourceModel: No file set.";
        return false;
    }
    return true;
}

FuncOutModel::FuncOutModel()
    : FdfBlockModel(FdfType::Output, io_names::FUNC_OUT, io_names::FUNC_OUT)
    , m_fileType(CatalogType::Pickle)
{
    QDir default_saveDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
                               .filePath(QString("DescartesBuilder") + QDir::separator()
                                         + QString("SavedFunctions"));
    if (!default_saveDir.exists())
        default_saveDir.mkpath(".");
    m_saveDir = default_saveDir;
    addPort<FunctionNode>(PortType::In);
}

QString FuncOutModel::fileTypeString() const
{
    if (CATALOG_STRING.count(m_fileType) > 0)
        return CATALOG_STRING.at(m_fileType);
    return "NONE";
}

QString FuncOutModel::getFileName() const
{
    return portCaption(PortType::In, 0);
}

QString FuncOutModel::getFileExtenstion() const
{
    for (auto &pair : CATALOG_EXTENSIONS)
        if (pair.second == m_fileType)
            return pair.first;
    return QString();
}

std::unordered_map<QString, QString> FuncOutModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    if (m_saveDir.exists())
        result["save_dir"] = m_saveDir.absolutePath();
    result["save_filename"] = caption();
    return result;
}

std::unordered_map<QString, QMetaType::Type> FuncOutModel::getParameterSchema() const
{
    std::unordered_map<QString, QMetaType::Type> schema;
    schema["save_dir"] = QMetaType::QString;
    return schema;
}

void FuncOutModel::setParameter(const QString &key, const QString &value)
{
    if (key == "save_dir") {
        QDir dir(value);
        if (dir.exists()) {
            m_saveDir = dir;
        }
    }
}

Signature FuncOutModel::getFuncSignature()
{
    if (auto function = castedPort<FunctionNode>(PortType::In, 0))
        return function->signature();
    return Signature();
}

DataOutModel::DataOutModel()
    : FdfBlockModel(FdfType::Output, io_names::DATA_OUT, io_names::DATA_OUT)
    , m_fileType(CatalogType::Pickle)
{
    addPort<DataNode>(PortType::In);
}

QString DataOutModel::fileTypeString() const
{
    if (CATALOG_STRING.count(m_fileType) > 0)
        return CATALOG_STRING.at(m_fileType);
    return "NONE";
}

QString DataOutModel::getFileName() const
{
    return portCaption(PortType::In, 0);
}

QString DataOutModel::getFileExtenstion() const
{
    for (auto &pair : CATALOG_EXTENSIONS)
        if (pair.second == m_fileType)
            return pair.first;
    return QString();
}

GraphModel::GraphModel()
    : FdfBlockModel(FdfType::Output, io_names::GRAPH_FUNCTION, io_names::GRAPH_FUNCTION)
    , m_graph(nullptr)
{
    addPort<FunctionNode>(PortType::In, "f");
}

void GraphModel::setFile(const QFileInfo &file)
{
    if (file == m_file)
        return;
    m_file = file;
    updateGraph();
}

QWidget *GraphModel::embeddedWidget()
{
    if (!m_graph) {
        m_graph = new QLabel;
        m_graph->setStyleSheet("QLabel{ background: transparent; }");
        if (!m_file.absoluteFilePath().isEmpty())
            m_graph->setPixmap(QPixmap(m_file.absoluteFilePath()));
    }
    return m_graph;
}

void GraphModel::updateGraph()
{
    if (!m_graph)
        return;
    m_graph->setPixmap(QPixmap(m_file.absoluteFilePath()));
    emit contentUpdated();
}
