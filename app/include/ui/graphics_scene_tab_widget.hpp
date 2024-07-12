#pragma once

#include <QTabWidget>

class QWidget;

namespace QtNodes
{
    class DataFlowGraphModel;
    class DataFlowGraphicsScene;
    class GraphicsView;
}

class TabComponents
{
public:
    TabComponents(QWidget *parent = nullptr);

    QtNodes::DataFlowGraphModel *getModel() const { return m_model; }
    QtNodes::DataFlowGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }

private:
    QtNodes::DataFlowGraphModel *m_model;
    QtNodes::DataFlowGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
};

class GraphicsSceneTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    GraphicsSceneTabWidget(QWidget *parent = nullptr);
    void addBlankTab();

signals:
    void countChanged();

public slots:
    bool save();
    bool saveAs();
    bool open();

private slots:
    void closeTab(int index);
    void onTabCountChanged();
    void setCurrentTabText(const QString &label);

protected:
    virtual void tabInserted(int index) override;
    virtual void tabRemoved(int index) override;

private:
    QtNodes::DataFlowGraphicsScene *getCurrentScene() const;

    std::map<QWidget *, TabComponents> m_tabs;
};