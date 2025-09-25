#include "ui/models/composer_models.hpp"
#include "ui/models/function_names.hpp"

ComposerModel::ComposerModel()
    : FdfBlockModel(FdfType::Composer, composer_names::FUNCTION_COMPOSER)
{
    addPort<FunctionNode>(PortType::In);
    addPort<FunctionNode>(PortType::Out, "f_out");
}

bool ComposerModel::portNumberModifiable(const PortType &portType) const
{
    return portType == PortType::In;
}

uint ComposerModel::minModifiablePorts(const PortType &portType, const QString &typeId) const
{
    return 1;
}

void ComposerModel::setInputPortNumber(uint num)
{
    setPortNumber<FunctionNode>(PortType::In, num);
}

bool ComposerModel::canConnect(ConnectionInfo &connInfo) const
{
    auto inSignature = connInfo.receivedSignature;
    auto inIndex = connInfo.inIndex;

    // Check function signature compatibility with previous input
    if (inIndex > 0) {
        if (auto prevFunc = castedPort<FunctionNode>(PortType::In, inIndex - 1)) {
            if (prevFunc->hasValidSignature()) {
                if (prevFunc->signature().outputs != inSignature->inputs) {
                    connInfo.expectedSignature = std::make_shared<Signature>(prevFunc->signature());
                    return warnInvalidConnection(connInfo, constants::SIGNATURE_MISMATCH_PREVFUNC);
                }
            }
        }
    }

    // Check function signature compatibility with next input
    if (inIndex < nPorts(PortType::In) - 1) {
        if (auto nextFunc = castedPort<FunctionNode>(PortType::In, inIndex + 1)) {
            if (nextFunc->hasValidSignature()) {
                if (inSignature->outputs != nextFunc->signature().inputs) {
                    connInfo.expectedSignature = std::make_shared<Signature>(nextFunc->signature());
                    return warnInvalidConnection(connInfo, constants::SIGNATURE_MISMATCH_NEXTFUNC);
                }
            }
        }
    }
    return true;
}

void ComposerModel::onFunctionInputSet(const PortIndex &index)
{
    // re-compute output port signature based on the new input function
    updateFunctionPortWithSignature();
}

void ComposerModel::onFunctionInputReset(const PortIndex &index)
{
    // re-compute output port signature based on the deleted input function
    updateFunctionPortWithSignature();
}

void ComposerModel::updateFunctionPortWithSignature()
{
    std::vector<Signature> sigs;
    for (int i = 0; i < nPorts(PortType::In); ++i)
        if (auto func = castedPort<FunctionNode>(PortType::In, i))
            sigs.push_back(func->signature());

    if (auto outPort = castedPort<FunctionNode>(PortType::Out, 0)) {
        if (sigs.empty()) {
            outPort->setSignature(Signature());
            return;
        }

        Signature composed;
        composed.inputs = sigs.front().inputs;
        composed.outputs = sigs.back().outputs;
        outPort->setSignature(composed);
    }
}