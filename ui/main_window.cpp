#include "main_window.h"
#include "ui_main_window.h"

#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <QMouseEvent>
#include <QPainter>
#include <QProcess>

MainWindow::MainWindow(
    QWidget* parent,
    desktop_todo::core::EventRepository* event_repository,
    desktop_todo::services::UpdateService* update_service)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      event_repository_(event_repository),
      update_service_(update_service),
      task_list_dialog_(new TaskListDialog(nullptr, event_repository)),
      add_dialog_(new AddDialog(nullptr, event_repository)) {
  ui_->setupUi(this);

  qDebug() << "start!!!";

  // 去边框
  setWindowFlags(Qt::FramelessWindowHint | windowFlags());

  // 任务栏隐藏
  setWindowFlags(Qt::ToolTip | windowFlags());

  // 背景透明
  setAttribute(Qt::WA_TranslucentBackground);

  // 置顶
  setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());

  tray_icon_ = new QSystemTrayIcon(this);
  tray_icon_->setIcon(QIcon(QStringLiteral("../image/cat.ico")));
  tray_icon_->setToolTip(QStringLiteral("cat"));
  connect(tray_icon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
          SLOT(OnActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
  connect(tray_icon_, &QSystemTrayIcon::messageClicked,
          [this]() { this->show(); });

  ConnectUpdateService();
  CreateActions();
  CreateMenu();

  movie_ = new QMovie(QStringLiteral("../image/bear.gif"), QByteArray(), this);
  movie_->start();

  // 在系统托盘显示此对象
  tray_icon_->show();
}

void MainWindow::ConnectUpdateService() {
  connect(update_service_,
          &desktop_todo::services::UpdateService::UpToDate, this,
          &MainWindow::OnUpToDate);
  connect(update_service_,
          &desktop_todo::services::UpdateService::UpdateAvailable, this,
          &MainWindow::OnUpdateAvailable);
  connect(update_service_,
          &desktop_todo::services::UpdateService::CheckFailed, this,
          &MainWindow::OnUpdateCheckFailed);
}

/// TODO: 图片用户自定义 可以直接从配置文件中读取
void MainWindow::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);
  painter.drawPixmap(0, 10, movie_->currentPixmap());
}

// 拖拽操作
void MainWindow::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    dragging_ = true;
    drag_start_mouse_position_ = event->globalPosition();
    drag_start_window_position_ = this->frameGeometry().topLeft();

    task_list_dialog_->HideList();
  }
  if (event->button() == Qt::RightButton) {
    task_list_dialog_->ShowList();
  }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
  update();  // 刷新 GIF
  if (dragging_) {
    const QPointF distance = event->globalPosition() - drag_start_mouse_position_;
    this->move(drag_start_window_position_.toPoint() + distance.toPoint());
  }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    dragging_ = false;
  }
}

void MainWindow::OnActivatedSysTrayIcon(
    QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger:
      this->show();
      break;
    default:
      break;
  }
}

void MainWindow::CreateActions() {
  show_action_ = new QAction(QStringLiteral("Add"), this);
  connect(show_action_, &QAction::triggered, this, [&] {
    add_dialog_->setWindowFlags(Qt::CoverWindow);
    add_dialog_->Show();
    qDebug() << "Add";
  });

  hide_action_ = new QAction(QStringLiteral("Hide"), this);
  connect(hide_action_, &QAction::triggered, this, [this]() { this->hide(); });

  update_action_ = new QAction(QStringLiteral("Update"), this);
  connect(update_action_, &QAction::triggered, this,
          [this]() { update_service_->CheckForUpdates(); });

  exit_action_ = new QAction(QStringLiteral("Quit"), this);
  connect(exit_action_, &QAction::triggered, this, [] { exit(0); });
}

void MainWindow::OnUpToDate() {
  QMessageBox message_box;
  message_box.setText(QStringLiteral("已经是最新版本啦！^-^"));
  message_box.exec();
}

void MainWindow::OnUpdateAvailable(const QString& /*release_date*/,
                                   const QString& log,
                                   const QString& url) {
  QMessageBox message_box;
  message_box.setText(QStringLiteral("检测到有新版本！"));
  message_box.setInformativeText(log);
  message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  message_box.setDefaultButton(QMessageBox::Cancel);
  if (message_box.exec() == QMessageBox::Ok) {
    OpenUrlInBrowser(url);
  }
}

void MainWindow::OnUpdateCheckFailed(const QString& /*reason*/) {
  QMessageBox message_box;
  message_box.setText(QStringLiteral("网络错误^-^！"));
  message_box.exec();
}

void MainWindow::OpenUrlInBrowser(const QString& url) {
  QProcess process;
  const QString command = QStringLiteral("start ") + url;
  process.start(QStringLiteral("cmd"),
                QStringList() << QStringLiteral("/c") << command);
  process.waitForStarted();
  process.waitForFinished();
}

void MainWindow::CreateMenu() {
  tray_menu_ = new QMenu(this);
  tray_menu_->addAction(show_action_);
  tray_menu_->addAction(hide_action_);
  tray_menu_->addAction(update_action_);
  tray_menu_->addAction(exit_action_);
  tray_icon_->setContextMenu(tray_menu_);
}

MainWindow::~MainWindow() { delete ui_; }
