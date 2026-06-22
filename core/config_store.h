#ifndef DESKTOP_TODO__CORE__CONFIG_STORE_H_
#define DESKTOP_TODO__CORE__CONFIG_STORE_H_

#include <QList>
#include <memory>

#include "core/event.h"
#include "core/settings_interface.h"

namespace desktop_todo {
namespace core {

// Serializes and deserializes the event list to/from persistent storage.
// Keeps the legacy .ini key format: eventList/count, eventList/eventN.
class ConfigStore {
 public:
  explicit ConfigStore(std::unique_ptr<SettingsInterface> settings);

  QList<Event> LoadEvents() const;
  void SaveEvents(const QList<Event>& events);

 private:
  static constexpr char kGroup[] = "eventList";
  static constexpr char kCountKey[] = "count";
  static constexpr char kEventKeyPrefix[] = "event";

  std::unique_ptr<SettingsInterface> settings_;
};

}  // namespace core
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__CORE__CONFIG_STORE_H_
