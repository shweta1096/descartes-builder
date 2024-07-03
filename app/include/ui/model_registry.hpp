#pragma once

#include "ui/models/AdditionModel.hpp"
#include "ui/models/DivisionModel.hpp"
#include "ui/models/MultiplicationModel.hpp"
#include "ui/models/NumberDisplayDataModel.hpp"
#include "ui/models/NumberSourceDataModel.hpp"
#include "ui/models/SubtractionModel.hpp"

#include "ui/models/test_model.hpp"

namespace model_registry
{
    static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
    {
        auto ret = std::make_shared<NodeDelegateModelRegistry>();
        ret->registerModel<NumberSourceDataModel>("Sources");
        ret->registerModel<NumberDisplayDataModel>("Displays");
        ret->registerModel<AdditionModel>("Operators");
        ret->registerModel<SubtractionModel>("Operators");
        ret->registerModel<MultiplicationModel>("Operators");
        ret->registerModel<DivisionModel>("Operators");

        ret->registerModel<TestModel>("Test");

        return ret;
    }
}