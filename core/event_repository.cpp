#include "core/event_repository.h"

#include <algorithm>

namespace desktop_todo {
namespace core {

EventRepository::EventRepository(ConfigStore* config_store)
    : config_store_(config_store) {}

void EventRepository::Load() {
  events_ = config_store_->LoadEvents();
  // Ensure every loaded event has a valid id.
  for (int i = 0; i < events_.size(); ++i) {
    if (events_[i].id() <= 0) {
      events_[i].set_id(i + 1);
    }
  }
}

void EventRepository::Save() {
  if (config_store_ != nullptr) {
    config_store_->SaveEvents(events_);
  }
}

void EventRepository::AddEvent(const Event& event) {
  Event copy = event;
  if (copy.id() <= 0) {
    copy.set_id(NextId());
  }
  events_.append(copy);
}

void EventRepository::AddEvent(QString text, EventPriority priority) {
  Event event;
  event.set_id(NextId());
  event.set_text(std::move(text));
  event.set_done(false);
  event.set_priority(priority);
  events_.append(event);
}

void EventRepository::RemoveEvent(int id) {
  events_.erase(
      std::remove_if(events_.begin(), events_.end(),
                     [id](const Event& event) { return event.id() == id; }),
      events_.end());
}

void EventRepository::Clear() { events_.clear(); }

Event* EventRepository::FindEvent(int id) {
  for (Event& event : events_) {
    if (event.id() == id) {
      return &event;
    }
  }
  return nullptr;
}

int EventRepository::NextId() const {
  int max_id = 0;
  for (const Event& event : events_) {
    if (event.id() > max_id) {
      max_id = event.id();
    }
  }
  return max_id + 1;
}

}  // namespace core
}  // namespace desktop_todo
