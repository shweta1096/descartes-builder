#pragma once

#include "fdf_block_model.hpp"

class ProcessorSplitDataModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ProcessorSplitDataModel();
    // need to be true even if parameters is empty
    virtual bool hasParameters() const override { return true; }
    virtual std::unordered_map<QString, QString> getParameters() const override;
    std::optional<int> getRandomState() const { return m_randomState; }
    std::optional<int> getSplitTime() const { return m_splitTime; }
    void setRandomState(const std::optional<int> &randomState) { m_randomState = randomState; }
    void setSplitTime(const std::optional<int> &splitTime) { m_splitTime = splitTime; }

private:
    inline static const QString RANDOM_STATE = "random_state";
    inline static const QString SPLIT_TIME = "split_time";

    std::optional<int> m_randomState;
    std::optional<int> m_splitTime;
};

class ReduceModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ReduceModel();
};

class ScoreModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ScoreModel();
};

class LoadMatModel : public FdfBlockModel
{
    Q_OBJECT
public:
    LoadMatModel();
    virtual std::unordered_map<QString, QString> getParameters() const override;
    QString getDataPath() const { return m_dataPath; }
    void setDataPath(const QString &dataPath) { m_dataPath = dataPath; }

private:
    inline static const QString DATA_PATH = "data_path";

    QString m_dataPath;
};