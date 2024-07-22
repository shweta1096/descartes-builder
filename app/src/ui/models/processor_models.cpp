#include "ui/models/processor_models.hpp"

ProcessorSplitDataModel::ProcessorSplitDataModel()
    : FdfBlockModel(FdfType::Processor, "split_data", "split_data")
{
    addPort(PortType::In, std::make_shared<DataNode>("data"));
    addPort(PortType::In, std::make_shared<DataNode>("data"));
    addPort(PortType::In, std::make_shared<DataNode>("parameters"));
    addPort(PortType::Out, std::make_shared<DataNode>("X_train"));
    addPort(PortType::Out, std::make_shared<DataNode>("X_test"));
    addPort(PortType::Out, std::make_shared<DataNode>("Y_train"));
    addPort(PortType::Out, std::make_shared<DataNode>("Y_test"));
}

void ProcessorSplitDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex const index)
{
    auto dataNode = std::dynamic_pointer_cast<DataNode>(data);
    auto currentData = std::dynamic_pointer_cast<DataNode>(inData(index));

    if (!dataNode)
    {
        emit dataInvalidated(index);
        currentData->reset();
        return;
    }
    currentData->setName(dataNode->name());
    propagateUpdate();
}

ReduceModel::ReduceModel()
    : FdfBlockModel(FdfType::Processor, "reduce")
{
    addPort(PortType::In, std::make_shared<FunctionNode>("xform"));
    addPort(PortType::In, std::make_shared<DataNode>("train"));
    addPort(PortType::Out, std::make_shared<DataNode>("train"));
}

void ReduceModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
