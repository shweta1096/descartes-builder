#include "ui/models/processor_models.hpp"

#include "ui/models/function_names.hpp"

namespace {
using Plot = ScoreModel::Plot;
std::unordered_map<Plot, QString> PLOT_STRING = {
    {Plot::Regression, "regression"},
    {Plot::TimeSeries, "time_series"},
};
} // namespace

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

void SplitDataModel::onDataInputSet(const PortIndex &index)
{
    QUuid target;
    if (auto data = castedPort<DataNode>(PortType::In, index))
        target = data->typeId();
    setOutputTypeId(index, target);
}

void SplitDataModel::onDataInputReset(const PortIndex &index)
{
    setOutputTypeId(index, QUuid());
}

void SplitDataModel::setOutputTypeId(const PortIndex &inputIndex, const QUuid &typeId)
{
    // hardcoded wiring from input to output
    std::unordered_map<PortIndex, std::vector<PortIndex>> relations = {{0, {0, 1}}, {1, {2, 3}}};
    for (auto &relatedIndex : relations.at(inputIndex)) {
        auto port = castedPort<DataNode>(PortType::Out, relatedIndex);
        port->setTypeId(typeId);
    }
}

ExternalProcessorModel::ExternalProcessorModel()
    : ProcessorModel("reduce", processor_function::PROCESSOR)
{
    addPort<FunctionNode>(PortType::In);
}

bool ExternalProcessorModel::canConnect(PortType portType, PortIndex index, QUuid typeId) const
{
    if (m_signature.isEmpty())
        return true;
    if (index == 0)
        return true; // function input, always allow function to be connected, may want to disconnect data after this
    return m_signature.inputs.at(--index) == typeId;
}

void ExternalProcessorModel::onFunctionInputSet(const PortIndex &index)
{
    if (auto function = castedPort<FunctionNode>(PortType::In, 0))
        m_signature = function->signature();
    updateDataPortsWithSignature();
}

void ExternalProcessorModel::onFunctionInputReset(const PortIndex &index)
{
    m_signature = FunctionNode::Signature();
    updateDataPortsWithSignature();
}

bool ExternalProcessorModel::portNumberModifiable(const PortType &portType) const
{
    // port number will be modified by function instead
    return false;
}

void ExternalProcessorModel::updateDataPortsWithSignature()
{
    setPortNumber<DataNode>(PortType::In, m_signature.inputs.size());
    setPortNumber<DataNode>(PortType::Out, m_signature.outputs.size());
    for (int i = 0; i < m_signature.outputs.size(); ++i) {
        auto port = castedPort<DataNode>(PortType::Out, i);
        port->setTypeId(m_signature.outputs.at(i));
    }
}

ScoreModel::ScoreModel()
    : ProcessorModel("score", processor_function::SCORE)
{
    addPort<DataNode>(PortType::In, "Y_test");
    addPort<DataNode>(PortType::In, "Y_pred");
    addPort<DataNode>(PortType::Out, "nrmse");
    addPort<DataNode>(PortType::Out, "r2");

    setPlot(Plot::Regression);

    for (auto &port : allOutData<DataNode>())
        port->setTypeId(QUuid::createUuid());
}

std::unordered_map<QString, QString> ScoreModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    if (m_plot)
        result[PLOT] = PLOT_STRING.at(m_plot.value());
    return result;
}

std::unordered_map<QString, QMetaType::Type> ScoreModel::getParameterSchema() const
{
    std::unordered_map<QString, QMetaType::Type> schema;
    schema[PLOT] = QMetaType::QString;
    return schema;
}

QStringList ScoreModel::getParameterOptions(const QString &key) const
{
    QStringList result;
    if (key == PLOT) {
        for (auto pair : PLOT_STRING)
            result << pair.second;
    }
    return result;
}

void ScoreModel::setParameter(const QString &key, const QString &value) {}
