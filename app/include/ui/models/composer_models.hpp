#pragma once

#include "fdf_block_model.hpp"

class ComposerModel : public FdfBlockModel
{
    Q_OBJECT
public:
    ComposerModel();
    virtual bool portNumberModifiable(const PortType &portType) const override;
    virtual bool canConnect(ConnectionInfo &connInfo) const override;
    virtual uint minModifiablePorts(const PortType &portType, const QString &typeId) const override;

public slots:
    virtual void setInputPortNumber(uint num) override;
    virtual void onFunctionInputSet(const PortIndex &index) override;
    virtual void onFunctionInputReset(const PortIndex &index) override;

private:
    void updateFunctionPortWithSignature();
    Signature m_signature;
};