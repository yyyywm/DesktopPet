#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "Struct.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Form; }
QT_END_NAMESPACE

class Form : public QWidget {
 Q_OBJECT

 public:
    explicit Form(QWidget *parent = nullptr);
    ~Form() override;

    void ShowList();

    void Sentence();

    void HideList();

 private:
    Ui::Form *ui_;

    void LoadEvent();

    QList<checkBox> status_;
};

#endif // FORM_H
