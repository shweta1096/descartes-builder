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
    void initScene();
    void initMenu();

    QtNodes::DataFlowGraphicsScene *m_scene;
    QWidget *m_centralWidget;
};