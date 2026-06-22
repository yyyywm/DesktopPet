#include "task_list_dialog.h"
#include "Struct.h"
#include "ui_task_list_dialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

// 这个是查看事件的窗口

TaskListDialog::TaskListDialog(QWidget *parent) : QWidget(parent), ui_(new Ui::TaskListDialog) {
    ui_->setupUi(this);
    ui_->todolist->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch); // 设置表格平分
    //    ui_->todolist->setColumnWidth(0, 80);
    ui_->todolist->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

TaskListDialog::~TaskListDialog() { delete ui_; }

//  显示任务列表
void TaskListDialog::ShowList() {
    LoadEvent();
    Sentence();
    this->setWindowFlags(Qt::CoverWindow | Qt::FramelessWindowHint |
                         windowFlags()); // 设置窗口置顶，边框隐藏
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口透明
    this->show();
}

/// TODO: 1.
/// 关闭窗口的时候判断checkbox，然后删除eventlist里面的内容，再重写配置文件

void TaskListDialog::HideList() {
    for (int i = 0; i < eventCount; ++i) {
        if (eventlist[i].done->checkState() == Qt::Checked) {
            eventlist.removeAt(i);
            eventCount--;
            qDebug() << "delet: " << i;
        }
    }
    cfg->beginGroup("eventList");
    cfg->clear();
    cfg->setValue("count", eventCount);
    for (int i = 0; i < eventCount; ++i) {
        cfg->setValue("event" + QString::number(i + 1), eventlist[i].event);
    }
    cfg->endGroup();
    this->hide();
}

// 加载列表内的事件
void TaskListDialog::LoadEvent() {
    cfg->beginGroup("eventList");
    eventCount = cfg->value("count").toInt(); // 先从配置文件中获取事件的个数
    eventlist.clear();

    for (int i = 0; i < eventCount; ++i) {
        eventList node;
        node.done->setCheckState(Qt::Unchecked);
        node.event = cfg->value("event" + QString::number(i + 1)).toString();
        eventlist.append(node); // 从配置文件中读取事件存到列表中

//        disconnect(eventlist[i].done, 0, 0, 0);
//        connect(eventlist[i].done, &QCheckBox::stateChanged, this, [&, i] {
////            ui_->tableWidget->removeRow(i);      // 删除那一行
//            eventlist.removeAt(i);               // 从事件列表中删除
//            eventCount -= eventCount;
//
////            qDebug() << eventlist[i].event;
//        });
    }
    cfg->endGroup();

    ui_->todolist->setRowCount(eventCount);
    for (int i = 0; i < eventCount; ++i) {
        // 控件居中显示
        QWidget *widget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        eventlist[i].done->setFixedHeight(15);          // 这个数好像15最合适
        layout->addWidget(eventlist[i].done);
        widget->setLayout(layout);
        ui_->todolist->setCellWidget(i, 0, widget);

        ui_->todolist->setItem(
                i, 1,
                new QTableWidgetItem(eventlist[i].event)); // 渲染列表中存储好的事件
        ui_->todolist->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);  // 设置文本居中

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