#pragma once

#include <QObject>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>

#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class DataNode : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"DataNode", "Data Node"}; }
};

class FunctionNode : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"FunctionNode", "Function Node"}; }
};

class TestModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    TestModel();
    QString caption() const override { return QString("Test Model"); }
    QString name() const override { return QString("TestModel"); }
    unsigned int nPorts(PortType const portType) const override;
    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
    QWidget *embeddedWidget() override { return nullptr; }
};