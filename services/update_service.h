#ifndef DESKTOP_TODO__SERVICES__UPDATE_SERVICE_H_
#define DESKTOP_TODO__SERVICES__UPDATE_SERVICE_H_

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

namespace desktop_todo {
namespace services {

// Asynchronously checks whether a newer version of the application is available.
class UpdateService : public QObject {
  Q_OBJECT

 public:
  explicit UpdateService(QObject* parent = nullptr);

  // Initiates an asynchronous version check. Emits one of the signals below
  // when the check completes.
  void CheckForUpdates();

 signals:
  void UpToDate();
  void UpdateAvailable(const QString& release_date,
                       const QString& log,
                       const QString& url);
  void CheckFailed(const QString& reason);

 private slots:
  void OnReplyFinished(QNetworkReply* reply);

 private:
  static constexpr double kCurrentVersion = 0.1;
  static constexpr const char* kUpdateUrl =
      "http://rap2api.taobao.org/app/mock/315262/update/666";

  QNetworkAccessManager network_manager_;
};

}  // namespace services
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__SERVICES__UPDATE_SERVICE_H_
