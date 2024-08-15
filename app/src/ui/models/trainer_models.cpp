#include "ui/models/trainer_models.hpp"

#include "ui/models/function_names.hpp"
namespace {
using Model = BasicTrainerModel::Model;
std::unordered_map<Model, QString> MODEL_STRING = {
    {Model::Mlp, "mlp"},
    {Model::Mlp1, "mlp1"},
    {Model::Mlp2, "mlp2"},
    {Model::Lr, "lr"},
    {Model::Dt, "dt"},
    {Model::Svr, "svr"},
};
} // namespace

BasicTrainerModel::BasicTrainerModel()
    : FdfBlockModel(FdfType::Trainer, "trainer", trainer_function::BASIC_TRAINER)
{
    addPort<DataNode>(PortType::In, "X");
    addPort<DataNode>(PortType::In, "Y");
    addPort<FunctionNode>(PortType::Out, "predict");

    // hardcode parameter value until editor is implemented
    setModel(Model::Mlp);
    setRandomState(0);
}

std::unordered_map<QString, QString> BasicTrainerModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    result[MODEL] = MODEL_STRING.at(m_model);
    if (m_randomState)
        result[RANDOM_STATE] = QString::number(m_randomState.value());
    if (m_model == Model::Mlp2 && m_hiddenLayerSizes)
        result[HIDDEN_LAYER_SIZES] = QString("[%1, %2]")
                                         .arg(QString::number(m_hiddenLayerSizes.value().first),
                                              QString::number(m_hiddenLayerSizes.value().second));
    return result;
}
