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

    QString data = value.toString();
    m_file.setFile(data);
    setPortCaption(PortType::Out, 0, data);
    if (m_widget) {
        m_label->setText(data);
    }
    emit contentUpdated();
}

void DataSourceModel::setFile(const QFileInfo &file)
{
    if (file == m_file)
        return;
    if (!file.exists())
        return;
    m_file = file;
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
