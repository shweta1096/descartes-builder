#pragma once

#include "fdf_block_model.hpp"

class XformDataModel : public FdfBlockModel
{
    Q_OBJECT
public:
    XformDataModel();
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;
};