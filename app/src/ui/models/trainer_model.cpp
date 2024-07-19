#include "ui/models/trainer_model.hpp"

TrainerModel::TrainerModel()
    : FdfBlockModel(FdfType::Trainer, "trainer", "basic_trainer")
{
    addPort(PortType::In, std::make_shared<DataNode>());
    addPort(PortType::In, std::make_shared<DataNode>());
    addPort(PortType::Out, std::make_shared<FunctionNode>());
}

void TrainerModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}
