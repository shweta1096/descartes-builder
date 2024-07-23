#include "ui/models/processor_models.hpp"

ProcessorSplitDataModel::ProcessorSplitDataModel()
    : FdfBlockModel(FdfType::Processor, "split_data", "split_data")
{
    addInPort(std::make_unique<DataNode>());
    addInPort(std::make_unique<DataNode>());
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<DataNode>("X_train"));
    addOutPort(std::make_shared<DataNode>("X_test"));
    addOutPort(std::make_shared<DataNode>("Y_train"));
    addOutPort(std::make_shared<DataNode>("Y_test"));
}

ReduceModel::ReduceModel()
    : FdfBlockModel(FdfType::Processor, "reduce")
{
    addInPort(std::make_unique<FunctionNode>("xform"));
    addInPort(std::make_unique<DataNode>("train"));
    addOutPort(std::make_shared<DataNode>("train"));
}
