#include "ui/application_controller.h"

#include <QApplication>

namespace desktop_todo {
namespace ui {

ApplicationController::ApplicationController(
    services::UpdateService* update_service, QObject* parent)
    : QObject(parent), update_service_(update_service) {
  CreateTrayIcon();
  CreateActions();
  CreateMenu();
  ConnectUpdateService();
  tray_icon_->show();
}

void ApplicationController::CreateTrayIcon() {
  tray_icon_ = new QSystemTrayIcon(this);
  tray_icon_->setIcon(QIcon(QStringLiteral("../image/cat.ico")));
  tray_icon_->setToolTip(QStringLiteral("DesktopPet"));
  connect(tray_icon_, &QSystemTrayIcon::activated, this,
          [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger) {
              emit ShowPetWindowRequested();
            }
          });
}

void ApplicationController::CreateActions() {
  hide_action_ = new QAction(QStringLiteral("Hide"), this);
  connect(hide_action_, &QAction::triggered, this,
          &ApplicationController::HidePetWindowRequested);

  add_action_ = new QAction(QStringLiteral("Add"), this);
  connect(add_action_, &QAction::triggered, this,
          &ApplicationController::OpenAddDialogRequested);

  update_action_ = new QAction(QStringLiteral("Update"), this);
  connect(update_action_, &QAction::triggered, this,
          &ApplicationController::CheckForUpdates);

  exit_action_ = new QAction(QStringLiteral("Quit"), this);
  connect(exit_action_, &QAction::triggered, this,
          &ApplicationController::Quit);
}

void ApplicationController::CreateMenu() {
  // QSystemTrayIcon::setContextMenu() takes ownership of the menu.
  tray_menu_ = new QMenu();
  tray_menu_->addAction(hide_action_);
  tray_menu_->addAction(add_action_);
  tray_menu_->addAction(update_action_);
  tray_menu_->addAction(exit_action_);
  tray_icon_->setContextMenu(tray_menu_);
}

void ApplicationController::ConnectUpdateService() {
  if (update_service_ == nullptr) {
    return;
  }
  connect(update_service_, &services::UpdateService::UpToDate, this,
          &ApplicationController::OnUpToDate);
  connect(update_service_, &services::UpdateService::UpdateAvailable, this,
          &ApplicationController::OnUpdateAvailable);
  connect(update_service_, &services::UpdateService::CheckFailed, this,
          &ApplicationController::OnUpdateCheckFailed);
}

void ApplicationController::CheckForUpdates() {
  if (update_service_ == nullptr) {
    return;
  }
  update_service_->CheckForUpdates();
}

void ApplicationController::Quit() { QApplication::quit(); }

void ApplicationController::OnUpToDate() {
  emit UpdateMessageRequested(QStringLiteral("已经是最新版本啦！^-^"), QString(),
                              QString());
}

void ApplicationController::OnUpdateAvailable(const QString& /*release_date*/,
                                              const QString& log,
                                              const QString& url) {
  emit UpdateMessageRequested(QStringLiteral("检测到有新版本！"), log, url);
}

void ApplicationController::OnUpdateCheckFailed(const QString& /*reason*/) {
  emit UpdateMessageRequested(QStringLiteral("网络错误^-^！"), QString(),
                              QString());
}

}  // namespace ui
}  // namespace desktop_todo
