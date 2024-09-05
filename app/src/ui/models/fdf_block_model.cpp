#include "ui/models/fdf_block_model.hpp"

#include <QFileInfo>

#include "data/constants.hpp"

FdfBlockModel::FdfBlockModel(FdfType type, const QString &name, const QString &functionName)
    : NodeDelegateModel()
    , m_type(type)
    , m_name(name)
    , m_functionName(functionName)
    , m_caption(typeAsString())
{
    if (type == FdfType::Output)
        m_caption = name;
    updateStyle();
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

QString FdfBlockModel::defaultPortCaption(PortType portType, PortIndex portIndex) const
{
    if (!indexCheck(portType, portIndex))
        return QString();
    if (portType == PortType::In) {
        if (auto referencedNamedNode = std::dynamic_pointer_cast<NamedNode>(
                m_inPorts.at(portIndex).second.lock()))
            // if there's a referenced port, use that caption
            return referencedNamedNode->defaultName();
        else if (auto namedNode = dynamic_cast<NamedNode *>(m_inPorts.at(portIndex).first.get()))
            namedNode->defaultName(); // otherwise use the default
    }
    if (portType == PortType::Out)
        if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(m_outPorts.at(portIndex).first))
            return namedNode->defaultName();
    return QString();
}

QJsonObject FdfBlockModel::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();
    QJsonObject parameters;
    modelJson["name"] = m_name;
    if (!m_functionName.isEmpty())
        modelJson["function_name"] = m_functionName;
    modelJson["caption"] = m_caption;
    for (auto parameter : getParameters())
        parameters[parameter.first] = parameter.second;
    modelJson["parameters"] = parameters;
    return modelJson;
}

void FdfBlockModel::load(QJsonObject const &p)
{
    NodeDelegateModel::load(p);
    QJsonValue value = p["name"];
    if (!value.isUndefined())
        m_name = value.toString();
    value = p["function_name"];
    if (!value.isUndefined())
        m_functionName = value.toString();
    value = p["caption"];
    if (!value.isUndefined())
        m_caption = value.toString();
    value = p["parameters"];
    if (!value.isUndefined()) {
        QJsonObject parameters = value.toObject();
        for (auto key : parameters.keys())
            setParameter(key, parameters.value(key).toString());
    }
}

std::shared_ptr<NodeData> FdfBlockModel::portData(PortType const type, PortIndex const index) const
{
    if (!indexCheck(type, index))
        return std::shared_ptr<NodeData>();
    if (type == PortType::In)
        return m_inPorts.at(index).second.lock();
    if (type == PortType::Out)
        return m_outPorts.at(index).first;
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

void FdfBlockModel::setInputPortNumber(uint num)
{
    qDebug() << "Input port number cannot be modified";
}

void FdfBlockModel::setOutputPortNumber(uint num)
{
    qDebug() << "Output port number cannot be modified";
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
    m_caption = QString(caption);
    emit contentUpdated();
    emit captionUpdated(m_caption);
}

bool FdfBlockModel::setPortCaption(PortType type, PortIndex index, const QString &caption)
{
    if (!indexCheck(type, index))
        return false;
    switch (type) {
    case PortType::In:
        if (auto namedNode = dynamic_cast<NamedNode *>(m_inPorts.at(index).first.get())) {
            namedNode->setName(caption);
        } else
            return false;
        break;
    case PortType::Out:
        if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(m_outPorts.at(index).first)) {
            namedNode->setName(caption);
        } else
            return false;
        break;
    default:
        return false;
    }
    propagateUpdate();
    return true;
}

bool FdfBlockModel::setPortDefaultCaption(PortType type, PortIndex index, const QString &caption)
{
    if (!indexCheck(type, index))
        return false;
    switch (type) {
    case PortType::In:
        if (auto namedNode = dynamic_cast<NamedNode *>(m_inPorts.at(index).first.get())) {
            namedNode->setDefaultName(caption);
        } else
            return false;
        break;
    case PortType::Out:
        if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(m_outPorts.at(index).first)) {
            if (namedNode->defaultName() != caption) {
                namedNode->setDefaultName(caption);
                emit outPortCaptionUpdated(index, caption);
            }
        } else
            return false;
        break;
    default:
        return false;
    }
    propagateUpdate();
    return true;
}

bool FdfBlockModel::resetPortCaption(PortType type, PortIndex index)
{
    if (!indexCheck(type, index))
        return false;
    switch (type) {
    case PortType::In:
        if (auto namedNode = dynamic_cast<NamedNode *>(m_inPorts.at(index).first.get())) {
            namedNode->reset();
        } else
            return false;
        break;
    case PortType::Out:
        if (auto namedNode = std::dynamic_pointer_cast<NamedNode>(m_outPorts.at(index).first)) {
            namedNode->reset();
        } else
            return false;
        break;
    default:
        return false;
    }
    propagateUpdate();
    return true;
}

std::shared_ptr<NodeData> FdfBlockModel::inData(PortIndex const index)
{
    if (!indexCheck(PortType::In, index))
        return std::shared_ptr<NodeData>();
    return m_inPorts.at(index).second.lock();
}

std::unordered_map<QString, QString> FdfBlockModel::getParameters() const
{
    return std::unordered_map<QString, QString>();
}

std::unordered_map<QString, QMetaType::Type> FdfBlockModel::getParameterSchema() const
{
    return std::unordered_map<QString, QMetaType::Type>();
}

QStringList FdfBlockModel::getParameterOptions(const QString &key) const
{
    return QStringList();
}

void FdfBlockModel::setParameter(const QString &key, const QString &value) {}

unsigned int FdfBlockModel::nPorts(const PortType &portType, const QString &typeId) const
{
    uint result = 0;
    if (portType == PortType::In)
        for (auto &pair : m_inPorts)
            if (pair.first.get()->type().id == typeId)
                ++result;
    if (portType == PortType::Out)
        for (auto &pair : m_outPorts)
            if (pair.first.get()->type().id == typeId)
                ++result;
    return result;
}

unsigned int FdfBlockModel::minModifiablePorts(const PortType &portType, const QString &typeId) const
{
    return 0;
}

void FdfBlockModel::setExecutedValues(const std::unordered_map<QString, QString> &values)
{
    m_executedValues = values;
    emit contentUpdated();
}

void FdfBlockModel::setExecutedGraphs(const QStringList &paths)
{
    if (m_executedGraphs == paths)
        return;
    m_executedGraphs.clear();
    for (auto &path : paths)
        if (QFileInfo(path).exists())
            m_executedGraphs << path;
    emit contentUpdated();
}

void FdfBlockModel::updateStyle()
{
    auto style = nodeStyle();
    switch (m_type) {
    case FdfType::Coder:
        style.GradientColor1 = constants::COLOR_CODER;
        break;
    case FdfType::Processor:
        style.GradientColor1 = constants::COLOR_PROCESSOR;
        break;
    case FdfType::Trainer:
        style.GradientColor1 = constants::COLOR_TRAINER;
        break;
    case FdfType::Data:
    case FdfType::Output:
    default:
        return;
    }
    style.GradientColor0 = style.GradientColor1.lighter(110);
    style.GradientColor2 = style.GradientColor1.darker(110);
    style.GradientColor3 = style.GradientColor1.darker(150);
    setNodeStyle(style);
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
