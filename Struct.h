
#ifndef STRUCT_H
#define STRUCT_H

#include <QCheckBox>
#include <QSettings>

struct eventList {
    QCheckBox *done = new QCheckBox;
    QString event;
};

struct checkBox {
    QCheckBox *cbox = new QCheckBox;
};

//QSettings *cfg;
static QList<eventList> eventlist;
static int eventCount;
static QSettings *cfg = new QSettings("../config/eventlist.ini", QSettings::IniFormat);  //保存.ini路径

#endif // STRUCT_H
