#pragma once

#include <QDateTime>
#include <QString>

struct UserSession {
    int id = 0;
    QString username;
    QString fullName;
    QString role;
    bool enabled = false;
    bool isAdmin() const { return role == QStringLiteral("ADMIN"); }
};

struct SensorReading {
    QDateTime timestamp = QDateTime::currentDateTime();
    double temperature = 0.0;
    double humidity = 0.0;
    int mq2Raw = 0;
    int mq2Mv = 0;
    int mq2Baseline = 0;
    int mq2Threshold = 0;
    bool mq2Ready = true;
    bool mq2DigitalMode = false;
    bool mq2Detected = false;
    QString status = QStringLiteral("NORMAL");
    QString deviceId;
    bool alarm = false;
    QString alarmType = QStringLiteral("NONE");
};

struct AppSettings {
    int mq2Threshold = 2500;
    double temperatureHigh = 50.0;
    double humidityHigh = 90.0;
    double humidityLow = 20.0;
    int storageIntervalSeconds = 5;
    QString mqttHost = QStringLiteral("127.0.0.1");
    int mqttPort = 1883;
    QString mqttUsername = QStringLiteral("air_app");
};
