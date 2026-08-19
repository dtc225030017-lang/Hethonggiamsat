#include "settings_service.h"
#include "app_config.h"
#include "database_manager.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QSqlError>

SettingsService::SettingsService(DatabaseManager &db) : db_(db) {}

AppSettings SettingsService::load() const
{
    AppSettings s;
    s.mq2Threshold = db_.config(QStringLiteral("mq2_ao_threshold"), QString::number(s.mq2Threshold)).toInt();
    s.temperatureHigh = db_.config(QStringLiteral("temperature_high"), QString::number(s.temperatureHigh)).toDouble();
    s.humidityHigh = db_.config(QStringLiteral("humidity_high"), QString::number(s.humidityHigh)).toDouble();
    s.humidityLow = db_.config(QStringLiteral("humidity_low"), QString::number(s.humidityLow)).toDouble();
    s.storageIntervalSeconds = db_.config(QStringLiteral("storage_interval_seconds"), QString::number(s.storageIntervalSeconds)).toInt();
    s.mqttHost = db_.config(QStringLiteral("mqtt_host"), s.mqttHost);
    s.mqttPort = db_.config(QStringLiteral("mqtt_port"), QString::number(s.mqttPort)).toInt();
    s.mqttUsername = db_.config(QStringLiteral("mqtt_username"), s.mqttUsername);
    return s;
}

bool SettingsService::save(const UserSession &actor, const AppSettings &s, const QString &password, QString *error)
{
    if (!actor.isAdmin()) { if (error) *error = QStringLiteral("Không đủ quyền ADMIN."); return false; }
    if (s.mq2Threshold < 0 || s.mq2Threshold > 4095 || s.mqttPort < 1 || s.mqttPort > 65535 ||
        s.humidityLow < 0 || s.humidityHigh > 100 || s.humidityLow >= s.humidityHigh ||
        s.storageIntervalSeconds < 1 || s.storageIntervalSeconds > 3600) {
        if (error) *error = QStringLiteral("Giá trị cấu hình không hợp lệ.");
        return false;
    }
    auto db = db_.database(); if (!db.transaction()) { if (error) *error = db.lastError().text(); return false; }
    const QList<QPair<QString, QString>> values = {
        {QStringLiteral("mq2_ao_threshold"), QString::number(s.mq2Threshold)},
        {QStringLiteral("temperature_high"), QString::number(s.temperatureHigh)},
        {QStringLiteral("humidity_high"), QString::number(s.humidityHigh)},
        {QStringLiteral("humidity_low"), QString::number(s.humidityLow)},
        {QStringLiteral("storage_interval_seconds"), QString::number(s.storageIntervalSeconds)},
        {QStringLiteral("mqtt_host"), s.mqttHost}, {QStringLiteral("mqtt_port"), QString::number(s.mqttPort)},
        {QStringLiteral("mqtt_username"), s.mqttUsername}
    };
    for (const auto &v : values) if (!db_.setConfig(v.first, v.second, error)) { db.rollback(); return false; }
    if (!db.commit()) { if (error) *error = db.lastError().text(); return false; }
    if (!password.isEmpty()) {
        const QString path = QDir(db_.rootPath()).filePath(AppConfig::MqttSecretFile);
        QSettings secret(path, QSettings::IniFormat); secret.setValue(QStringLiteral("mqtt/password"), password); secret.sync();
        QFile::setPermissions(path, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }
    qInfo() << "Configuration changed by" << actor.username;
    return true;
}

QString SettingsService::mqttPassword() const
{
    QSettings secret(QDir(db_.rootPath()).filePath(AppConfig::MqttSecretFile), QSettings::IniFormat);
    return secret.value(QStringLiteral("mqtt/password")).toString();
}
