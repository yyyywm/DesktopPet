#ifndef DESKTOP_TODO__CORE__EVENT_H_
#define DESKTOP_TODO__CORE__EVENT_H_

#include <QString>

namespace desktop_todo {
namespace core {

// Priority of a todo event. Lower numeric value means lower priority.
enum class EventPriority {
  kLow = 0,
  kNormal = 1,
  kHigh = 2,
};

// Domain model for a single todo event.
class Event {
 public:
  Event() = default;
  Event(int id, QString text, bool done, EventPriority priority)
      : id_(id),
        text_(std::move(text)),
        done_(done),
        priority_(priority) {}

  int id() const { return id_; }
  void set_id(int id) { id_ = id; }

  const QString& text() const { return text_; }
  void set_text(QString text) { text_ = std::move(text); }

  bool done() const { return done_; }
  void set_done(bool done) { done_ = done; }

  EventPriority priority() const { return priority_; }
  void set_priority(EventPriority priority) { priority_ = priority; }

  bool operator==(const Event& other) const {
    return id_ == other.id_ && text_ == other.text_ && done_ == other.done_ &&
           priority_ == other.priority_;
  }

  bool operator!=(const Event& other) const { return !(*this == other); }

 private:
  int id_ = -1;
  QString text_;
  bool done_ = false;
  EventPriority priority_ = EventPriority::kNormal;
};

}  // namespace core
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__CORE__EVENT_H_
