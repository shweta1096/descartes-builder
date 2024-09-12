#pragma once

#include "fdf_block_model.hpp"

class ProcessorModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ProcessorModel(const QString &name, const QString &functionName);
    virtual bool portNumberModifiable(const PortType &portType) const override;
    virtual uint minModifiablePorts(const PortType &portType, const QString &typeId) const override;

public slots:
    virtual void setInputPortNumber(uint num) override;
    virtual void setOutputPortNumber(uint num) override;
};

class SplitDataModel : public ProcessorModel
{
    Q_OBJECT
public:
    SplitDataModel();
    // need to be true even if parameters is empty
    virtual bool hasParameters() const override { return true; }
    virtual std::unordered_map<QString, QString> getParameters() const override;
    virtual std::unordered_map<QString, QMetaType::Type> getParameterSchema() const override;
    virtual void setParameter(const QString &key, const QString &value) override;
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

class ExternalProcessorModel : public ProcessorModel
{
    Q_OBJECT
public:
    ExternalProcessorModel();
};

class ScoreModel : public ProcessorModel
{
    Q_OBJECT
public:
    enum Plot {
        Regression,
        TimeSeries,
    };

    ScoreModel();
    virtual std::unordered_map<QString, QString> getParameters() const override;
    virtual std::unordered_map<QString, QMetaType::Type> getParameterSchema() const override;
    virtual QStringList getParameterOptions(const QString &key) const override;
    virtual void setParameter(const QString &key, const QString &value) override;
    std::optional<Plot> getPlot() const { return m_plot; }
    void setPlot(const std::optional<Plot> &plot) { m_plot = plot; }

private:
    inline static const QString PLOT = "plot";

    std::optional<Plot> m_plot;
};