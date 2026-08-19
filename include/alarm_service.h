#pragma once

#include "models.h"
#include <QList>

class DatabaseManager;

struct AlarmRecord {
    int id = 0;
    QDateTime startTime;
    QDateTime endTime;
    QString type;
    double value = 0;
    double threshold = 0;
    double maxValue = 0;
    QString status;
};

class AlarmService {
public:
    explicit AlarmService(DatabaseManager &db);
    bool process(const SensorReading &reading, double threshold, QString *error = nullptr);
    bool externalAlarm(const QString &type, double value, double threshold,
                       QString *error = nullptr);
    QList<AlarmRecord> query(const QDateTime &from, const QDateTime &to,
                             int limit = 1000, QString *error = nullptr) const;
    bool active() const;
private:
    bool start(const QString &type, double value, double threshold, QString *error);
    bool update(double value, QString *error);
    bool finish(QString *error);
    DatabaseManager &db_;
    int activeId_ = 0;
    double maxValue_ = 0;
    int normalSamples_ = 0;
};

