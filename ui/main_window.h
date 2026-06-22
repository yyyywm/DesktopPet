#ifndef DESKTOP_TODO__MAIN_WINDOW_H_
#define DESKTOP_TODO__MAIN_WINDOW_H_

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QMovie>
#include <QSystemTrayIcon>

#include "add_dialog.h"
#include "core/event_repository.h"
#include "services/update_service.h"
#include "task_list_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(
      QWidget* parent,
      desktop_todo::core::EventRepository* event_repository,
      desktop_todo::services::UpdateService* update_service);
  ~MainWindow();

 private:
  void CreateActions();
  void CreateMenu();
  void ConnectUpdateService();

  void OnUpToDate();
  void OnUpdateAvailable(const QString& release_date,
                         const QString& log,
                         const QString& url);
  void OnUpdateCheckFailed(const QString& reason);
  void OpenUrlInBrowser(const QString& url);

  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

 private slots:
  void OnActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

 private:
  Ui::MainWindow* ui_ = nullptr;
  desktop_todo::core::EventRepository* event_repository_ = nullptr;
  desktop_todo::services::UpdateService* update_service_ = nullptr;

  bool dragging_ = false;
  QPointF drag_start_mouse_position_;
  QPointF drag_start_window_position_;

  QSystemTrayIcon* tray_icon_ = nullptr;
  QMenu* tray_menu_ = nullptr;
  QAction* show_action_ = nullptr;
  QAction* hide_action_ = nullptr;
  QAction* update_action_ = nullptr;
  QAction* exit_action_ = nullptr;

  TaskListDialog* task_list_dialog_ = nullptr;
  AddDialog* add_dialog_ = nullptr;
  QMovie* movie_ = nullptr;
};

#endif  // DESKTOP_TODO__MAIN_WINDOW_H_
