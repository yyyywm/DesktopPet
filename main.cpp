
#include "main_window.h"

#include <QApplication>
#include <QDir>

#include "core/config_store.h"
#include "core/event_repository.h"
#include "core/qsettings_adapter.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  const QString config_path =
      QDir::current().filePath(QStringLiteral("../config/eventlist.ini"));
  auto settings =
      std::make_unique<desktop_todo::core::QSettingsAdapter>(config_path);
  auto config_store =
      std::make_unique<desktop_todo::core::ConfigStore>(std::move(settings));
  desktop_todo::core::EventRepository event_repository(config_store.get());
  event_repository.Load();

  MainWindow w(nullptr, &event_repository);
  w.show();
  return a.exec();
}
