#pragma once

#include <QObject>

#include <QtNodes/NodeDelegateModel>

using QtNodes::ConnectionId;
using QtNodes::NodeDelegateModel;
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
    QString functionName() const { return m_functionName; }
    QString caption() const override { return m_caption; }
    unsigned int nPorts(PortType const portType) const override;
    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const index) override;
    virtual void setInData(std::shared_ptr<NodeData> data, PortIndex const index) override;
    virtual QWidget *embeddedWidget() override;
    QString portCaption(PortType portType, PortIndex portIndex) const override;

    virtual std::shared_ptr<NodeData> portData(PortType const type, PortIndex const index);
    virtual std::vector<std::shared_ptr<NodeData>> connectedPortData(PortType const type) const;
    void setCaption(const QString &caption);
    bool setPortCaption(PortType type, PortIndex index, const QString &caption);
    bool resetPortCaption(PortType portType, PortIndex portIndex);
    virtual std::shared_ptr<NodeData> inData(PortIndex const index);

public slots:
    virtual void outputConnectionCreated(ConnectionId const &conn) override;
    virtual void outputConnectionDeleted(ConnectionId const &conn) override;

protected:
    bool indexCheck(PortType type, PortIndex index) const;
    void propagateUpdate();
    PortIndex addInPort(std::unique_ptr<NodeData> port);
    PortIndex addOutPort(std::shared_ptr<NodeData> port);

private:
    const std::unordered_map<FdfType, QString> TYPE_STRING = {
        {FdfType::Coder, "coder"},
        {FdfType::Processor, "processor"},
        {FdfType::Trainer, "trainer"},
        {FdfType::Data, "data"},
        {FdfType::Output, "output"},
    };

    FdfType m_type;
    QString m_name; // name in the library
    QString m_functionName;
    QString m_caption; // appears in the scene
    // first is for structuring, second is actual data linked to connected block
    std::vector<std::pair<std::unique_ptr<NodeData>, std::weak_ptr<NodeData>>> m_inPorts;
    // first is for data, second represents whether it's in use
    std::vector<std::pair<std::shared_ptr<NodeData>, bool>> m_outPorts;
};