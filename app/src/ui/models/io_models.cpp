#include "ui/models/io_models.hpp"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

std::unordered_map<DataSourceModel::CatalogType, QString> CATALOG_STRING = {
    {DataSourceModel::CatalogType::Pickle, "pickle.PickleDataSet"},
    {DataSourceModel::CatalogType::Csv, "pandas.CSVDataSet"},
    {DataSourceModel::CatalogType::H5, "kedro_umbrella.library.H5Dataset"},
};

std::unordered_map<QString, DataSourceModel::CatalogType> CATALOG_EXTENSIONS = {
    {"csv", DataSourceModel::CatalogType::Csv},
    {"pickle", DataSourceModel::CatalogType::Pickle},
    {"mat", DataSourceModel::CatalogType::H5},
    {"jld2", DataSourceModel::CatalogType::H5},
};

} // namespace

DataSourceModel::DataSourceModel()
    : FdfBlockModel(FdfType::Data, io_names::DATA_SOURCE)
{
    addPort<DataNode>(PortType::Out);
}

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
        layout->addWidget(m_label);

        auto button = new QPushButton("Import");
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, &DataSourceModel::importClicked);
    }

    return m_widget;
}

QJsonObject DataSourceModel::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();
    modelJson["data-name"] = m_file.fileName();
    return modelJson;
}

void DataSourceModel::load(QJsonObject const &p)
{
    QJsonValue value = p["data-name"];

    if (value.isUndefined())
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
    if (file == m_file)
        return;
    m_file = file;
    if (CATALOG_EXTENSIONS.count(m_file.suffix()) > 0)
        m_fileType = CATALOG_EXTENSIONS.at(m_file.suffix());
    m_label->setText(m_file.fileName());
    updatePortCaption(m_file.baseName());
    emit contentUpdated();
}

QString DataSourceModel::fileFilter()
{
    QStringList extensions;
    for (auto &pair : CATALOG_EXTENSIONS)
        extensions << "*." + pair.first;
    return extensions.join(' ');
}

void DataSourceModel::updatePortCaption(const QString &name)
{
    if (name == portCaption(PortType::Out, 0))
        return;
    setPortCaption(PortType::Out, 0, name);
}

FuncOutModel::FuncOutModel()
    : FdfBlockModel(FdfType::Output, io_names::FUNC_OUT)
{
    addPort<FunctionNode>(PortType::In);
}
