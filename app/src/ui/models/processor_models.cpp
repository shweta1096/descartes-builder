#include "ui/models/processor_models.hpp"

#include "ui/models/function_names.hpp"

ProcessorSplitDataModel::ProcessorSplitDataModel()
    : FdfBlockModel(FdfType::Processor, "split_data", processor_function::SPLIT_DATA)
{
    addPort<DataNode>(PortType::In, "X");
    addPort<DataNode>(PortType::In, "Y");
    addPort<DataNode>(PortType::In, "parameters");
    addPort<DataNode>(PortType::Out, "X_train");
    addPort<DataNode>(PortType::Out, "X_test");
    addPort<DataNode>(PortType::Out, "Y_train");
    addPort<DataNode>(PortType::Out, "Y_test");
}

ReduceModel::ReduceModel()
    : FdfBlockModel(FdfType::Processor, "reduce", processor_function::PROCESSOR)
{
    addPort<FunctionNode>(PortType::In);
    addPort<DataNode>(PortType::In);
    addPort<DataNode>(PortType::Out);
}

ScoreModel::ScoreModel()
    : FdfBlockModel(FdfType::Processor, "score", processor_function::SCORE)
{
    addPort<DataNode>(PortType::In, "Y_test");
    addPort<DataNode>(PortType::In, "Y_pred");
    addPort<DataNode>(PortType::Out, "nrmse");
    addPort<DataNode>(PortType::Out, "r2");
}

LoadMatModel::LoadMatModel()
    : FdfBlockModel(FdfType::Processor, "load_mat", processor_function::LOAD_MAT)
{
    addPort<DataNode>(PortType::In, "parameters");
    addPort<DataNode>(PortType::Out, "x");
    addPort<DataNode>(PortType::Out, "y");
}
