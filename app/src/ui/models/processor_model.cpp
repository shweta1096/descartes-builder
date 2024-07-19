#include "ui/models/processor_model.hpp"

ProcessorModel::ProcessorModel()
    : FdfBlockModel(FdfType::Processor, "reduce")
{
    addPort(PortType::In, std::make_shared<DataNode>());
    addPort(PortType::In, std::make_shared<FunctionNode>());
    addPort(PortType::Out, std::make_shared<DataNode>());
}

void ProcessorModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
