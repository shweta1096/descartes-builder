#include "ui/models/trainer_models.hpp"

BasicTrainerModel::BasicTrainerModel()
    : FdfBlockModel(FdfType::Trainer, "trainer", "basic_trainer")
{
    addInPort(std::make_unique<DataNode>("X_train"));
    addInPort(std::make_unique<DataNode>("Y_train"));
    addInPort(std::make_unique<DataNode>("parameters"));
    addOutPort(std::make_shared<FunctionNode>("regressor"));
}
