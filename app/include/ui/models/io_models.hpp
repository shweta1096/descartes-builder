#pragma once

#include "fdf_block_model.hpp"

#include <QtUtility/data/constexpr_qstring.hpp>

class QLineEdit;

namespace io_names {
using ConstLatin1String = QtUtility::data::ConstLatin1String;
constexpr ConstLatin1String DATA_SOURCE = "data_source";
constexpr ConstLatin1String FUNC_OUT = "func_out";
} // namespace io_names

class DataSourceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    DataSourceModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {};
    QWidget *embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const &p) override;
    QString fileName() const { return m_fileName; }

private slots:
    void onWidgetEdited(const QString &name);

private:
    QLineEdit *m_widget;
    QString m_fileName;
};

class FuncOutModel : public FdfBlockModel
{
    Q_OBJECT
public:
    FuncOutModel();
};