#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;

class DataNode : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"DataNode", "Data"}; }
};

class FunctionNode : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"FunctionNode", "Function"}; }
};