#include "add_dialog.h"
#include "ui_add_dialog.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidgetItem>

// 这个是添加事件的窗口

/// TODO: 1. 写入配置文件，每次打开都要从配置文件中导入渲染
///       2. 鼠标悬停，随机弹出tips  （qt好像实现不了这样的效果）
///       3. 允许用户自定义图像
///       4. 对配置文件的操作很乱，可以适当优化一下

AddDialog::AddDialog(
    QWidget *parent,
    desktop_todo::core::EventRepository* event_repository)
    : QDialog(parent),
      ui_(new Ui::AddDialog),
      event_repository_(event_repository) {
    ui_->setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint |
                   windowFlags());  // 置顶让用户方便查看其他信息
    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);  // 设置表格平分

    // 确认按钮
    connect(ui_->confirm_button, &QPushButton::clicked, this, [&] {
        SaveToRepository();
        this->hide();
    });

    // 添加按钮
    connect(ui_->add_button, &QPushButton::clicked, this, [&] {
        const int new_row = ui_->tableWidget->rowCount();
        ui_->tableWidget->setRowCount(new_row + 1);
        ui_->tableWidget->setItem(new_row, 0, new QTableWidgetItem());
    });

    // 删除按钮
    connect(ui_->delete_button, &QPushButton::clicked, this, [&] {
        const int position = ui_->tableWidget->currentRow();
        if (position != -1) {
            ui_->tableWidget->removeRow(position);  // 删除那一行
        }
    });

    // 取消按钮
    connect(ui_->cancel_button, &QPushButton::clicked, this,
            [&] { this->hide(); });
}

AddDialog::~AddDialog() { delete ui_; }

void AddDialog::Show() {
    LoadFromRepository();
    this->show();
}

void AddDialog::LoadFromRepository() {
    ui_->tableWidget->clearContents();
    const auto& events = event_repository_->events();
    ui_->tableWidget->setRowCount(events.size());
    ui_->tableWidget->setColumnCount(1);
    for (int i = 0; i < events.size(); ++i) {
        ui_->tableWidget->setItem(
            i, 0, new QTableWidgetItem(events[i].text()));
    }
}

void AddDialog::SaveToRepository() {
    event_repository_->Clear();
    for (int i = 0; i < ui_->tableWidget->rowCount(); ++i) {
        event_repository_->AddEvent(RowText(i),
                                     desktop_todo::core::EventPriority::kNormal);
    }
    event_repository_->Save();
}

QString AddDialog::RowText(int row) const {
    QTableWidgetItem* item = ui_->tableWidget->item(row, 0);
    if (item == nullptr) {
        return QString();
    }
    return item->text();
}
