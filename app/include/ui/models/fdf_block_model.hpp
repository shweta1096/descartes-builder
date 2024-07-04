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
    FdfBlockModel(const QString &caption, const QString &name);
    QString caption() const override;
    QString name() const override;
    unsigned int nPorts(PortType const portType) const override;
    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    virtual void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
    virtual QWidget *embeddedWidget() override;

    PortIndex addPort(PortType const portType, std::shared_ptr<NodeData> port);

private:
    QString m_caption;
    QString m_name;
    std::map<PortType, std::vector<std::shared_ptr<NodeData>>> m_ports;
};