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
        ret->registerModel<ProcessorModel>("Processors");
        ret->registerModel<CoderModel>("Coders");
        ret->registerModel<TrainerModel>("Trainers");

        return ret;
    }
}