#pragma once

#include "models.h"
#include <QObject>

struct mosquitto;

class MqttService : public QObject {
    Q_OBJECT
public:
    explicit MqttService(QObject *parent = nullptr);
    ~MqttService() override;
    bool start(const AppSettings &settings, const QString &password, QString *error = nullptr);
    void stop();
    bool publish(const QString &topic, const QByteArray &payload, bool retained = false);
    bool publishSettings(const AppSettings &settings);
signals:
    void connectedChanged(bool connected);
    void sensorReceived(const SensorReading &reading);
    void deviceStatusReceived(const QString &status);
    void alarmReceived(const QString &type, double value, double threshold);
    void serviceError(const QString &message);
private:
    static void onConnect(struct mosquitto *, void *, int);
    static void onDisconnect(struct mosquitto *, void *, int);
    static void onMessage(struct mosquitto *, void *, const struct mosquitto_message *);
    void handleMessage(const QString &topic, const QByteArray &payload);
    struct mosquitto *client_ = nullptr;
    bool libraryInitialized_ = false;
};

