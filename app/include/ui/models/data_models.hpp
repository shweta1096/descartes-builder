#pragma once

#include "fdf_block_model.hpp"

class QLineEdit;

class DataSourceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    DataSourceModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {};
    QWidget *embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const &p) override;

private slots:
    void onWidgetEdited(const QString &name);

private:
    std::shared_ptr<DataNode> m_data;
    QLineEdit *m_widget;
};