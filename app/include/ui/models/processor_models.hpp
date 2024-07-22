#pragma once

#include "fdf_block_model.hpp"

class ProcessorSplitDataModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ProcessorSplitDataModel();
    void setInData(std::shared_ptr<NodeData> data, PortIndex const index) override;
};

class ReduceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ReduceModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
};