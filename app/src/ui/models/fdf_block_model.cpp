#include "ui/models/fdf_block_model.hpp"

FdfBlockModel::FdfBlockModel(FdfType type, const QString &name, const QString &functionName)
    : NodeDelegateModel()
    , m_type(type)
    , m_name(name)
    , m_functionName(functionName)
    , m_caption(QString("%1(%2)").arg(typeAsString(), name))
{
    updateShape();
}

unsigned int FdfBlockModel::nPorts(PortType const portType) const
{
    if (portType == PortType::In)
        return m_inPorts.size();
    if (portType == PortType::Out)
        return m_outPorts.size();
    return 0;
}

NodeDataType FdfBlockModel::dataType(PortType const portType, PortIndex const portIndex) const
{
    if (!indexCheck(portType, portIndex))
        return NodeDataType();
    if (portType == PortType::In)
        return m_inPorts.at(portIndex).first->type();
    if (portType == PortType::Out)
        return m_outPorts.at(portIndex).first->type();
    return NodeDataType();
}

std::shared_ptr<NodeData> FdfBlockModel::outData(PortIndex const index)
{
    if (!indexCheck(PortType::Out, index))
        return std::shared_ptr<NodeData>();
    return m_outPorts.at(index).first;
}

void FdfBlockModel::setInData(std::shared_ptr<NodeData> data, PortIndex const index)
{
    if (!indexCheck(PortType::In, index))
        return;
    if (!data) {
        emit dataInvalidated(index);
        m_inPorts.at(index).second = std::weak_ptr<NodeData>();
        resetPortCaption(PortType::In, index);
        return;
    }
    m_inPorts.at(index).second = data;
    setPortCaption(PortType::In, index, data->type().name);
    propagateUpdate();
}

QWidget *FdfBlockModel::embeddedWidget()
{
    return nullptr;
}

QString FdfBlockModel::portCaption(PortType portType, PortIndex portIndex) const
{
    if (!indexCheck(portType, portIndex))
        return QString();
    if (portType == PortType::In) {
        if (auto referencedPort = m_inPorts.at(portIndex).second.lock())
            return referencedPort->type().name; // if there's a referenced port, use that caption
        return m_inPorts.at(portIndex).first->type().name; // otherwise use the default
    }
    if (portType == PortType::Out)
        return m_outPorts.at(portIndex).first->type().name;
    return QString();
}

std::shared_ptr<NodeData> FdfBlockModel::portData(PortType const type, PortIndex const index)
{
    if (type == PortType::In)
        return inData(index);
    if (type == PortType::Out)
        return outData(index);
    return std::shared_ptr<NodeData>();
}

std::vector<std::shared_ptr<NodeData>> FdfBlockModel::connectedPortData(PortType const type) const
{
    std::vector<std::shared_ptr<NodeData>> result;
    if (type == PortType::None)
        return result;
    if (type == PortType::In) {
        for (auto &portPair : m_inPorts)
            if (auto block = portPair.second.lock())
                result.push_back(block);
    } else if (type == PortType::Out) {
        for (auto &portPair : m_outPorts)
            if (portPair.second) // second represents it's in use
                result.push_back(portPair.first);
    }
    return result;
}

void FdfBlockModel::outputConnectionCreated(ConnectionId const &conn)
{
    PortIndex index = conn.outPortIndex;
    if (!indexCheck(PortType::Out, index))
        return;
    m_outPorts.at(index).second = true;
}

void FdfBlockModel::outputConnectionDeleted(ConnectionId const &conn)
{
    PortIndex index = conn.outPortIndex;
    if (!indexCheck(PortType::Out, index))
        return;
    m_outPorts.at(index).second = false;
}

bool FdfBlockModel::indexCheck(PortType type, PortIndex index) const
{
    if (type == PortType::In)
        return m_inPorts.size() > index;
    if (type == PortType::Out)
        return m_outPorts.size() > index;
    return false;
}

void FdfBlockModel::propagateUpdate()
{
    for (uint i = 0; i < nPorts(PortType::Out); ++i)
        emit dataUpdated(i);
}

void FdfBlockModel::setCaption(const QString &caption)
{
    if (m_caption == caption)
        return;
    m_caption = caption;
}

bool FdfBlockModel::setPortCaption(PortType type, PortIndex index, const QString &caption)
{
    if (!indexCheck(type, index))
        return false;
    if (type == PortType::In)
        if (auto namedNode = dynamic_cast<NamedNode *>(m_inPorts.at(index).first.get())) {
            namedNode->setName(caption);
            return true;
        }
    if (type == PortType::Out)
        if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(m_outPorts.at(index).first)) {
            namedNode->setName(caption);
            return true;
        }
    return false;
}

bool FdfBlockModel::resetPortCaption(PortType type, PortIndex index)
{
    if (!indexCheck(type, index))
        return false;
    if (type == PortType::In)
        if (auto namedNode = dynamic_cast<NamedNode *>(m_inPorts.at(index).first.get())) {
            namedNode->reset();
            return true;
        }
    if (type == PortType::Out)
        if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(m_outPorts.at(index).first)) {
            namedNode->reset();
            return true;
        }
    return false;
}

std::shared_ptr<NodeData> FdfBlockModel::inData(PortIndex const index)
{
    if (!indexCheck(PortType::In, index))
        return std::shared_ptr<NodeData>();
    return m_inPorts.at(index).second.lock();
}

PortIndex FdfBlockModel::addInPort(std::unique_ptr<NodeData> port)
{
    if (!port)
        return QtNodes::InvalidPortIndex;
    PortIndex i = m_inPorts.size();
    m_inPorts.push_back({std::move(port), std::weak_ptr<NodeData>()});
    return i;
}

PortIndex FdfBlockModel::addOutPort(std::shared_ptr<NodeData> port)
{
    if (!port)
        return QtNodes::InvalidPortIndex;
    PortIndex i = m_outPorts.size();
    m_outPorts.push_back({port, false});
    return i;
}

void FdfBlockModel::updateShape()
{
    switch (m_type) {
    case FdfType::Coder:
        m_shape = NodeShape::Trapezoid;
        break;
    case FdfType::Processor:
        m_shape = NodeShape::Rectangle;
        break;
    case FdfType::Trainer:
        m_shape = NodeShape::Pentagon;
        break;
    case FdfType::Data:
    case FdfType::Output:
    default:
        m_shape = NodeShape::RoundedRectangle;
        break;
    }
}
