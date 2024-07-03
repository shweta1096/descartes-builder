#pragma once

#include <QObject>

#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

#include "ui/models/nodes.hpp"

class CoderModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    CoderModel();
    QString caption() const override { return QString("Coder"); }
    QString name() const override { return QString("CoderModel"); }
    unsigned int nPorts(PortType const portType) const override;
    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
    QWidget *embeddedWidget() override { return nullptr; }
};