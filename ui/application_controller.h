#ifndef DESKTOP_TODO__UI__APPLICATION_CONTROLLER_H_
#define DESKTOP_TODO__UI__APPLICATION_CONTROLLER_H_

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

#include "services/update_service.h"

namespace desktop_todo {
namespace ui {

class ApplicationController : public QObject {
  Q_OBJECT

 public:
  explicit ApplicationController(services::UpdateService* update_service,
                                 QObject* parent = nullptr);

 signals:
  void OpenAddDialogRequested();
  void ShowPetWindowRequested();
  void HidePetWindowRequested();
  void UpdateMessageRequested(const QString& title, const QString& body,
                              const QString& url);

 public slots:
  void CheckForUpdates();
  void Quit();

 private:
  void CreateTrayIcon();
  void CreateActions();
  void CreateMenu();
  void ConnectUpdateService();

  void OnUpToDate();
  void OnUpdateAvailable(const QString& release_date, const QString& log,
                         const QString& url);
  void OnUpdateCheckFailed(const QString& reason);

  services::UpdateService* update_service_ = nullptr;

  QSystemTrayIcon* tray_icon_ = nullptr;
  QMenu* tray_menu_ = nullptr;
  QAction* hide_action_ = nullptr;
  QAction* add_action_ = nullptr;
  QAction* update_action_ = nullptr;
  QAction* exit_action_ = nullptr;
};

}  // namespace ui
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__UI__APPLICATION_CONTROLLER_H_
