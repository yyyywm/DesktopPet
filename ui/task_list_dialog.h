#ifndef DESKTOP_TODO__TASK_LIST_DIALOG_H_
#define DESKTOP_TODO__TASK_LIST_DIALOG_H_

#include <QWidget>

#include "core/event_repository.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TaskListDialog; }
QT_END_NAMESPACE

class TaskListDialog : public QWidget {
 Q_OBJECT

 public:
    explicit TaskListDialog(
        QWidget *parent,
        desktop_todo::core::EventRepository* event_repository);
    ~TaskListDialog() override;

    void ShowList();

    void Sentence();

    void HideList();

 private:
    void LoadEvent();
    void OnCheckBoxStateChanged(int row, int state);

    Ui::TaskListDialog *ui_;
    desktop_todo::core::EventRepository* event_repository_ = nullptr;
    QList<int> row_event_ids_;
};

#endif  // DESKTOP_TODO__TASK_LIST_DIALOG_H_
