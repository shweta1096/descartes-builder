#pragma once

#include <QObject>

#include <QtNodes/NodeDelegateModel>

using QtNodes::ConnectionId;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeShape;
using QtNodes::PortIndex;
using QtNodes::PortType;

#include "ui/models/nodes.hpp"

class FdfBlockModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    enum FdfType {
        Coder,
        Processor,
        Trainer,
        Data,
        Output,
    };

    FdfBlockModel(FdfType type, const QString &name, const QString &functionName = QString());
    FdfType type() const { return m_type; }
    QString typeAsString() const { return TYPE_STRING.at(m_type); }
    QString name() const override { return m_name; }
    NodeShape shape() const override { return m_shape; }
    QString functionName() const { return m_functionName; }
    QString caption() const override { return m_caption; }
    unsigned int nPorts(PortType const portType) const override;
    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const index) override;
    virtual void setInData(std::shared_ptr<NodeData> data, PortIndex const index) override;
    virtual QWidget *embeddedWidget() override;
    QString portCaption(PortType portType, PortIndex portIndex) const override;

    virtual std::shared_ptr<NodeData> portData(PortType const type, PortIndex const index) const;
    virtual std::vector<std::shared_ptr<NodeData>> connectedPortData(PortType const type) const;
    void setCaption(const QString &caption);
    bool setPortCaption(PortType type, PortIndex index, const QString &caption);
    bool setPortDefaultCaption(PortType type, PortIndex index, const QString &caption);
    bool resetPortCaption(PortType portType, PortIndex portIndex);
    virtual std::shared_ptr<NodeData> inData(PortIndex const index);

public slots:
    virtual void outputConnectionCreated(ConnectionId const &conn) override;
    virtual void outputConnectionDeleted(ConnectionId const &conn) override;

protected:
    bool indexCheck(PortType type, PortIndex index) const;
    void propagateUpdate();
    template<typename T>
    void addPort(PortType type, const QString &name = QString())
    {
        static_assert(std::is_base_of<NodeData, T>::value, "T must derive from NodeData");
        if (type == PortType::In) {
            auto port = name.isEmpty() ? std::make_unique<T>() : std::make_unique<T>(name);
            m_inPorts.push_back({std::move(port), std::weak_ptr<NodeData>()});
        } else if (type == PortType::Out) {
            auto port = name.isEmpty() ? std::make_shared<T>() : std::make_shared<T>(name);
            m_outPorts.push_back({port, false});
        } else {
            qCritical() << "Unhandled type";
            return;
        }
        emit portsInserted();
    }

private:
    void updateStyle();
    void updateShape();

    const std::unordered_map<FdfType, QString> TYPE_STRING = {
        {FdfType::Coder, "coder"},
        {FdfType::Processor, "processor"},
        {FdfType::Trainer, "trainer"},
        {FdfType::Data, "data"},
        {FdfType::Output, "output"},
    };

    FdfType m_type;
    QString m_name; // name in the library
    NodeShape m_shape;
    QString m_functionName;
    QString m_caption; // appears in the scene
    // first is for structuring, second is actual data linked to connected block
    std::vector<std::pair<std::unique_ptr<NodeData>, std::weak_ptr<NodeData>>> m_inPorts;
    // first is for data, second represents whether it's in use
    std::vector<std::pair<std::shared_ptr<NodeData>, bool>> m_outPorts;
};
