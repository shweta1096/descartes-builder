#include <QApplication>
#include <QFile>

#include "log_manager.hpp"
#include "ui/main_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    { // load stylesheet
        QFile style(":/style.qss");
        style.open(QFile::ReadOnly);
        app.setStyleSheet(QLatin1String(style.readAll()));
        style.close();
    }

    LogManager::instance().init();

    MainWindow w;
    w.show();
    return app.exec();
}