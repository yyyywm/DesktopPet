#ifndef DESKTOP_TODO__ADD_DIALOG_H_
#define DESKTOP_TODO__ADD_DIALOG_H_

#include <QDialog>

#include "core/event_repository.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AddDialog;
}
QT_END_NAMESPACE

class AddDialog : public QDialog {
 Q_OBJECT

 public:
    explicit AddDialog(
        QWidget *parent,
        desktop_todo::core::EventRepository* event_repository);
    ~AddDialog();

    void Show();

 private:
    void LoadFromRepository();
    void SaveToRepository();
    QString RowText(int row) const;

    Ui::AddDialog *ui_;
    desktop_todo::core::EventRepository* event_repository_ = nullptr;
};

#endif  // DESKTOP_TODO__ADD_DIALOG_H_
