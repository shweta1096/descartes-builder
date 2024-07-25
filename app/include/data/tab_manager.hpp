#pragma once

#include <QFileInfo>
#include <QObject>

#include <QtNodes/Definitions>

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

class TabManager : public QObject
{
    Q_OBJECT
public:
    using ViewWidget = QWidget;

    TabManager(QObject *parent = nullptr);
    std::optional<TabComponents> getCurrentTab() const;
    std::optional<TabComponents> getTab(QWidget *view) const;
    size_t size() const { return m_tabs.size(); }
    QWidget *currentWidget() const { return m_currentView; }
    QtNodes::DirectedAcyclicGraphModel *currentGraph() const;
    QtNodes::DagGraphicsScene *currentScene() const;
    QtNodes::GraphicsView *currentView() const;
    bool addTab(const TabComponents &tab);
    void removeTab(const TabComponents &tab);
    void removeTab(ViewWidget *view);
    void setTabParent(QWidget *parent) { m_tabParent = parent; }
    void setCurrentView(ViewWidget *view) { m_currentView = view; }

signals:
    void newTabCreated(ViewWidget *view, QString filename);
    void currentChanged(ViewWidget *view);
    void tabFileNameChanged(ViewWidget *view, QString fileName);

public:
    void newTab();
    bool save();
    bool saveAs();
    bool open();

private:
    bool openIfExists(QtNodes::DagGraphicsScene *scene);

    std::unordered_map<ViewWidget *, TabComponents> m_tabs;
    ViewWidget *m_currentView;
    QWidget *m_tabParent;
};
