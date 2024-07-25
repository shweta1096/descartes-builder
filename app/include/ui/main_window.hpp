#pragma once

#include <QMainWindow>

class GraphicsSceneTabWidget;
class AbstractEngine;
class TabManager;
class BlockManager;
class Temp;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private slots:
    bool callExecute();

private:
    // Ui inits
    void initScene();
    void initMenuBar();
    void initPrimarySideBar();
    void initLogPanel();

    // UI
    GraphicsSceneTabWidget *m_graphicsSceneTabWidget;
    QWidget *m_centralWidget;

    // Logic
    std::unique_ptr<AbstractEngine> m_engine;
    std::shared_ptr<TabManager> m_tabManager;
    std::shared_ptr<BlockManager> m_blockManager;
    Temp *m_temp; // class used for testing temporary code
};