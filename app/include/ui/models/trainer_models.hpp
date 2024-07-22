#pragma once

#include "fdf_block_model.hpp"

class BasicTrainerModel : public FdfBlockModel
{
    Q_OBJECT
public:
    BasicTrainerModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
};