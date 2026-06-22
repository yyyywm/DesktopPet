#ifndef DESKTOP_TODO__ADD_DIALOG_H_
#define DESKTOP_TODO__ADD_DIALOG_H_

#include <QDialog>
#include "Struct.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AddDialog;
}
QT_END_NAMESPACE

class AddDialog : public QDialog {
 Q_OBJECT

 public:
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

    void Show();
 private:
    Ui::AddDialog *ui_;

};

#endif // DESKTOP_TODO__ADD_DIALOG_H_
