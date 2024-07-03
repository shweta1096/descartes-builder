#pragma once

#include "ui/models/coder_model.hpp"

namespace model_registry
{
    static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
    {
        // All models to be used must be registered here
        auto ret = std::make_shared<NodeDelegateModelRegistry>();
        ret->registerModel<CoderModel>("Coder");

        return ret;
    }
}