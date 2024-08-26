#pragma once

#include "fdf_block_model.hpp"

class CoderModel : public FdfBlockModel
{
    Q_OBJECT
public:
    enum Process {
        None,
        Std,
        Pca,
        PcaStd,
        StdPca,
    };

    CoderModel();
    virtual std::unordered_map<QString, QString> getParameters() const override;
    virtual std::unordered_map<QString, QMetaType::Type> getParameterSchema() const override;
    virtual QStringList getParameterOptions(const QString &key) const override;
    virtual void setParameter(const QString &key, const QString &value) override;
    virtual bool portNumberModifiable(const PortType &portType) const override;
    virtual uint minModifiablePorts(const PortType &portType, const QString &typeId) const override;
    Process getProcess() const { return m_process; }
    std::optional<int> getRandomState() const { return m_randomState; }
    void setRandomState(const std::optional<int> &randomState) { m_randomState = randomState; }
    void setProcess(const Process &dataXform) { m_process = dataXform; }

public slots:
    virtual void setInputPortNumber(uint num) override;

private:
    inline static const QString PROCESS = "data_xform";
    inline static const QString RANDOM_STATE = "random_state";

    Process m_process;
    std::optional<int> m_randomState;
};