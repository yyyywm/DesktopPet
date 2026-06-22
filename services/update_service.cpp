#include "services/update_service.h"

#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>

namespace desktop_todo {
namespace services {

UpdateService::UpdateService(QObject* parent) : QObject(parent) {
    connect(&network_manager_, &QNetworkAccessManager::finished, this,
        &UpdateService::OnReplyFinished);
}

void UpdateService::CheckForUpdates() {
    network_manager_.get(QNetworkRequest(QUrl(kUpdateUrl)));
}

void UpdateService::OnReplyFinished(QNetworkReply* reply) {
    if (reply == nullptr) {
        emit CheckFailed(QStringLiteral("Empty network reply."));
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit CheckFailed(reply->errorString());
        reply->deleteLater();
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    if (!document.isObject()) {
        emit CheckFailed(QStringLiteral("Invalid update response format."));
        return;
    }

    const QJsonObject object = document.object();
    const double version = object.value(QStringLiteral("version")).toDouble();
    const QString date = object.value(QStringLiteral("releaseDate")).toString();
    const QString url = object.value(QStringLiteral("url")).toString();
    const QString log = object.value(QStringLiteral("logs")).toString();

    if (version == kCurrentVersion) {
        emit UpToDate();
    } else {
        emit UpdateAvailable(date, log, url);
    }
}

}  // namespace services
}  // namespace desktop_todo
