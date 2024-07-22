#include "ui/models/data_models.hpp"

#include <QLineEdit>

DataSourceModel::DataSourceModel()
    : FdfBlockModel(FdfType::Data, "data_source"),
      m_data(std::make_shared<DataNode>("data"))
{
    addPort(PortType::Out, m_data);
}

QWidget *DataSourceModel::embeddedWidget()
{
    if (!m_widget)
    {
        m_widget = new QLineEdit();

        m_widget->setMaximumSize(m_widget->sizeHint());

        connect(m_widget, &QLineEdit::textChanged, this, &DataSourceModel::onWidgetEdited);

        m_widget->setText(m_data->name());
    }

    return m_widget;
}

QJsonObject DataSourceModel::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();
    modelJson["data-name"] = m_data->name();
    return modelJson;
}

void DataSourceModel::load(QJsonObject const &p)
{
    QJsonValue value = p["data-name"];

    if (value.isUndefined())
        return;

    QString data = value.toString();
    m_data->setName(data);
    if (m_widget)
        m_widget->setText(data);
}

void DataSourceModel::onWidgetEdited(const QString &name)
{
    if (name == m_data->name())
        return;
    m_data->setName(name);
    propagateUpdate();
}