#include "ui/models/coder_models.hpp"

XformDataModel::XformDataModel()
    : FdfBlockModel(FdfType::Coder, "xform", "xform_data")
{
    addInPort(std::make_unique<DataNode>("train"));
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<FunctionNode>("xform"));
    addOutPort(std::make_shared<FunctionNode>("inv_xform"));
}