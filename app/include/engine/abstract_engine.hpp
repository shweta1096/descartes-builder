#pragma once

#include <QString>
#include <QStringList>

#include <QtNodes/Definitions>

class CustomGraph;

class AbstractEngine
{
public:
    virtual ~AbstractEngine() {}
    virtual bool execute(CustomGraph *graph, const QString &name = QString()) = 0;
    QString getExecutionError() const { return m_executionError; }

    virtual bool validityCheck(CustomGraph *graph) = 0;
    QStringList getValidityWarnings() const { return m_validityWarnings; }

    virtual QVariant getNodeOutput(CustomGraph *graph, QtNodes::NodeId id) = 0;

protected:
    void setExecutionError(const QString &error) { m_executionError = error; }
    void setValidityWarnings(const QStringList &warnings) { m_validityWarnings = warnings; }
    void appendValidityWarning(const QString &warning) { m_validityWarnings.append(warning); }

private:
    QString m_executionError;
    QStringList m_validityWarnings;
};
