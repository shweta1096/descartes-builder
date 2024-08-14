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

    // hardcode parameter value until editor is implemented
    setRandomState(0);
}

std::unordered_map<QString, QString> ProcessorSplitDataModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    if (m_randomState)
        result[RANDOM_STATE] = QString::number(m_randomState.value());
    if (m_splitTime)
        result[SPLIT_TIME] = QString::number(m_splitTime.value());
    return result;
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

std::unordered_map<QString, QString> LoadMatModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    result[DATA_PATH] = '\"' + m_dataPath + '\"';
    return result;
}
