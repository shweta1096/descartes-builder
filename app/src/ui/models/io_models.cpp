#include "ui/models/io_models.hpp"

#include <QLineEdit>

DataSourceModel::DataSourceModel()
    : FdfBlockModel(FdfType::Data, "data_source")
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
    modelJson["data-name"] = portCaption(PortType::Out, 0);
    return modelJson;
}

void DataSourceModel::load(QJsonObject const &p)
{
    QJsonValue value = p["data-name"];

    if (value.isUndefined())
        return;

    QString data = value.toString();
    setPortCaption(PortType::Out, 0, data);
    if (m_widget)
        m_widget->setText(data);
}

void DataSourceModel::onWidgetEdited(const QString &name)
{
    if (name == portCaption(PortType::Out, 0))
        return;
    setPortCaption(PortType::Out, 0, name);
    emit contentUpdated();
}

FuncOutModel::FuncOutModel()
    : FdfBlockModel(FdfType::Output, "func_out")
{
    addPort<FunctionNode>(PortType::In);
}
