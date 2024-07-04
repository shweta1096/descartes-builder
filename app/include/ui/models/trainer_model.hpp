#pragma once

#include "fdf_block_model.hpp"

class TrainerModel : public FdfBlockModel
{
    Q_OBJECT
public:
    TrainerModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
};