#pragma once

#include <QtNodes/NodeData>

#include "data/constants.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class NamedNode : public NodeData
{
public:
    NamedNode(const QString &name)
        : m_defaultName(name)
        , m_type({"NamedNode", m_defaultName})
    {}

    virtual QString id() { return m_type.id; }
    QString name() const { return m_type.name; }
    QString defaultName() const { return m_defaultName; }
    void setName(const QString &name) { m_type.name = name; }
    void setDefaultName(const QString &name) { m_defaultName = name; }
    void reset() { m_type.name = m_defaultName; }
    NodeDataType type() const override { return m_type; }

protected:
    QString m_defaultName;
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
        m_type.id = constants::DATA_PORT_ID;
    }
    QUuid typeId() const { return m_typeId; }
    void setTypeId(const QUuid &typeId) { m_typeId = typeId; }

private:
    QUuid m_typeId;
};

class FunctionNode : public NamedNode
{
public:
    struct Signature
    {
        std::vector<QUuid> inputs;
        std::vector<QUuid> outputs;
        std::pair<size_t, size_t> size() const { return {inputs.size(), outputs.size()}; }
        void update(unsigned int port, QUuid typeId) {
            if (port < inputs.size())
                inputs.at(port) = typeId;
            else
                outputs.at(port - inputs.size()) = typeId;  
        }
        void inverse() { std::swap(inputs, outputs); }
        bool isEmpty() const { return inputs.empty() && outputs.empty(); }
        void print() const { qDebug() << inputs << " => " << outputs; }
    };

    FunctionNode()
        : FunctionNode("function")
    {}
    FunctionNode(const QString &name)
        : NamedNode(name)
    {
        m_type.id = constants::FUNCTION_PORT_ID;
    }
    Signature signature() const { return m_singature; }
    void setSignature(const Signature &signature) { m_singature = signature; }

private:
    Signature m_singature;
};