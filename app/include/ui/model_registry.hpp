#pragma once

#include "ui/models/coder_model.hpp"
#include "ui/models/processor_model.hpp"
#include "ui/models/trainer_model.hpp"

namespace model_registry
{
    static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
    {
        // All models to be used must be registered here
        auto ret = std::make_shared<NodeDelegateModelRegistry>();
        ret->registerModel<ProcessorModel>("Processor");
        ret->registerModel<CoderModel>("Coder");
        ret->registerModel<TrainerModel>("Trainer");

        return ret;
    }
}