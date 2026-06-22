#ifndef DESKTOP_TODO__CORE__SETTINGS_INTERFACE_H_
#define DESKTOP_TODO__CORE__SETTINGS_INTERFACE_H_

#include <QString>
#include <QVariant>

namespace desktop_todo {
namespace core {

// Abstract interface for persistent key-value storage.
// Allows the application to swap QSettings for another backend later.
class SettingsInterface {
 public:
  virtual ~SettingsInterface() = default;

  virtual void BeginGroup(const QString& group) = 0;
  virtual void EndGroup() = 0;
  virtual void Clear() = 0;

  virtual void SetValue(const QString& key, const QVariant& value) = 0;
  virtual QVariant Value(const QString& key,
                         const QVariant& default_value = {}) const = 0;
};

}  // namespace core
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__CORE__SETTINGS_INTERFACE_H_
