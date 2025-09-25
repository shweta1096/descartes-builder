#include "ui/models/fdf_block_model.hpp"
#include "data/constants.hpp"
#include "data/tab_manager.hpp"
#include <QAbstractButton>
#include <QFileInfo>
#include <QJsonArray>
#include <QMessageBox>

FdfBlockModel::FdfBlockModel(FdfType type, const QString &name, const QString &functionName)
    : NodeDelegateModel()
    , m_type(type)
    , m_name(name)
    , m_functionName(functionName)
    , m_caption(name)
    , m_label(nullptr)
{
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

bool FdfBlockModel::hasDataOutPorts()
{
    for (const auto &port : m_outPorts)
        if (std::dynamic_pointer_cast<DataNode>(port.first))
            return true;
    return false;
}

bool FdfBlockModel::hasFunctionOutPorts()
{
    for (const auto &port : m_outPorts)
        if (std::dynamic_pointer_cast<FunctionNode>(port.first))
            return true;
    return false;
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
        // delete the data and reset the port names accordingly
        emit dataInvalidated(index);
        m_inPorts.at(index).second = std::weak_ptr<NodeData>();
        resetPortCaption(PortType::In, index);
        if (auto casted = getInputPortAt<FunctionNode>(index))
            onFunctionInputReset(index);
        else if (auto casted = getInputPortAt<DataNode>(index))
            onDataInputReset(index);
        return;
    }
    m_inPorts.at(index).second = data;
    setPortCaption(PortType::In, index, data->type().name);
    if (auto casted = std::dynamic_pointer_cast<FunctionNode>(data))
        onFunctionInputSet(index);
    else if (auto casted = std::dynamic_pointer_cast<DataNode>(data))
        onDataInputSet(index);
    propagateUpdate();
}

QWidget *FdfBlockModel::embeddedWidget()
{
    if (!m_label) {
        // Enables resizing of the block
        m_label = new QLabel();
        m_label->setAlignment(Qt::AlignCenter);
        m_label->setStyleSheet("QLabel{ background: transparent; }");
    }
    return m_label;
}

