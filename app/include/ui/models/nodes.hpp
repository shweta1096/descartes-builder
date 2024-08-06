#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class NamedNode : public NodeData
{
public:
    NamedNode(const QString &name)
        : m_DEFAULT_NAME(name)
        , m_type({"NamedNode", m_DEFAULT_NAME})
    {}

    virtual QString id() { return m_type.id; }
    QString name() const { return m_type.name; }
    QString DefaultName() const { return m_DEFAULT_NAME; }
    void setName(const QString &name) { m_type.name = name; }
    void reset() { m_type.name = m_DEFAULT_NAME; }
    NodeDataType type() const override { return m_type; }

protected:
    const QString m_DEFAULT_NAME;
    NodeDataType m_type;
};

class DataNode : public NamedNode
{
public:
    DataNode()
        : DataNode("data")
    {}
    DataNode(const QString &name)
        : NamedNode(name)
    {
        m_type.id = "DataNode";
    }
};

class FunctionNode : public NamedNode
{
public:
    FunctionNode()
        : FunctionNode("function")
    {}
    FunctionNode(const QString &name)
        : NamedNode(name)
    {
        m_type.id = "FunctionNode";
    }
};