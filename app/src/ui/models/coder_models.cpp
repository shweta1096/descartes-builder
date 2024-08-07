#include "ui/models/coder_models.hpp"

#include "ui/models/function_names.hpp"

TransformDataModel::TransformDataModel()
    : FdfBlockModel(FdfType::Coder, "transform", coder_function::TRANSFORM_DATA)
{
    addPort<DataNode>(PortType::In);
    addPort<DataNode>(PortType::In, "parameters");
    addPort<FunctionNode>(PortType::Out, "transform");
    addPort<FunctionNode>(PortType::Out, "inv_transform");
}