bool FdfBlockModel::checkBlockValidity() const
{
    // check if all input ports are connected.
    int index = 0;
    for (const auto &portPair : m_inPorts) {
        if (!portPair.second.lock()) {
            qWarning() << "Input port at index " << index << " of block " << this->caption()
                       << " is not connected.";
            return false;
        }
        index++;
    }
    return true;
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

    // Save output ports
    QJsonArray outputPortsJson;
    for (size_t i = 0; i < m_outPorts.size(); ++i) {
        QJsonObject portJson;
        portJson["index"] = static_cast<int>(i);

        if (auto dataPort = castedPort<DataNode>(PortType::Out, i)) {
            portJson["type_tag"] = dataPort->typeTagName();
            portJson["annotation"] = dataPort->annotation();
        } else if (auto funcPort = castedPort<FunctionNode>(PortType::Out, i)) {
            portJson["caption"] = funcPort->name();
        }
        outputPortsJson.append(portJson);
    }
    modelJson["output_ports"] = outputPortsJson;

    // Save input ports
    QJsonArray inputPortsJson;
    for (size_t i = 0; i < m_inPorts.size(); ++i) {
        QJsonObject portJson;
        portJson["index"] = static_cast<int>(i);
        if (auto casted = getInputPortAt<DataNode>(i))
            portJson["type_id"] = constants::DATA_PORT_ID;
        else if (auto casted = getInputPortAt<FunctionNode>(i))
            portJson["type_id"] = constants::FUNCTION_PORT_ID;
        else
            qWarning() << "Unknown input port type at index" << i;
        inputPortsJson.append(portJson);
    }
    modelJson["input_ports"] = inputPortsJson;
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
        m_caption = constants::sanitizeCaption(value.toString());
    value = p["parameters"];
    if (!value.isUndefined()) {
        QJsonObject parameters = value.toObject();
        for (auto key : parameters.keys())
            setParameter(key, parameters.value(key).toString());

        // load input ports
        QJsonArray inputPortsJson = p["input_ports"].toArray();
        if (!inputPortsJson.isEmpty()) {
            // First, clear current ports (or sync to expected count)
            setPortNumber<DataNode>(PortType::In, 0);
            setPortNumber<FunctionNode>(PortType::In, 0);

            for (const auto &inputPort : inputPortsJson) {
                QJsonObject portJson = inputPort.toObject();
                QString typeId = portJson["type_id"].toString(); // data or function
                if (typeId == constants::DATA_PORT_ID) {
                    addPort<DataNode>(PortType::In);
                } else if (typeId == constants::FUNCTION_PORT_ID) {
                    addPort<FunctionNode>(PortType::In);
                } else {
                    qWarning() << "Unknown input port typeId:" << typeId;
                }
            }
        }
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

void FdfBlockModel::onFunctionInputSet(const PortIndex &index) {}

void FdfBlockModel::onDataInputSet(const PortIndex &index) {}

void FdfBlockModel::onFunctionInputReset(const PortIndex &index) {}

void FdfBlockModel::onDataInputReset(const PortIndex &index) {}

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

bool FdfBlockModel::setPortTagAndAnnotation(PortType type,
                                            PortIndex index,
                                            const QString &tag,
                                            const QString &annot)
{
    // This will set the port tag and annotation and internally call setportcaption with the new caption
    if (!indexCheck(type, index))
        return false;
    switch (type) {
    case PortType::In:
        if (auto casted = getInputPortAt<DataNode>(index)) {
            casted->setTypeTagName(tag);
            casted->setAnnotation(annot);
        } else
            return false;
        break;
    case PortType::Out:
        if (auto casted = castedPort<DataNode>(PortType::Out, index)) {
            casted->setTypeTagName(tag);
            casted->setAnnotation(annot);
            emit outPortCaptionUpdated(index, casted->name());
        } else
            return false;
        break;
    default:
        return false;
    }
    return true;
}

bool FdfBlockModel::setPortCaption(PortType type, PortIndex index, const QString &caption)
{
    if (!indexCheck(type, index))
        return false;
    switch (type) {
    case PortType::In:
        if (auto casted = getInputPortAt<NamedNode>(index)) {
            casted->setName(caption);
        } else
            return false;
        break;
    case PortType::Out:
        if (auto casted = castedPort<NamedNode>(PortType::Out, index)) {
            casted->setName(caption);
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
        if (auto casted = getInputPortAt<NamedNode>(index)) {
            casted->reset();
        } else
            return false;
        break;
    case PortType::Out:
        if (auto casted = castedPort<NamedNode>(PortType::Out, index)) {
            casted->reset();
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

bool FdfBlockModel::canConnect(ConnectionInfo &connInfo) const
{
    return true;
}

bool FdfBlockModel::warnInvalidConnection(ConnectionInfo connInfo, const QString &message) const
{
    // if already showing a warning, don’t show another
    static bool warningActive = false;
    if (warningActive)
        return false;

    warningActive = true;
    bool ret = const_cast<FdfBlockModel *>(this)->showWarning(connInfo, message);
    warningActive = false;

    return ret;
}

bool FdfBlockModel::showWarning(ConnectionInfo connInfo, const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<b><span style='color:red;'>Invalid Connection</span></b>");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    // Check the message and handle accordingly
    if (message == constants::TYPE_MISMATCH) {
        auto expectedInType = connInfo.expectedInType;
        auto receivedOutType = connInfo.receivedOutType;
        auto uidManager = TabManager::getUIDManager();
        QString expectedTag = uidManager->getTag(expectedInType);
        QString gotTag = uidManager->getTag(receivedOutType);
        msgBox.setInformativeText(QString(message).arg(gotTag).arg(expectedTag));

        if (expectedTag != UIDManager::NONE_TAG && gotTag != UIDManager::NONE_TAG) {
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore);
            msgBox.button(QMessageBox::Ignore)->setText("Override");
        }

        if (msgBox.exec() == QMessageBox::Ignore) {
            uidManager->updateMap(receivedOutType, expectedTag);
            return true;
        }

    } else if (message == constants::SINGULAR_SIGNATURE) {
        msgBox.setInformativeText(message);
        msgBox.exec();

    } else if (message == constants::SIGNATURE_MISMATCH_PREVFUNC) {
        auto receivedTypes = connInfo.receivedSignature->inputs;
        auto expectedTypes = connInfo.expectedSignature->outputs;
        return handleSignatureMismatch(expectedTypes, receivedTypes, message);
    } else if (message == constants::SIGNATURE_MISMATCH_NEXTFUNC) {
        auto receivedTypes = connInfo.receivedSignature->outputs;
        auto expectedTypes = connInfo.expectedSignature->inputs;
        return handleSignatureMismatch(expectedTypes, receivedTypes, message);
    } else {
        qWarning() << "Unknown connection warning message: " << message;
    }
    return false;
}

bool FdfBlockModel::handleSignatureMismatch(std::vector<FdfUID> expectedTypes,
                                            std::vector<FdfUID> receivedTypes,
                                            const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<b><span style='color:red;'>Invalid Connection</span></b>");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    auto uidManager = TabManager::getUIDManager();
    if (receivedTypes.size() != expectedTypes.size()) {
        msgBox.setInformativeText(QString(message)
                                      .arg(uidManager->toString(receivedTypes))
                                      .arg(uidManager->toString(expectedTypes)));
        msgBox.exec();
        return false;
    }

    for (int i = 0; i < receivedTypes.size(); ++i) {
        if (receivedTypes[i] != expectedTypes[i]) {
            QString expectedTag = uidManager->getTag(expectedTypes[i]);
            QString gotTag = uidManager->getTag(receivedTypes[i]);

            if (expectedTag != UIDManager::NONE_TAG && gotTag != UIDManager::NONE_TAG) {
                msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore);
                msgBox.button(QMessageBox::Ignore)->setText("Override");
            } else {
                msgBox.setStandardButtons(QMessageBox::Ok);
            }
            msgBox.setInformativeText(QString(message).arg(gotTag).arg(expectedTag));

            if (msgBox.exec() == QMessageBox::Ignore) {
                uidManager->updateMap(receivedTypes[i], expectedTag);
            } else {
                return false;
            }
        }
    }
    return true;
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
    case FdfType::Composer:
        style.GradientColor1 = constants::COLOR_COMPOSER;
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
    case FdfType::Composer:
        m_shape = NodeShape::Circle;
        break;
    case FdfType::Data:
    case FdfType::Output:
    default:
        m_shape = NodeShape::RoundedRectangle;
        break;
    }
}
