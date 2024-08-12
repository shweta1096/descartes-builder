#include "ui/models/io_models.hpp"

#include <QLineEdit>

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
        m_widget = new QLineEdit();

        m_widget->setMaximumSize(m_widget->sizeHint());

        connect(m_widget, &QLineEdit::textChanged, this, &DataSourceModel::onWidgetEdited);

        m_widget->setText(portCaption(PortType::Out, 0));
    }

    return m_widget;
}

QJsonObject DataSourceModel::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();
    modelJson["data-name"] = m_fileName;
    return modelJson;
}

void DataSourceModel::load(QJsonObject const &p)
{
    QJsonValue value = p["data-name"];

    if (value.isUndefined())
        return;

    QString data = value.toString();
    m_fileName = data;
    setPortCaption(PortType::Out, 0, data);
    if (m_widget) {
        m_widget->setText(data);
    }
    emit contentUpdated();
}

void DataSourceModel::setFile(const QFileInfo &file)
{
    if (file == m_file)
        return;
    if (!file.exists())
        return;
    // move file to workspace
    // m_file = ; // assign to path to workspace
}

void DataSourceModel::onWidgetEdited(const QString &name)
{
    if (name == portCaption(PortType::Out, 0))
        return;
    m_fileName = name;
    setPortCaption(PortType::Out, 0, name);
    emit contentUpdated();
}

FuncOutModel::FuncOutModel()
    : FdfBlockModel(FdfType::Output, io_names::FUNC_OUT)
{
    addPort<FunctionNode>(PortType::In);
}
