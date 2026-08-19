#include "mqtt_service.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QUuid>
#include <mosquitto.h>

MqttService::MqttService(QObject *parent) : QObject(parent)
{
    libraryInitialized_ = mosquitto_lib_init() == MOSQ_ERR_SUCCESS;
}

MqttService::~MqttService() { stop(); if (libraryInitialized_) mosquitto_lib_cleanup(); }

bool MqttService::start(const AppSettings &s, const QString &password, QString *error)
{
    stop();
    if (!libraryInitialized_) { if (error) *error = QStringLiteral("Không khởi tạo được libmosquitto."); return false; }
    const QByteArray id = QStringLiteral("Hethonggiamsat_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8)).toUtf8();
    client_ = mosquitto_new(id.constData(), true, this);
    if (!client_) { if (error) *error = QStringLiteral("Không tạo được MQTT client."); return false; }
    mosquitto_connect_callback_set(client_, &MqttService::onConnect);
    mosquitto_disconnect_callback_set(client_, &MqttService::onDisconnect);
    mosquitto_message_callback_set(client_, &MqttService::onMessage);
    mosquitto_reconnect_delay_set(client_, 1, 30, true);
    if (!s.mqttUsername.isEmpty()) {
        const QByteArray user = s.mqttUsername.toUtf8(), pass = password.toUtf8();
        mosquitto_username_pw_set(client_, user.constData(), pass.isEmpty() ? nullptr : pass.constData());
    }
    const QByteArray host = s.mqttHost.toUtf8();
    int rc = mosquitto_connect_async(client_, host.constData(), s.mqttPort, 20);
    if (rc == MOSQ_ERR_SUCCESS) rc = mosquitto_loop_start(client_);
    if (rc != MOSQ_ERR_SUCCESS) { if (error) *error = QString::fromUtf8(mosquitto_strerror(rc)); stop(); return false; }
    qInfo() << "MQTT connecting" << s.mqttHost << s.mqttPort;
    return true;
}

void MqttService::stop()
{
    if (!client_) return;
    mosquitto_disconnect(client_); mosquitto_loop_stop(client_, true); mosquitto_destroy(client_); client_ = nullptr;
}

bool MqttService::publish(const QString &topic, const QByteArray &payload, bool retained)
{
    if (!client_) return false;
    const QByteArray t = topic.toUtf8();
    return mosquitto_publish(client_, nullptr, t.constData(), int(payload.size()), payload.constData(), 1, retained) == MOSQ_ERR_SUCCESS;
}

bool MqttService::publishSettings(const AppSettings &s)
{
    bool ok = publish(QStringLiteral("air/config/mq2_ao_threshold"), QByteArray::number(s.mq2Threshold), true);
    ok &= publish(QStringLiteral("air/config/temp_high"), QByteArray::number(s.temperatureHigh), true);
    ok &= publish(QStringLiteral("air/config/humidity_high"), QByteArray::number(s.humidityHigh), true);
    ok &= publish(QStringLiteral("air/config/humidity_low"), QByteArray::number(s.humidityLow), true);
    return ok;
}

void MqttService::onConnect(mosquitto *m, void *obj, int rc)
{
    auto *self = static_cast<MqttService *>(obj);
    if (rc == 0) {
        mosquitto_subscribe(m, nullptr, "air/sensor/data", 1); mosquitto_subscribe(m, nullptr, "air/status", 1); mosquitto_subscribe(m, nullptr, "air/alarm", 1);
    }
    QMetaObject::invokeMethod(self, [self, rc] { emit self->connectedChanged(rc == 0); if (rc != 0) emit self->serviceError(QStringLiteral("MQTT connect: %1").arg(QString::fromUtf8(mosquitto_connack_string(rc)))); }, Qt::QueuedConnection);
}

void MqttService::onDisconnect(mosquitto *, void *obj, int rc)
{
    auto *self = static_cast<MqttService *>(obj);
    QMetaObject::invokeMethod(self, [self, rc] { emit self->connectedChanged(false); qWarning() << "MQTT disconnected" << rc; }, Qt::QueuedConnection);
}

void MqttService::onMessage(mosquitto *, void *obj, const mosquitto_message *msg)
{
    auto *self = static_cast<MqttService *>(obj); const QString topic = QString::fromUtf8(msg->topic);
    const QByteArray payload(static_cast<const char *>(msg->payload), msg->payloadlen);
    QMetaObject::invokeMethod(self, [self, topic, payload] { self->handleMessage(topic, payload); }, Qt::QueuedConnection);
}

void MqttService::handleMessage(const QString &topic, const QByteArray &payload)
{
    if (topic == QStringLiteral("air/status")) { emit deviceStatusReceived(QString::fromUtf8(payload).trimmed().toUpper()); return; }
    QJsonParseError parseError; const auto doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) { emit serviceError(QStringLiteral("JSON MQTT không hợp lệ trên %1").arg(topic)); return; }
    const auto o = doc.object();
    if (topic == QStringLiteral("air/sensor/data")) {
        SensorReading r; r.timestamp = QDateTime::currentDateTime(); r.deviceId = o.value(QStringLiteral("device_id")).toString();
        r.temperature = o.value(QStringLiteral("temperature")).toDouble(); r.humidity = o.value(QStringLiteral("humidity")).toDouble();
        r.mq2Raw = o.value(QStringLiteral("mq2_raw")).toInt(); r.mq2Mv = o.value(QStringLiteral("mq2_mv")).toInt();
        r.mq2Baseline = o.value(QStringLiteral("mq2_baseline")).toInt();
        r.mq2Threshold = o.value(QStringLiteral("mq2_threshold")).toInt();
        r.mq2Ready = !o.contains(QStringLiteral("mq2_ready")) || o.value(QStringLiteral("mq2_ready")).toBool();
        r.mq2DigitalMode = o.value(QStringLiteral("mq2_mode")).toString() == QStringLiteral("DO");
        r.mq2Detected = o.value(QStringLiteral("mq2_digital")).toBool();
        r.alarm = o.value(QStringLiteral("alarm")).toBool(); r.alarmType = o.value(QStringLiteral("alarm_type")).toString(QStringLiteral("NONE"));
        r.status = r.alarm ? QStringLiteral("ALARM") : QStringLiteral("NORMAL"); emit sensorReceived(r);
    } else if (topic == QStringLiteral("air/alarm")) {
        emit alarmReceived(o.value(QStringLiteral("type")).toString(QStringLiteral("MQ2_HIGH")), o.value(QStringLiteral("value")).toDouble(), o.value(QStringLiteral("threshold")).toDouble());
    }
}
