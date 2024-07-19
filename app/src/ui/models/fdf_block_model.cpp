#include "ui/models/fdf_block_model.hpp"

FdfBlockModel::FdfBlockModel(FdfType type, const QString &name, const QString &functionName)
    : NodeDelegateModel(),
      m_type(type),
      m_name(name),
      m_functionName(functionName)
{
    // init empty port map
    auto portTypes = {PortType::In, PortType::Out, PortType::None};
    for (auto pType : portTypes)
        m_ports[pType] = {};
}

unsigned int FdfBlockModel::nPorts(PortType const portType) const
{
    return m_ports.at(portType).size();
}

NodeDataType FdfBlockModel::dataType(PortType const portType, PortIndex const portIndex) const
{
    auto ports = m_ports.at(portType);
    if (ports.size() <= portIndex)
        return NodeDataType();
    return ports.at(portIndex)->type();
}

std::shared_ptr<NodeData> FdfBlockModel::outData(PortIndex const port)
{
    auto ports = m_ports.at(PortType::Out);
    if (ports.size() <= port)
        return std::shared_ptr<NodeData>();
    return ports.at(port);
}

void FdfBlockModel::setInData(std::shared_ptr<NodeData>, PortIndex const)
{
}

QWidget *FdfBlockModel::embeddedWidget()
{
    return nullptr;
}

void FdfBlockModel::setCaption(const QString &caption)
{
    if (m_caption == caption)
        return;
    m_caption = caption;
}

PortIndex FdfBlockModel::addPort(PortType const portType, std::shared_ptr<NodeData> port)
{
    PortIndex i = m_ports[portType].size();
    m_ports[portType].push_back(port);
    return i;
}
