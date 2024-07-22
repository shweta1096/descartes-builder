#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class NamedNode : public NodeData
{
public:
    NamedNode(const QString &name) : m_DEFAULT(name), m_name(m_DEFAULT) {}

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }
    void reset() { m_name = m_DEFAULT; }

protected:
    const QString m_DEFAULT;
    QString m_name;
};

class DataNode : public NamedNode
{
public:
    DataNode() : NamedNode("data") {}
    DataNode(const QString &name) : NamedNode(name) {}

    NodeDataType type() const override { return NodeDataType{"DataNode", m_name}; }
};

class FunctionNode : public NamedNode
{
public:
    FunctionNode() : NamedNode("function") {}
    FunctionNode(const QString &name) : NamedNode(name) {}

    NodeDataType type() const override { return NodeDataType{"FunctionNode", m_name}; }
};