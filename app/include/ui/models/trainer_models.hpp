#pragma once

#include "fdf_block_model.hpp"

class BasicTrainerModel : public FdfBlockModel
{
    Q_OBJECT
public:
    enum Model {
        Mlp,
        Mlp1,
        Mlp2,
        Lr,
        Dt,
        Svr,
    };

    BasicTrainerModel();
    virtual std::unordered_map<QString, QString> getParameters() const override;
    Model getModel() const { return m_model; }
    std::optional<int> getRandomState() const { return m_randomState; }
    std::optional<std::pair<int, int>> getHiddenLayerSizes() const { return m_hiddenLayerSizes; }
    void setModel(const Model &model) { m_model = model; }
    void setRandomState(const std::optional<int> &randomState) { m_randomState = randomState; }
    void setHiddenLayerSizes(const std::optional<std::pair<int, int>> &hiddenLayerSizes)
    {
        m_hiddenLayerSizes = hiddenLayerSizes;
    }

private:
    inline static const QString RANDOM_STATE = "random_state";
    inline static const QString MODEL = "model";
    inline static const QString HIDDEN_LAYER_SIZES = "hidden_layer_sizes";

    Model m_model;
    std::optional<int> m_randomState;
    // only for mlp2
    std::optional<std::pair<int, int>> m_hiddenLayerSizes;
};