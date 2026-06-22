#include "core/event_repository.h"

namespace desktop_todo {
namespace core {

EventRepository::EventRepository(ConfigStore* config_store,
                               QObject* parent)
    : QObject(parent), config_store_(config_store) {}

void EventRepository::Load() {
  if (config_store_ == nullptr) {
    return;
  }
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
  emit EventAdded(events_.last().id());
}

void EventRepository::AddEvent(QString text, EventPriority priority) {
  Event event;
  event.set_id(NextId());
  event.set_text(std::move(text));
  event.set_done(false);
  event.set_priority(priority);
  events_.append(event);
  emit EventAdded(events_.last().id());
}

void EventRepository::RemoveEvent(int id) {
  for (int i = events_.size() - 1; i >= 0; --i) {
    if (events_[i].id() == id) {
      events_.removeAt(i);
      emit EventRemoved(id);
      break;
    }
  }
}

void EventRepository::Clear() {
  events_.clear();
  emit EventsCleared();
}

void EventRepository::SetEventText(int id, QString text) {
  Event* event = FindEvent(id);
  if (event == nullptr) {
    return;
  }
  event->set_text(std::move(text));
  emit EventChanged(id);
}

void EventRepository::SetEventDone(int id, bool done) {
  Event* event = FindEvent(id);
  if (event == nullptr) {
    return;
  }
  event->set_done(done);
  emit EventChanged(id);
}

void EventRepository::RemoveDoneEvents() {
  for (int i = events_.size() - 1; i >= 0; --i) {
    if (events_[i].done()) {
      const int id = events_[i].id();
      events_.removeAt(i);
      emit EventRemoved(id);
    }
  }
}

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
