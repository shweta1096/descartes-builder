#pragma once

#include <QFileInfo>
#include <QObject>
#include <QTemporaryDir>

#include <QtNodes/Definitions>

namespace QtNodes {
class DagGraphicsScene;
class GraphicsView;
} // namespace QtNodes

class CustomGraph;

class TabComponents
{
public:
    TabComponents(QWidget *parent, std::optional<QFileInfo> fileInfo = std::nullopt);
    ~TabComponents();
    CustomGraph *getGraph() const { return m_graph; }
    QtNodes::DagGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }
    std::shared_ptr<QTemporaryDir> getTempDir() { return m_dir; }
    QFileInfo getFileInfo() { return m_localFile; }
    void setFileInfo(const QFileInfo &fileInfo) { m_localFile = fileInfo; }

private:
    CustomGraph *m_graph;
    QtNodes::DagGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
    // dir with .dag and imported files
    std::shared_ptr<QTemporaryDir> m_dir;
    // zipped local file to output to
    QFileInfo m_localFile;
};

class TabManager : public QObject
{
    Q_OBJECT
public:
    using ViewWidget = QWidget;

    TabManager(QObject *parent = nullptr);
    std::shared_ptr<TabComponents> getCurrentTab() const;
    std::shared_ptr<TabComponents> getTab(QWidget *view) const;
    size_t size() const { return m_tabs.size(); }
    QWidget *currentWidget() const { return m_currentView; }
    CustomGraph *currentGraph() const;
    QtNodes::DagGraphicsScene *currentScene() const;
    QtNodes::GraphicsView *currentView() const;
    QString currentTabName() const;
    bool addTab(std::shared_ptr<TabComponents> tab);
    void removeTab(const TabComponents &tab);
    void removeTab(ViewWidget *view);
    void setTabParent(QWidget *parent) { m_tabParent = parent; }
    void setCurrentView(ViewWidget *view);
    QFileInfo getFileInfo(ViewWidget *view) const;
    void clear();

signals:
    void tabCreated(ViewWidget *view);
    void tabDeleted(ViewWidget *view);
    void currentChanged(ViewWidget *view);
    void tabFileNameChanged(ViewWidget *view, QString fileName);

public:
    void newTab();
    bool save();
    bool saveAs();
    bool open();
    bool openFrom(const QString &filePath);

private:
    bool openIfExists(const QFileInfo &file);

    std::unordered_map<ViewWidget *, std::shared_ptr<TabComponents>> m_tabs;
    ViewWidget *m_currentView;
    QWidget *m_tabParent;
};
