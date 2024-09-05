#include "ui/models/processor_models.hpp"

#include "ui/models/function_names.hpp"

ProcessorModel::ProcessorModel(const QString &name, const QString &functionName)
    : FdfBlockModel(FdfType::Processor, name, functionName)
{}

bool ProcessorModel::portNumberModifiable(const PortType &portType) const
{
    return true;
}

uint ProcessorModel::minModifiablePorts(const PortType &portType, const QString &typeId) const
{
    if (portType == PortType::In)
        if (typeId == constants::DATA_PORT_ID)
            return 1;
    if (portType == PortType::Out)
        if (typeId == constants::DATA_PORT_ID)
            return 1;
    return 0;
}

void ProcessorModel::setInputPortNumber(uint num)
{
    setPortNumber<DataNode>(PortType::In, num);
}

void ProcessorModel::setOutputPortNumber(uint num)
{
    setPortNumber<DataNode>(PortType::Out, num);
}

SplitDataModel::SplitDataModel()
    : ProcessorModel("split_data", processor_function::SPLIT_DATA)
{
    addPort<DataNode>(PortType::In, "X");
    addPort<DataNode>(PortType::In, "Y");
    addPort<DataNode>(PortType::Out, "X_train");
    addPort<DataNode>(PortType::Out, "X_test");
    addPort<DataNode>(PortType::Out, "Y_train");
    addPort<DataNode>(PortType::Out, "Y_test");

    setRandomState(0);
}

std::unordered_map<QString, QString> SplitDataModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    if (m_randomState)
        result[RANDOM_STATE] = QString::number(m_randomState.value());
    if (m_splitTime)
        result[SPLIT_TIME] = QString::number(m_splitTime.value());
    return result;
}

std::unordered_map<QString, QMetaType::Type> SplitDataModel::getParameterSchema() const
{
    std::unordered_map<QString, QMetaType::Type> schema;
    schema[RANDOM_STATE] = QMetaType::Int;
    schema[SPLIT_TIME] = QMetaType::Int;
    return schema;
}

void SplitDataModel::setParameter(const QString &key, const QString &value)
{
    if (key == RANDOM_STATE) {
        setRandomState(value.toInt());
    } else if (key == SPLIT_TIME) {
        setSplitTime(value.toInt());
    }
}

ExternalProcessorModel::ExternalProcessorModel()
    : ProcessorModel("reduce", processor_function::PROCESSOR)
{
    addPort<FunctionNode>(PortType::In);
    addPort<DataNode>(PortType::In);
    addPort<DataNode>(PortType::Out);
}

ScoreModel::ScoreModel()
    : ProcessorModel("score", processor_function::SCORE)
{
    addPort<DataNode>(PortType::In, "Y_test");
    addPort<DataNode>(PortType::In, "Y_pred");
    addPort<DataNode>(PortType::Out, "nrmse");
    addPort<DataNode>(PortType::Out, "r2");
}