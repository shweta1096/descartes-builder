#include "ui/models/trainer_models.hpp"

#include "ui/models/function_names.hpp"

BasicTrainerModel::BasicTrainerModel()
    : FdfBlockModel(FdfType::Trainer, "trainer", trainer_function::BASIC_TRAINER)
{
    addPort<DataNode>(PortType::In, "X");
    addPort<DataNode>(PortType::In, "Y");
    addPort<DataNode>(PortType::In, "parameters");
    addPort<FunctionNode>(PortType::Out, "predict");
}
