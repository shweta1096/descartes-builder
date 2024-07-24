#pragma once

#include <QFileInfo>
#include <QTabWidget>

#include <QtNodes/Definitions>

class QWidget;

namespace QtNodes {
class DirectedAcyclicGraphModel;
class DagGraphicsScene;
class GraphicsView;
} // namespace QtNodes

class TabComponents
{
public:
    TabComponents(QWidget *parent = nullptr);

    QtNodes::DirectedAcyclicGraphModel *getGraph() const { return m_graph; }
    QtNodes::DagGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }
    QFileInfo getFile() const;

private:
    QtNodes::DirectedAcyclicGraphModel *m_graph;
    QtNodes::DagGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
};

class GraphicsSceneTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    GraphicsSceneTabWidget(QWidget *parent = nullptr);
    QtNodes::DirectedAcyclicGraphModel *getCurrentGraph() const;

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