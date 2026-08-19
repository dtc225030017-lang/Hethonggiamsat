#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();
    bool initialize(const QString &rootPath, QString *error = nullptr);
    QSqlDatabase database() const;
    QString rootPath() const;
    bool setConfig(const QString &key, const QString &value, QString *error = nullptr);
    QString config(const QString &key, const QString &defaultValue = {}) const;
private:
    bool createSchema(QString *error);
    QString connectionName_;
    QString rootPath_;
};

