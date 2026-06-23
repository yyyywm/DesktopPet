#include <memory>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDir>

#include "core/config_store.h"
#include "core/event_repository.h"
#include "core/qsettings_adapter.h"
#include "services/update_service.h"
#include "ui/application_controller.h"
#include "ui/task_list_model.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  const QString config_path =
      QDir::current().filePath(QStringLiteral("../config/eventlist.ini"));
  auto settings =
      std::make_unique<desktop_todo::core::QSettingsAdapter>(config_path);
  auto config_store =
      std::make_unique<desktop_todo::core::ConfigStore>(std::move(settings));
  desktop_todo::core::EventRepository event_repository(config_store.get());
  event_repository.Load();
  qDebug() << "Loaded" << event_repository.events().size() << "events";

  desktop_todo::services::UpdateService update_service;
  desktop_todo::ui::TaskListModel task_model(&event_repository);
  desktop_todo::ui::ApplicationController controller(&update_service);

  std::unique_ptr<QObject> theme;
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty(
      QStringLiteral("TaskModel"), &task_model);
  engine.rootContext()->setContextProperty(QStringLiteral("App"), &controller);

  QQmlComponent theme_component(
      &engine, QUrl(QStringLiteral("qrc:/DesktopPet/qml/Theme.qml")));
  theme.reset(theme_component.create());
  if (theme != nullptr) {
    engine.rootContext()->setContextProperty(QStringLiteral("AppTheme"),
                                             theme.get());
  }

  const QUrl url(QStringLiteral("qrc:/DesktopPet/qml/main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject* obj, const QUrl& obj_url) {
        if (obj == nullptr && url == obj_url) {
          qCritical() << "Failed to load main QML:" << url;
          QCoreApplication::exit(-1);
        }
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
