#pragma once

#include <QMainWindow>

namespace QtNodes
{
    class DataFlowGraphicsScene;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private:
    // Ui inits
    void initScene();
    void initMenuBar();
    void initPrimarySideBar();

    QtNodes::DataFlowGraphicsScene *m_scene;
    QWidget *m_centralWidget;
};