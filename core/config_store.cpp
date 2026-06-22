#include "core/config_store.h"

#include <QString>

namespace desktop_todo {
namespace core {

ConfigStore::ConfigStore(std::unique_ptr<SettingsInterface> settings)
    : settings_(std::move(settings)) {}

QList<Event> ConfigStore::LoadEvents() const {
  QList<Event> events;
  settings_->BeginGroup(kGroup);
  const int count = settings_->Value(kCountKey).toInt();
  for (int i = 0; i < count; ++i) {
    const QString key = QStringLiteral("%1%2").arg(kEventKeyPrefix).arg(i + 1);
    Event event;
    event.set_id(i + 1);
    event.set_text(settings_->Value(key).toString());
    event.set_done(false);
    event.set_priority(EventPriority::kNormal);
    events.append(event);
  }
  settings_->EndGroup();
  return events;
}

void ConfigStore::SaveEvents(const QList<Event>& events) {
  settings_->BeginGroup(kGroup);
  settings_->Clear();
  settings_->SetValue(kCountKey, events.size());
  for (int i = 0; i < events.size(); ++i) {
    const QString key =
        QStringLiteral("%1%2").arg(kEventKeyPrefix).arg(i + 1);
    settings_->SetValue(key, events[i].text());
  }
  settings_->EndGroup();
}

}  // namespace core
}  // namespace desktop_todo
