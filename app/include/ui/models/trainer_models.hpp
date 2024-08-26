#pragma once

#include "fdf_block_model.hpp"

class TrainerModel : public FdfBlockModel
{
    Q_OBJECT
public:
    TrainerModel(const QString &name, const QString &functionName);
    virtual bool portNumberModifiable(const PortType &portType) const override;
    virtual uint minModifiablePorts(const PortType &portType, const QString &typeId) const override;

public slots:
    virtual void setInputPortNumber(uint num) override;
};

class BasicTrainerModel : public TrainerModel
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
    virtual std::unordered_map<QString, QMetaType::Type> getParameterSchema() const override;
    virtual QStringList getParameterOptions(const QString &key) const override;
    virtual void setParameter(const QString &key, const QString &value) override;
    Model getModel() const { return m_model; }
    std::optional<int> getRandomState() const { return m_randomState; }
    std::optional<std::vector<int>> getHiddenLayerSizes() const { return m_hiddenLayerSizes; }
    void setModel(const Model &model);
    void setRandomState(const std::optional<int> &randomState) { m_randomState = randomState; }
    void setHiddenLayerSizes(const std::optional<std::vector<int>> &hiddenLayerSizes)
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
    std::optional<std::vector<int>> m_hiddenLayerSizes;
};