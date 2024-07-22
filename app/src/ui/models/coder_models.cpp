#include "ui/models/coder_models.hpp"

XformDataModel::XformDataModel()
    : FdfBlockModel(FdfType::Coder, "xform", "xform_data")
{
    addPort(PortType::In, std::make_shared<DataNode>("train"));
    addPort(PortType::In, std::make_shared<DataNode>("parameters"));
    addPort(PortType::Out, std::make_shared<FunctionNode>("xform"));
    addPort(PortType::Out, std::make_shared<FunctionNode>("inv_xform"));
}

void XformDataModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
