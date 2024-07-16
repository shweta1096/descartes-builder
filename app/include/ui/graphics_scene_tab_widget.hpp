#pragma once

#include <QTabWidget>
#include <QFileInfo>

class QWidget;

namespace QtNodes
{
    class DagGraphModel;
    class DagGraphicsScene;
    class GraphicsView;
}

class TabComponents
{
public:
    TabComponents(QWidget *parent = nullptr);

    QtNodes::DagGraphModel *getModel() const { return m_model; }
    QtNodes::DagGraphicsScene *getScene() const { return m_scene; }
    QtNodes::GraphicsView *getView() const { return m_view; }
    QFileInfo getFile() const;

private:
    QtNodes::DagGraphModel *m_model;
    QtNodes::DagGraphicsScene *m_scene;
    QtNodes::GraphicsView *m_view;
};

class GraphicsSceneTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    GraphicsSceneTabWidget(QWidget *parent = nullptr);
    QtNodes::DagGraphModel *getCurrentModel() const;

signals:
    void countChanged(int count);
    void runClicked();

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

protected:
    virtual void tabInserted(int index) override;
    virtual void tabRemoved(int index) override;

private:
    QtNodes::DagGraphicsScene *getCurrentScene() const;
    bool openIfExists(QtNodes::DagGraphicsScene *scene);

    std::map<QWidget *, TabComponents> m_tabs;
};