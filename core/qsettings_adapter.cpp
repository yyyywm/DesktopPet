#include "core/qsettings_adapter.h"

namespace desktop_todo {
namespace core {

QSettingsAdapter::QSettingsAdapter(const QString& file_path)
    : settings_(std::make_unique<QSettings>(
          file_path, QSettings::IniFormat)) {}

void QSettingsAdapter::BeginGroup(const QString& group) {
  settings_->beginGroup(group);
}

void QSettingsAdapter::EndGroup() { settings_->endGroup(); }

void QSettingsAdapter::Clear() { settings_->clear(); }

void QSettingsAdapter::SetValue(const QString& key, const QVariant& value) {
  settings_->setValue(key, value);
}

QVariant QSettingsAdapter::Value(const QString& key,
                                 const QVariant& default_value) const {
  return settings_->value(key, default_value);
}

}  // namespace core
}  // namespace desktop_todo
