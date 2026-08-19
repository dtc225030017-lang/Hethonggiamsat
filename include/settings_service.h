#pragma once

#include "models.h"

class DatabaseManager;

class SettingsService {
public:
    explicit SettingsService(DatabaseManager &db);
    AppSettings load() const;
    bool save(const UserSession &actor, const AppSettings &settings,
              const QString &mqttPassword, QString *error = nullptr);
    QString mqttPassword() const;
private:
    DatabaseManager &db_;
};

