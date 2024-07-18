#pragma once

#include <QTabWidget>
#include <QFileInfo>

#include <QtNodes/Definitions>

class QWidget;

namespace QtNodes
{
    class DirectedAcyclicGraphModel;
    class DagGraphicsScene;
    class GraphicsView;
}

class TabComponents
{
public:
    TabComponents(QWidget *parent = nullptr);

    QtNodes::DirectedAcyclicGraphModel *getModel() const { return m_model; }
    QtNodes::DagGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }
    QFileInfo getFile() const;

private:
    QtNodes::DirectedAcyclicGraphModel *m_model;
    QtNodes::DagGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
};

class GraphicsSceneTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    GraphicsSceneTabWidget(QWidget *parent = nullptr);
    QtNodes::DirectedAcyclicGraphModel *getCurrentModel() const;

signals:
    void countChanged(int count);
    void runClicked();
    void nodeSelected(QtNodes::NodeId id);

public slots:
    void newTab();
    bool save();
    bool saveAs();
    bool open();
    void closeCurrentTab();

private slots:
    void closeTab(int index);
    void onTabCountChanged(int count);
    void setCurrentTabText(const QString &label);
    void onSceneSelectionChanged();

protected:
    virtual void tabInserted(int index) override;
    virtual void tabRemoved(int index) override;

private:
    QtNodes::DagGraphicsScene *getCurrentScene() const;
    bool openIfExists(QtNodes::DagGraphicsScene *scene);
    void addTabComponent(const TabComponents &tabComponents);

    std::map<QWidget *, TabComponents> m_tabs;
};