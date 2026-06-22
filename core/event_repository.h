#ifndef DESKTOP_TODO__CORE__EVENT_REPOSITORY_H_
#define DESKTOP_TODO__CORE__EVENT_REPOSITORY_H_

#include <QList>
#include <QObject>
#include <memory>

#include "core/config_store.h"
#include "core/event.h"

namespace desktop_todo {
namespace core {

// In-memory owner of the event list. All mutations go through this class
// and are persisted via ConfigStore on demand.
class EventRepository : public QObject {
  Q_OBJECT

 public:
  explicit EventRepository(ConfigStore* config_store,
                         QObject* parent = nullptr);

  void Load();
  void Save();

  const QList<Event>& events() const { return events_; }
  QList<Event>& events() { return events_; }

  void AddEvent(const Event& event);
  void AddEvent(QString text, EventPriority priority);
  void RemoveEvent(int id);
  void Clear();

  // Returns a pointer to the event with the given id, or nullptr if absent.
  Event* FindEvent(int id);

  void SetEventText(int id, QString text);
  void SetEventDone(int id, bool done);
  void RemoveDoneEvents();

 signals:
  void EventAdded(int id);
  void EventRemoved(int id);
  void EventChanged(int id);
  void EventsCleared();

 private:
  int NextId() const;

  ConfigStore* config_store_ = nullptr;
  QList<Event> events_;
};

}  // namespace core
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__CORE__EVENT_REPOSITORY_H_
