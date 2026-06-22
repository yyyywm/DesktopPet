
#ifndef DESKTOP_TODO__MAINWINDOW_H_
#define DESKTOP_TODO__MAINWINDOW_H_

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QTextCharFormat>
#include <QPainter>
#include <QBitmap>
#include "form.h"
#include "adddialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSystemTrayIcon *tray_icon_;
    QMenu *m_menu_;
    QAction *show_action_;
    QAction *exit_action_;
    QAction *hide_action_;
    void CreateActions();
    void CreateMenu();

 private:
    Ui::MainWindow *ui_;
    bool m_b_drag_{};  // 判断拖拽状态
    QPointF mouse_start_point_;   // 鼠标起点坐标
    QPointF window_top_left_point_;  // 窗口左上角坐标

 protected:
    // 重写以下函数

    void paintEvent(QPaintEvent *) override;

    // 鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;

    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;

    // 鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;

    Form *todo_win_ = new Form;

    AddDialog *add_event_ = new AddDialog;

 private slots:
    void OnActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

};

#endif // DESKTOP_TODO__MAINWINDOW_H_
