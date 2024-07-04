#include "ui/models/fdf_block_model.hpp"

FdfBlockModel::FdfBlockModel(const QString &caption, const QString &name)
    : NodeDelegateModel(),
      m_caption(caption),
      m_name(name)
{
    // init empty port map
    auto portTypes = {PortType::In, PortType::Out, PortType::None};
    for (auto pType : portTypes)
        m_ports[pType] = {};
}

QString FdfBlockModel::caption() const
{
    return m_caption;
}
QString FdfBlockModel::name() const
{
    return m_name;
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

PortIndex FdfBlockModel::addPort(PortType const portType, std::shared_ptr<NodeData> port)
{
    PortIndex i = m_ports[portType].size();
    m_ports[portType].push_back(port);
    return i;
}
