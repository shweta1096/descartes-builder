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

TrainerModel::TrainerModel(const QString &name, const QString &functionName)
    : FdfBlockModel(FdfType::Trainer, name, functionName)
    , m_signature({{QUuid()}, {QUuid()}})
{}

bool TrainerModel::portNumberModifiable(const PortType &portType) const
{
    return portType == PortType::In;
}

uint TrainerModel::minModifiablePorts(const PortType &portType, const QString &typeId) const
{
    if (portType == PortType::In)
        if (typeId == constants::DATA_PORT_ID)
            return 2;
    if (portType == PortType::Out)
        if (typeId == constants::FUNCTION_PORT_ID)
            return 1;
    return 0;
}

void TrainerModel::setInputPortNumber(uint num)
{
    setPortNumber<DataNode>(PortType::In, num);
}

void TrainerModel::setTrainerInputNumber(uint num)
{
    if (m_signature.inputs.size() == num)
        return;
    m_signature.inputs.resize(num);
    setInputPortNumber(m_signature.inputs.size() + m_signature.outputs.size());
    updateSignature();
}

void TrainerModel::setTrainerOutputNumber(uint num)
{
    if (m_signature.outputs.size() == num)
        return;
    m_signature.outputs.resize(num);
    setInputPortNumber(m_signature.inputs.size() + m_signature.outputs.size());
    updateSignature();
}

void TrainerModel::onDataInputSet(const PortIndex &index)
{
    Q_UNUSED(index);
    updateSignature();
}

void TrainerModel::onDataInputReset(const PortIndex &index)
{
    m_signature.inputs.at(index) = QUuid();
    if (auto function = castedPort<FunctionNode>(PortType::Out, 0))
        function->setSignature(m_signature);
}

void TrainerModel::updateSignature()
{
    std::vector<QUuid> inputTypeIds;
    for (int i = 0; i < nPorts(PortType::In); ++i) {
        QUuid typeId;
        if (auto data = castedPort<DataNode>(PortType::In, i))
            typeId = data->typeId();
        if (i < m_signature.inputs.size())
            m_signature.inputs.at(i) = typeId;
        else
            m_signature.outputs.at(i - m_signature.inputs.size()) = typeId;
    }
    if (auto function = castedPort<FunctionNode>(PortType::Out, 0))
        function->setSignature(m_signature);
}

BasicTrainerModel::BasicTrainerModel()
    : TrainerModel("trainer", trainer_function::BASIC_TRAINER)
{
    addPort<DataNode>(PortType::In);
    addPort<DataNode>(PortType::In);
    addPort<FunctionNode>(PortType::Out, "predict");

    setModel(Model::Mlp);
    setRandomState(0);
}

std::unordered_map<QString, QString> BasicTrainerModel::getParameters() const
{
    std::unordered_map<QString, QString> result;
    result[MODEL] = MODEL_STRING.at(m_model);
    if (m_randomState)
        result[RANDOM_STATE] = QString::number(m_randomState.value());
    if (m_model == Model::Mlp2 && m_hiddenLayerSizes) {
        QStringList sizes;
        for (auto &size : m_hiddenLayerSizes.value())
            sizes << QString::number(size);
        result[HIDDEN_LAYER_SIZES] = sizes.join(", ");
    }
    return result;
}

std::unordered_map<QString, QMetaType::Type> BasicTrainerModel::getParameterSchema() const
{
    std::unordered_map<QString, QMetaType::Type> schema;
    schema[RANDOM_STATE] = QMetaType::Int;
    schema[MODEL] = QMetaType::QString;
    schema[HIDDEN_LAYER_SIZES] = QMetaType::QVector2D;
    return schema;
}

QStringList BasicTrainerModel::getParameterOptions(const QString &key) const
{
    QStringList result;
    if (key == MODEL) {
        for (auto pair : MODEL_STRING)
            result << pair.second;
    }
    return result;
}

void BasicTrainerModel::setParameter(const QString &key, const QString &value)
{
    if (key == RANDOM_STATE)
        setRandomState(value.toInt());
    else if (key == MODEL) {
        for (auto pair : MODEL_STRING)
            if (pair.second == value) {
                setModel(pair.first);
                break;
            }
    } else if (key == HIDDEN_LAYER_SIZES) {
        std::vector<int> result;
        auto sizes = value.split(", ");
        for (auto size : sizes)
            result.push_back(size.toInt());
        setHiddenLayerSizes(result);
    }
}

void BasicTrainerModel::setModel(const Model &model)
{
    if (m_model == model)
        return;
    m_model = model;
    if (m_model == Model::Mlp2)
        m_hiddenLayerSizes = std::vector<int>();
    else if (m_hiddenLayerSizes)
        m_hiddenLayerSizes = std::nullopt;
    emit contentUpdated();
}
