#ifndef DESKTOP_TODO__CORE__EVENT_REPOSITORY_H_
#define DESKTOP_TODO__CORE__EVENT_REPOSITORY_H_

#include <QList>
#include <memory>

#include "core/config_store.h"
#include "core/event.h"

namespace desktop_todo {
namespace core {

// In-memory owner of the event list. All mutations go through this class
// and are persisted via ConfigStore on demand.
class EventRepository {
 public:
  explicit EventRepository(ConfigStore* config_store);

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

 private:
  int NextId() const;

  ConfigStore* config_store_ = nullptr;
  QList<Event> events_;
};

}  // namespace core
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__CORE__EVENT_REPOSITORY_H_
