#pragma once

#include "fdf_block_model.hpp"

class ProcessorModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ProcessorModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
};