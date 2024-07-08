#pragma once

#include <QMainWindow>

namespace QtNodes
{
    class DataFlowGraphicsScene;
}

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

    QtNodes::DataFlowGraphicsScene *m_scene;
    QWidget *m_centralWidget;
    Temp *m_temp; // class used for testing temporary code
};