#include "ui/models/test_model.hpp"

TestModel::TestModel()
{
}

unsigned int TestModel::nPorts(PortType const portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 2;
        break;

    case PortType::Out:
        result = 1;
        break;
    case PortType::None:
        break;
    }

    return result;
}

NodeDataType TestModel::dataType(PortType const portType, PortIndex const portIndex) const
{
    switch (portType)
    {
    case PortType::In:
        switch (portIndex)
        {
        case 0:
            return DataNode().type();
        case 1:
            return FunctionNode().type();
        }
        break;

    case PortType::Out:
        switch (portIndex)
        {
        case 0:
            return DataNode().type();
        }
        break;

    case PortType::None:
        break;
    }
    // FIXME: control may reach end of non-void function [-Wreturn-type]
    return NodeDataType();
}

std::shared_ptr<NodeData> TestModel::outData(PortIndex const port)
{
    return std::make_shared<DataNode>();
}

void TestModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
