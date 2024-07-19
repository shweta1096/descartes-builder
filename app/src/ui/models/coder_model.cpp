#include "ui/models/coder_model.hpp"

CoderModel::CoderModel()
    : FdfBlockModel(FdfType::Coder, "xform", "xform_data")
{
    addPort(PortType::In, std::make_shared<DataNode>());
    addPort(PortType::Out, std::make_shared<FunctionNode>());
    addPort(PortType::Out, std::make_shared<FunctionNode>());
}

void CoderModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
