#include "ui/models/processor_models.hpp"

#include "ui/models/function_names.hpp"

ProcessorSplitDataModel::ProcessorSplitDataModel()
    : FdfBlockModel(FdfType::Processor, "split_data", processor_function::SPLIT_DATA)
{
    addInPort(std::make_unique<DataNode>("X"));
    addInPort(std::make_unique<DataNode>("Y"));
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<DataNode>("X_train"));
    addOutPort(std::make_shared<DataNode>("X_test"));
    addOutPort(std::make_shared<DataNode>("Y_train"));
    addOutPort(std::make_shared<DataNode>("Y_test"));
}

ReduceModel::ReduceModel()
    : FdfBlockModel(FdfType::Processor, "reduce", processor_function::PROCESSOR)
{
    addInPort(std::make_unique<FunctionNode>());
    addInPort(std::make_unique<DataNode>());
    addOutPort(std::make_shared<DataNode>());
}

ScoreModel::ScoreModel()
    : FdfBlockModel(FdfType::Processor, "score", processor_function::SCORE)
{
    addInPort(std::make_unique<DataNode>("Y_test"));
    addInPort(std::make_unique<DataNode>("T_pred"));
    addOutPort(std::make_shared<DataNode>("nrmse"));
    addOutPort(std::make_shared<DataNode>("r2"));
}

LoadMatModel::LoadMatModel()
    : FdfBlockModel(FdfType::Processor, "load_mat", processor_function::LOAD_MAT)
{
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<DataNode>("x"));
    addOutPort(std::make_shared<DataNode>("y"));
}
