#pragma once

#include <QObject>

#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

#include "ui/models/nodes.hpp"

class FdfBlockModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    enum FdfType
    {
        Coder,
        Processor,
        Trainer,
        Data,
    };

    FdfBlockModel(FdfType type, const QString &name, const QString & = QString());
    FdfType type() const { return m_type; }
    QString typeAsString() const { return TYPE_STRING.at(m_type); }
    QString name() const override { return m_name; }
    QString functionName() const { return m_functionName; }
    QString caption() const override { return m_caption; }
    unsigned int nPorts(PortType const portType) const override;
    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    virtual void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
    virtual QWidget *embeddedWidget() override;
    void setCaption(const QString &caption);

    PortIndex addPort(PortType const portType, std::shared_ptr<NodeData> port);

private:
    FdfType m_type;
    QString m_name; // name in the library
    QString m_functionName;
    QString m_caption; // appears in the scene
    std::map<PortType, std::vector<std::shared_ptr<NodeData>>> m_ports;

    std::unordered_map<FdfType, QString> TYPE_STRING = {
        {FdfType::Coder, "coder"},
        {FdfType::Processor, "processor"},
        {FdfType::Trainer, "trainer"},
        {FdfType::Data, "data"},
    };
};