#pragma once

#include "models.h"
#include <QList>

class DatabaseManager;

class SensorRepository {
public:
    explicit SensorRepository(DatabaseManager &db);
    bool insert(const SensorReading &reading, QString *error = nullptr);
    QList<SensorReading> query(const QDateTime &from, const QDateTime &to,
                               int limit = 1000, QString *error = nullptr) const;
private:
    DatabaseManager &db_;
};

