#include <QApplication>

#include "logHandler.hpp"
#include "ui/main_window.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  qInstallMessageHandler(log::logHandler);

  MainWindow w;
  w.show();
  return app.exec();
}