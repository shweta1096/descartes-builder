#pragma once

#include "fdf_block_model.hpp"

class ProcessorSplitDataModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ProcessorSplitDataModel();
};

class ReduceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ReduceModel();
};