#include "ui/models/coder_models.hpp"

#include "ui/models/function_names.hpp"

namespace {
using Process = CoderModel::Process;
std::unordered_map<Process, QString> PROCESS_STRING = {
    {Process::None, "none"},
    {Process::Std, "std"},
    {Process::Pca, "pca"},
    {Process::PcaStd, "pca_std"},
    {Process::StdPca, "std_pca"},
};
} // namespace

CoderModel::CoderModel()
    : FdfBlockModel(FdfType::Coder, "transform", coder_function::TRANSFORM_DATA)
{
    addPort<DataNode>(PortType::In);
    addPort<FunctionNode>(PortType::Out, "encode");
    addPort<FunctionNode>(PortType::Out, "decode");

    setProcess(Process::StdPca);
    setRandomState(0);
}

std::unordered_map<QString, QString> CoderModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    result[PROCESS] = PROCESS_STRING.at(m_process);
    if (m_randomState)
        result[RANDOM_STATE] = QString::number(m_randomState.value());
    return result;
}

std::unordered_map<QString, QMetaType::Type> CoderModel::getParameterSchema() const
{
    std::unordered_map<QString, QMetaType::Type> schema;
    schema[PROCESS] = QMetaType::QString;
    schema[RANDOM_STATE] = QMetaType::Int;
    return schema;
}

QStringList CoderModel::getParameterOptions(const QString &key) const
{
    QStringList result;
    if (key == PROCESS) {
        for (auto pair : PROCESS_STRING)
            result << pair.second;
    }
    return result;
}

void CoderModel::setParameter(const QString &key, const QString &value)
{
    if (key == PROCESS) {
        for (auto pair : PROCESS_STRING)
            if (pair.second == value)
                setProcess(pair.first);
    } else if (key == RANDOM_STATE) {
        setRandomState(value.toInt());
    }
}

bool CoderModel::portNumberModifiable(const PortType &portType) const
{
    return portType == PortType::In;
}

uint CoderModel::minModifiablePorts(const PortType &portType, const QString &typeId) const
{
    if (portType == PortType::In)
        if (typeId == constants::DATA_PORT_ID)
            return 1;
    if (portType == PortType::Out)
        if (typeId == constants::FUNCTION_PORT_ID)
            return 1;
    return 0;
}

void CoderModel::setInputPortNumber(uint num)
{
    setPortNumber<DataNode>(PortType::In, num);
}
