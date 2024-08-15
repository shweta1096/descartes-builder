#include "ui/models/coder_models.hpp"

#include "ui/models/function_names.hpp"

namespace {
using Process = TransformDataModel::Process;
std::unordered_map<Process, QString> PROCESS_STRING = {
    {Process::None, "none"},
    {Process::Std, "std"},
    {Process::Pca, "pca"},
    {Process::PcaStd, "pca_std"},
    {Process::StdPca, "std_pca"},
};
} // namespace

TransformDataModel::TransformDataModel()
    : FdfBlockModel(FdfType::Coder, "transform", coder_function::TRANSFORM_DATA)
{
    addPort<DataNode>(PortType::In);
    addPort<FunctionNode>(PortType::Out, "transform");
    addPort<FunctionNode>(PortType::Out, "inv_transform");

    // hardcode parameter value until editor is implemented
    setProcess(Process::StdPca);
    setRandomState(0);
}

std::unordered_map<QString, QString> TransformDataModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    result[PROCESS] = PROCESS_STRING.at(m_process);
    if (m_randomState)
        result[RANDOM_STATE] = QString::number(m_randomState.value());
    return result;
}
