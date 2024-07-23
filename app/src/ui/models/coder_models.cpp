#include "ui/models/coder_models.hpp"

#include "ui/models/function_names.hpp"

TransformDataModel::TransformDataModel()
    : FdfBlockModel(FdfType::Coder, "transform", coder_function::TRANSFORM_DATA)
{
    addInPort(std::make_unique<DataNode>());
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<FunctionNode>("transform"));
    addOutPort(std::make_shared<FunctionNode>("inv_transform"));
}