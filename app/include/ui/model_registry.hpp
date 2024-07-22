#pragma once

#include "ui/models/io_models.hpp"
#include "ui/models/coder_models.hpp"
#include "ui/models/processor_models.hpp"
#include "ui/models/trainer_models.hpp"

namespace model_registry
{
    static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
    {
        // All models to be used must be registered here
        auto ret = std::make_shared<NodeDelegateModelRegistry>();
        ret->registerModel<DataSourceModel>("IO");
        ret->registerModel<FuncOutModel>("IO");
        ret->registerModel<ProcessorSplitDataModel>("Processor");
        ret->registerModel<ReduceModel>("Processor");
        ret->registerModel<XformDataModel>("Coder");
        ret->registerModel<BasicTrainerModel>("Trainer");

        return ret;
    }
}