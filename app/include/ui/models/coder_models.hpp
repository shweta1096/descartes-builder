#pragma once

#include "fdf_block_model.hpp"

class TransformDataModel : public FdfBlockModel
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

    TransformDataModel();
    virtual std::unordered_map<QString, QString> getParameters() const override;
    Process getProcess() const { return m_process; }
    std::optional<int> getRandomState() const { return m_randomState; }
    void setRandomState(const std::optional<int> &randomState) { m_randomState = randomState; }
    void setProcess(const Process &dataXform) { m_process = dataXform; }

private:
    inline static const QString PROCESS = "data_xform";
    inline static const QString RANDOM_STATE = "random_state";

    Process m_process;
    std::optional<int> m_randomState;
};