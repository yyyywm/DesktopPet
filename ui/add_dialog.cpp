#include "add_dialog.h"
#include "Struct.h"
#include "ui_add_dialog.h"

// 这个是添加事件的窗口

/// TODO: 1. 写入配置文件，每次打开都要从配置文件中导入渲染
///       2. 鼠标悬停，随机弹出tips  （qt好像实现不了这样的效果）
///       3. 允许用户自定义图像
///       4. 对配置文件的操作很乱，可以适当优化一下

AddDialog::AddDialog(QWidget *parent)
        : QDialog(parent), ui_(new Ui::AddDialog) {
    ui_->setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint |
                   windowFlags()); // 置顶让用户方便查看其他信息
    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch); // 设置表格平分

    //  从配置文件中读取渲染到表格上
    cfg->beginGroup("eventList");
    eventCount = cfg->value("count").toInt();
    ui_->tableWidget->setRowCount(eventCount);
    ui_->tableWidget->setColumnCount(1);
    qDebug() << "dialog" << eventCount;
    for (int i = 0; i < eventCount; ++i) {
        ui_->tableWidget->setItem(
                i, 0,
                new QTableWidgetItem(
                        cfg->value("event" + QString::number(i + 1)).toString()));
        qDebug() << cfg->value("event" + QString::number(i + 1)).toString();
    }
    cfg->endGroup();

    // 确认按钮
    connect(ui_->confirm_button, &QPushButton::clicked, this, [&] {
        // 清理先前的内容，重新录入一遍
        eventlist.clear();

        // 直接读取表格内的内容添加进数组的
        for (int i = 0; i < ui_->tableWidget->rowCount(); ++i) {
            eventList node;
            node.event = ui_->tableWidget->item(i, 0)->text(); // 不能虚空添加。
            eventlist.append(node);
        }
        qDebug() << eventlist.size();

        this->hide();

        // 将数组的数据写入配置文件
        cfg->beginGroup("eventList");
        cfg->setValue("count", eventCount);
        for (int i = 0; i < eventlist.size(); ++i) {
            cfg->setValue("event" + QString::number(i + 1), eventlist[i].event);
        }
        cfg->endGroup();
    });

    // 添加按钮
    connect(ui_->add_button, &QPushButton::clicked, this, [&] {
        eventCount++;
        ui_->tableWidget->setRowCount(eventCount);
    });

    // 删除按钮

    connect(ui_->delete_button, &QPushButton::clicked, this, [&] {
        int position;                              // 默认是0
        position = ui_->tableWidget->currentRow(); // 如果没选中的话，就是-1
        if (position != -1) {
            eventCount ? eventCount-- : eventCount = 0; // 事件个数-1
            ui_->tableWidget->removeRow(position);      // 删除那一行
            eventlist.removeAt(position);               // 从列表中删除
            /// TODO: 如果删除之后，没有点击确定，而是直接点取消，那全部都会删除
            cfg->beginGroup("eventList");
            cfg->clear(); // 清空配置文件
            cfg->endGroup();
        }
    });

    // 取消按钮
    connect(ui_->cancel_button, &QPushButton::clicked, this,
            [&] { this->hide(); });

    // 读取表格上的数据进列表
    for (int i = 0; i < ui_->tableWidget->rowCount(); ++i) {
        eventList node;
        node.event =
                ui_->tableWidget->item(i, 0)->text(); // 不能虚空添加。（还没解决）
        eventlist.append(node);
    }
    qDebug() << eventlist.size();
}

AddDialog::~AddDialog() { delete ui_; }

void AddDialog::Show() {
    cfg->beginGroup("eventList");
    eventCount = cfg->value("count").toInt();
    ui_->tableWidget->setRowCount(eventCount);
    ui_->tableWidget->setColumnCount(1);
    for (int i = 0; i < eventCount; ++i) {
        ui_->tableWidget->setItem(
                i, 0,
                new QTableWidgetItem(
                        cfg->value("event" + QString::number(i + 1)).toString()));
    }
    cfg->endGroup();
    this->show();
}
