#pragma once

#include <QMainWindow>

class GraphicsSceneTabWidget;
class AbstractEngine;
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

    GraphicsSceneTabWidget *m_graphicsSceneTabWidget;
    QWidget *m_centralWidget;
    std::unique_ptr<AbstractEngine> m_engine;
    Temp *m_temp; // class used for testing temporary code
};