#pragma once

#include "fdf_block_model.hpp"

#include <QFileInfo>

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
    enum CatalogType { Pickle, Csv, H5 };

    DataSourceModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {};
    QWidget *embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const &p) override;
    QFileInfo file() const { return m_file; }
    QString fileName() const { return m_fileName; }
    std::optional<CatalogType> fileType() const { return m_fileType; }
    void setFile(const QFileInfo &file);

private slots:
    void onWidgetEdited(const QString &name);

private:
    QLineEdit *m_widget;
    QFileInfo m_file;
    QString m_fileName;
    std::optional<CatalogType> m_fileType;
};

class FuncOutModel : public FdfBlockModel
{
    Q_OBJECT
public:
    FuncOutModel();
};