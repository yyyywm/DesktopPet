#ifndef DESKTOP_TODO__CORE__QSETTINGS_ADAPTER_H_
#define DESKTOP_TODO__CORE__QSETTINGS_ADAPTER_H_

#include <QSettings>
#include <memory>

#include "core/settings_interface.h"

namespace desktop_todo {
namespace core {

// QSettings-backed implementation of SettingsInterface.
class QSettingsAdapter : public SettingsInterface {
 public:
  explicit QSettingsAdapter(const QString& file_path);

  void BeginGroup(const QString& group) override;
  void EndGroup() override;
  void Clear() override;

  void SetValue(const QString& key, const QVariant& value) override;
  QVariant Value(const QString& key,
                 const QVariant& default_value = {}) const override;

 private:
  std::unique_ptr<QSettings> settings_;
};

}  // namespace core
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__CORE__QSETTINGS_ADAPTER_H_
