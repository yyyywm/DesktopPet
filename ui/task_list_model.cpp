#include "ui/task_list_model.h"

namespace desktop_todo {
namespace ui {

TaskListModel::TaskListModel(core::EventRepository* repository, QObject* parent)
    : QAbstractListModel(parent), repository_(repository) {
  ConnectRepository();
}

int TaskListModel::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return repository_ == nullptr ? 0 : repository_->events().size();
}

QVariant TaskListModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || repository_ == nullptr) {
    return QVariant();
  }
  const int row = index.row();
  const auto& events = repository_->events();
  if (row < 0 || row >= events.size()) {
    return QVariant();
  }
  const core::Event& event = events[row];
  switch (role) {
    case Qt::DisplayRole:
      return event.text();
    case IdRole:
      return event.id();
    case TextRole:
      return event.text();
    case DoneRole:
      return event.done();
    case PriorityRole:
      return static_cast<int>(event.priority());
    default:
      return QVariant();
  }
}

bool TaskListModel::setData(const QModelIndex& index, const QVariant& value,
                            int role) {
  if (!index.isValid() || repository_ == nullptr) {
    return false;
  }
  const int row = index.row();
  auto& events = repository_->events();
  if (row < 0 || row >= events.size()) {
    return false;
  }
  const int id = events[row].id();
  switch (role) {
    case TextRole:
      repository_->SetEventText(id, value.toString());
      emit dataChanged(index, index, {TextRole});
      return true;
    case DoneRole:
      repository_->SetEventDone(id, value.toBool());
      emit dataChanged(index, index, {DoneRole});
      return true;
    default:
      return false;
  }
}

Qt::ItemFlags TaskListModel::flags(const QModelIndex& index) const {
  if (!index.isValid()) {
    return QAbstractListModel::flags(index);
  }
  return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> TaskListModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[IdRole] = "eventId";
  roles[TextRole] = "text";
  roles[DoneRole] = "done";
  roles[PriorityRole] = "priority";
  return roles;
}

void TaskListModel::addEvent(const QString& text) {
  if (repository_ == nullptr) {
    return;
  }
  const int row = rowCount();
  beginInsertRows(QModelIndex(), row, row);
  repository_->AddEvent(text, core::EventPriority::kNormal);
  endInsertRows();
}

void TaskListModel::removeDoneEvents() {
  if (repository_ == nullptr) {
    return;
  }
  beginResetModel();
  repository_->RemoveDoneEvents();
  endResetModel();
}

void TaskListModel::clear() {
  if (repository_ == nullptr) {
    return;
  }
  beginResetModel();
  repository_->Clear();
  endResetModel();
}

void TaskListModel::save() {
  if (repository_ == nullptr) {
    return;
  }
  repository_->Save();
}

void TaskListModel::ConnectRepository() {
  if (repository_ == nullptr) {
    return;
  }
  connect(repository_, &core::EventRepository::EventChanged, this,
          [this](int id) {
            const int row = FindRowById(id);
            if (row < 0) {
              return;
            }
            emit dataChanged(index(row), index(row));
          });
}

int TaskListModel::FindRowById(int id) const {
  if (repository_ == nullptr) {
    return -1;
  }
  const auto& events = repository_->events();
  for (int i = 0; i < events.size(); ++i) {
    if (events[i].id() == id) {
      return i;
    }
  }
  return -1;
}

}  // namespace ui
}  // namespace desktop_todo
