#ifndef DESKTOP_TODO__UI__TASK_LIST_MODEL_H_
#define DESKTOP_TODO__UI__TASK_LIST_MODEL_H_

#include <QAbstractListModel>
#include <QHash>

#include "core/event_repository.h"

namespace desktop_todo {
namespace ui {

class TaskListModel : public QAbstractListModel {
  Q_OBJECT

 public:
  explicit TaskListModel(core::EventRepository* repository,
                         QObject* parent = nullptr);

  enum Role {
    IdRole = Qt::UserRole + 1,
    TextRole,
    DoneRole,
    PriorityRole
  };
  Q_ENUM(Role)

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value,
               int role) override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void addEvent(const QString& text);
  Q_INVOKABLE void removeDoneEvents();
  Q_INVOKABLE void clear();
  Q_INVOKABLE void save();

 private:
  void ConnectRepository();
  int FindRowById(int id) const;

  core::EventRepository* repository_ = nullptr;
};

}  // namespace ui
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__UI__TASK_LIST_MODEL_H_
