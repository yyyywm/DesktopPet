#ifndef DESKTOP_TODO__TASK_LIST_DIALOG_H_
#define DESKTOP_TODO__TASK_LIST_DIALOG_H_

#include <QWidget>
#include "Struct.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TaskListDialog; }
QT_END_NAMESPACE

class TaskListDialog : public QWidget {
 Q_OBJECT

 public:
    explicit TaskListDialog(QWidget *parent = nullptr);
    ~TaskListDialog() override;

    void ShowList();

    void Sentence();

    void HideList();

 private:
    Ui::TaskListDialog *ui_;

    void LoadEvent();

    QList<checkBox> status_;
};

#endif // DESKTOP_TODO__TASK_LIST_DIALOG_H_
