#include "task_list_dialog.h"
#include "ui_task_list_dialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTableWidgetItem>

// 这个是查看事件的窗口

TaskListDialog::TaskListDialog(
    QWidget *parent,
    desktop_todo::core::EventRepository* event_repository)
    : QWidget(parent),
      ui_(new Ui::TaskListDialog),
      event_repository_(event_repository) {
    ui_->setupUi(this);
    ui_->todolist->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);  // 设置表格平分
    ui_->todolist->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

TaskListDialog::~TaskListDialog() { delete ui_; }

//  显示任务列表
void TaskListDialog::ShowList() {
    LoadEvent();
    Sentence();
    this->setWindowFlags(Qt::CoverWindow | Qt::FramelessWindowHint |
                         windowFlags());  // 设置窗口置顶，边框隐藏
    this->setAttribute(Qt::WA_TranslucentBackground);  // 设置窗口透明
    this->show();
}

void TaskListDialog::HideList() {
    // Sync checkbox state back to the model before deleting.
    for (int row = 0; row < row_event_ids_.size(); ++row) {
        QCheckBox* checkbox = qobject_cast<QCheckBox*>(
            ui_->todolist->cellWidget(row, 0));
        if (checkbox == nullptr) {
            continue;
        }
        desktop_todo::core::Event* event =
            event_repository_->FindEvent(row_event_ids_[row]);
        if (event != nullptr) {
            event->set_done(checkbox->checkState() == Qt::Checked);
        }
    }

    // Remove done events. Iterate backwards so indices remain valid.
    auto& events = event_repository_->events();
    for (int i = events.size() - 1; i >= 0; --i) {
        if (events[i].done()) {
            events.removeAt(i);
        }
    }

    event_repository_->Save();
    this->hide();
}

// 加载列表内的事件
void TaskListDialog::LoadEvent() {
    row_event_ids_.clear();
    auto& events = event_repository_->events();

    ui_->todolist->clearContents();
    ui_->todolist->setRowCount(events.size());
    for (int i = 0; i < events.size(); ++i) {
        row_event_ids_.append(events[i].id());

        // 控件居中显示
        QWidget* widget = new QWidget(this);
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QCheckBox* checkbox = new QCheckBox(widget);
        checkbox->setChecked(events[i].done());
        checkbox->setFixedHeight(15);  // 这个数好像15最合适
        layout->addWidget(checkbox);
        layout->setContentsMargins(0, 0, 0, 0);
        ui_->todolist->setCellWidget(i, 0, widget);

        connect(checkbox, &QCheckBox::stateChanged, this, [this, i](int state) {
            OnCheckBoxStateChanged(i, state);
        });

        ui_->todolist->setItem(
            i, 1,
            new QTableWidgetItem(events[i].text()));  // 渲染列表中存储好的事件
        ui_->todolist->item(i, 1)
            ->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);  // 设置文本居中
    }
}

void TaskListDialog::OnCheckBoxStateChanged(int row, int state) {
    if (row < 0 || row >= row_event_ids_.size()) {
        return;
    }
    desktop_todo::core::Event* event =
        event_repository_->FindEvent(row_event_ids_[row]);
    if (event != nullptr) {
        event->set_done(state == Qt::Checked);
    }
}

/// 开启了一言模式的话，拉低了性能。
void TaskListDialog::Sentence() {
//    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
//    QEventLoop loop;
//    QNetworkReply *reply = manager->get(
//            QNetworkRequest(QUrl("https://v1.hitokoto.cn/?c=k&encode=text")));
//
//    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
//    loop.exec();
//    QString text =
//            QString::fromUtf8(reply->readAll()); // readAll返回的是qbytearry
////    qDebug() << text;
//    qDebug() << reply->error();
//    ui_->showtext->setText(text);
}
