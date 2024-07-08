#include <QApplication>

#include "log_manager.hpp"
#include "ui/main_window.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  LogManager::instance().init();

  MainWindow w;
  w.show();
  return app.exec();
}