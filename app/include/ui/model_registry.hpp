#pragma once

#include <QtNodes/NodeDelegateModelRegistry>

#include "ui/models/coder_models.hpp"
#include "ui/models/io_models.hpp"
#include "ui/models/processor_models.hpp"
#include "ui/models/trainer_models.hpp"

namespace model_registry {
static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels()
{
    // All models to be used must be registered here
    auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    ret->registerModel<DataSourceModel>("IO");
    ret->registerModel<FuncOutModel>("IO");
    ret->registerModel<DataOutModel>("IO");
    ret->registerModel<GraphModel>("IO");
    ret->registerModel<SplitDataModel>("Processor");
    ret->registerModel<ExternalProcessorModel>("Processor");
    ret->registerModel<ScoreModel>("Processor");
    ret->registerModel<CoderModel>("Coder");
    ret->registerModel<BasicTrainerModel>("Trainer");

    return ret;
}
} // namespace model_registry