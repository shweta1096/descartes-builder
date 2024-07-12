#pragma once

#include <QMainWindow>

class GraphicsSceneTabWidget;
class Temp;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private:
    // Ui inits
    void initScene();
    void initMenuBar();
    void initPrimarySideBar();
    void initLogPanel();

    GraphicsSceneTabWidget *m_graphicsSceneTabWidget;
    QWidget *m_centralWidget;
    Temp *m_temp; // class used for testing temporary code
};