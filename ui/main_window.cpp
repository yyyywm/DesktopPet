
#include "main_window.h"
#include "ui_main_window.h"
#include <QPainter>
#include <QBitmap>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDebug>
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(
    QWidget *parent,
    desktop_todo::core::EventRepository* event_repository,
    desktop_todo::services::UpdateService* update_service)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      event_repository_(event_repository),
      update_service_(update_service),
      todo_win_(new TaskListDialog(nullptr, event_repository)),
      add_event_(new AddDialog(nullptr, event_repository)) {
    ui_->setupUi(this);

    qDebug() << "start!!!";

    // 去边框
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 任务栏隐藏
    setWindowFlags(Qt::ToolTip | windowFlags());     // 如果进行任务栏隐藏的话，子窗口关闭，主窗口也会关闭。但是不处理就没事

    //背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    //置顶
    setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());

    tray_icon_ = new QSystemTrayIcon(this);
    tray_icon_->setIcon(QIcon("../image/cat.ico"));
    tray_icon_->setToolTip(("cat"));
    connect(tray_icon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
            SLOT(OnActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    connect(tray_icon_, &QSystemTrayIcon::messageClicked, [&]() {
      this->show();
    });

    connect(update_service_, &desktop_todo::services::UpdateService::UpToDate,
            this, &MainWindow::OnUpToDate);
    connect(update_service_,
            &desktop_todo::services::UpdateService::UpdateAvailable,
            this, &MainWindow::OnUpdateAvailable);
    connect(update_service_,
            &desktop_todo::services::UpdateService::CheckFailed,
            this, &MainWindow::OnUpdateCheckFailed);

    //建立托盘操作的菜单
    CreateActions();
    CreateMenu();

    //在系统托盘显示此对象
    tray_icon_->show();

}

/// TODO: 图片用户自定义 可以直接从配置文件中读取
void MainWindow::paintEvent(QPaintEvent *) {
    QPainter P(this);        //创建画家对象
    P.begin(this);          //指定当前窗口为绘图设备
    P.drawPixmap(10, 0, QPixmap("../image/image_left.png"));  // 显示图片
}

//拖拽操作
void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_b_drag_ = true;
        //获得鼠标的初始位置
        mouse_start_point_ = event->globalPosition();

        //获得窗口的初始位置
        window_top_left_point_ = this->frameGeometry().topLeft();

        todo_win_->HideList();
    }
    if (event->button() == Qt::RightButton) {
        todo_win_->ShowList();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_b_drag_) {
        //获得鼠标移动的距离
        QPointF distance = event->globalPosition() - mouse_start_point_;

        //改变窗口的位置
        this->move(window_top_left_point_.toPoint() + distance.toPoint());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_b_drag_ = false;
    }
}

void MainWindow::OnActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:this->show();
//            tray_icon_->showMessage(("cat"), ("welcome use me"),
//                                      QSystemTrayIcon::Information, 1000);
        default:break;
    }
}

void MainWindow::CreateActions() {
    show_action_ = new QAction(("Add"), this);
    connect(show_action_, &QAction::triggered, this, [&] {
//      this->show();
      add_event_->setWindowFlags(Qt::CoverWindow);
      add_event_->Show();
      qDebug() << "Add";

    });

    hide_action_ = new QAction(("Hide"), this);
    connect(hide_action_, &QAction::triggered, [&]() {
//        qDebug() << "hide";
      this->hide();
    });

    update_action_ = new QAction(("Update"), this);
    connect(update_action_, &QAction::triggered, this, [&] {
      update_service_->CheckForUpdates();
    });

    exit_action_ = new QAction(("Quit"), this);
    connect(exit_action_, &QAction::triggered, this, [&] {
      exit(0);
    });
}

void MainWindow::OnUpToDate() {
    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("已经是最新版本啦！^-^"));
    msgBox.exec();
}

void MainWindow::OnUpdateAvailable(const QString& /*release_date*/,
                                   const QString& log,
                                   const QString& url) {
    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("检测到有新版本！"));
    msgBox.setInformativeText(log);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Ok) {
        OpenUrlInBrowser(url);
    }
}

void MainWindow::OnUpdateCheckFailed(const QString& /*reason*/) {
    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("网络错误^-^！"));
    msgBox.exec();
}

void MainWindow::OpenUrlInBrowser(const QString& url) {
    QProcess process;
    const QString command = QStringLiteral("start ") + url;
    process.start(QStringLiteral("cmd"), QStringList() << QStringLiteral("/c") << command);
    process.waitForStarted();
    process.waitForFinished();
}

void MainWindow::CreateMenu() {
    m_menu_ = new QMenu(this);
    m_menu_->addAction(show_action_);
    m_menu_->addAction(hide_action_);
    m_menu_->addAction(update_action_);
    m_menu_->addAction(exit_action_);
    tray_icon_->setContextMenu(m_menu_);
}

MainWindow::~MainWindow() {
    delete ui_;
}