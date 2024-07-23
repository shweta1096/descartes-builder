#include "ui/models/trainer_models.hpp"

#include "ui/models/function_names.hpp"

BasicTrainerModel::BasicTrainerModel()
    : FdfBlockModel(FdfType::Trainer, "trainer", trainer_function::BASIC_TRAINER)
{
    addInPort(std::make_unique<DataNode>("X"));
    addInPort(std::make_unique<DataNode>("Y"));
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<FunctionNode>("predict"));
}
