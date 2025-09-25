#pragma once

#include "fdf_block_model.hpp"

#include <QFileInfo>

#include <QtUtility/data/constexpr_qstring.hpp>

class QWidget;
class QLabel;



enum CatalogType { Pickle, Csv, H5 };

class DataSourceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    DataSourceModel();
    QWidget *embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const &p) override;
    QFileInfo file() const { return m_file; }
    std::optional<CatalogType> fileType() const { return m_fileType; }
    QString fileTypeString() const;
    void setFile(const QFileInfo &file);
    static QString fileFilter();
    QString outPortCaption();
    bool checkBlockValidity() const override;

signals:
    void importClicked();

private:
    QWidget *m_widget;
    QLabel *m_label;

    // not the actual file path, using it for relative path
    QFileInfo m_file;
    std::optional<CatalogType> m_fileType;
};

class FuncSourceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    FuncSourceModel();
    QWidget *embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const &p) override;
    void setFile(const QFileInfo &file);
    QString dillPath() const { return m_dillPath; }
    QFileInfo file() const { return m_file; }
    QString fileTypeString() const;
    QString getFileName() const;
    bool checkBlockValidity() const override;

signals:
    void importClicked();

private:
    QWidget *m_widget;
    QLabel *m_label;
    CatalogType m_fileType;
    QFileInfo m_file;
    QString m_dillPath;
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
    virtual std::unordered_map<QString, QString> getParameters() const override;
    virtual std::unordered_map<QString, QMetaType::Type> getParameterSchema() const override;
    virtual void setParameter(const QString &key, const QString &value) override;
    QString getSaveDir() const { return m_saveDir.absolutePath(); }
    Signature getFuncSignature();

private:
    CatalogType m_fileType;
    QDir m_saveDir; // Directory where the zip file will be saved
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

class GraphModel : public FdfBlockModel
{
    Q_OBJECT
public:
    GraphModel();
    QFileInfo file() const { return m_file; }
    void setFile(const QFileInfo &file);
    QWidget *embeddedWidget() override;

private:
    void updateGraph();

    QLabel *m_graph;
    QFileInfo m_file;
};