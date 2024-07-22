#include "ui/models/trainer_models.hpp"

BasicTrainerModel::BasicTrainerModel()
    : FdfBlockModel(FdfType::Trainer, "trainer", "basic_trainer")
{
    addPort(PortType::In, std::make_shared<DataNode>("X_train"));
    addPort(PortType::In, std::make_shared<DataNode>("Y_train"));
    addPort(PortType::In, std::make_shared<DataNode>("parameters"));
    addPort(PortType::Out, std::make_shared<FunctionNode>("regressor"));
}

void BasicTrainerModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
