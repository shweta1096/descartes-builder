#pragma once

#include <QFileInfo>
#include <QObject>

#include <QtNodes/Definitions>

namespace QtNodes {
class DagGraphicsScene;
class GraphicsView;
} // namespace QtNodes

class BlockManager;
class CustomGraph;

class TabComponents
{
public:
    TabComponents(QWidget *parent = nullptr);

    CustomGraph *getGraph() const { return m_graph; }
    QtNodes::DagGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }
    QFileInfo getFile() const;

private:
    CustomGraph *m_graph;
    QtNodes::DagGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
};

class TabManager : public QObject
{
    Q_OBJECT
public:
    using ViewWidget = QWidget;

    TabManager(QObject *parent = nullptr);
    void setBlockManager(std::shared_ptr<BlockManager> blockManager);
    std::optional<TabComponents> getCurrentTab() const;
    std::optional<TabComponents> getTab(QWidget *view) const;
    size_t size() const { return m_tabs.size(); }
    QWidget *currentWidget() const { return m_currentView; }
    CustomGraph *currentGraph() const;
    QtNodes::DagGraphicsScene *currentScene() const;
    QtNodes::GraphicsView *currentView() const;
    QString currentTabName() const;
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
    bool openFrom(const QString &filePath);

private:
    bool openIfExists(QtNodes::DagGraphicsScene *scene);

    std::shared_ptr<BlockManager> m_blockManager;
    std::unordered_map<ViewWidget *, TabComponents> m_tabs;
    ViewWidget *m_currentView;
    QWidget *m_tabParent;
};
