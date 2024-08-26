#pragma once

#include "fdf_block_model.hpp"

#include <QFileInfo>

#include <QtUtility/data/constexpr_qstring.hpp>

class QWidget;
class QLabel;

namespace io_names {
using ConstLatin1String = QtUtility::data::ConstLatin1String;
constexpr ConstLatin1String DATA_SOURCE = "data_source";
constexpr ConstLatin1String FUNC_OUT = "func_out";
constexpr ConstLatin1String DATA_OUT = "data_out";
} // namespace io_names

enum CatalogType { Pickle, Csv, H5 };

class DataSourceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    DataSourceModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {};
    QWidget *embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const &p) override;
    QFileInfo file() const { return m_file; }
    std::optional<CatalogType> fileType() const { return m_fileType; }
    QString fileTypeString() const;
    void setFile(const QFileInfo &file);
    static QString fileFilter();

signals:
    void importClicked();

private slots:
    void updatePortCaption(const QString &name);

private:
    QWidget *m_widget;
    QLabel *m_label;

    // not the actual file path, using it for relative path
    QFileInfo m_file;
    std::optional<CatalogType> m_fileType;
};

class FuncOutModel : public FdfBlockModel
{
    Q_OBJECT
public:
    FuncOutModel();
    CatalogType getFileType() const { return m_fileType; }
    QString fileTypeString() const;
    void setFileType(const CatalogType &fileType) { m_fileType = fileType; }
    QString getFileName() const;
    QString getFileExtenstion() const;

private:
    CatalogType m_fileType;
};

class DataOutModel : public FdfBlockModel
{
    Q_OBJECT
public:
    DataOutModel();
    CatalogType getFileType() const { return m_fileType; }
    QString fileTypeString() const;
    void setFileType(const CatalogType &fileType) { m_fileType = fileType; }
    QString getFileName() const;
    QString getFileExtenstion() const;

private:
    CatalogType m_fileType;
};