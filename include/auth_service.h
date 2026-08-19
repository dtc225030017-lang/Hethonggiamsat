#pragma once

#include "models.h"
#include <QList>

class DatabaseManager;

class AuthService {
public:
    explicit AuthService(DatabaseManager &db);
    bool ensureBootstrapAdmin(QString *message = nullptr);
    bool authenticate(const QString &username, const QString &password,
                      UserSession *session, QString *error = nullptr);
    QList<UserSession> users(QString *error = nullptr) const;
    bool addUser(const UserSession &actor, const QString &username,
                 const QString &password, const QString &fullName,
                 const QString &role, QString *error = nullptr);
    bool updateUser(const UserSession &actor, int userId, const QString &fullName,
                    const QString &role, bool enabled, QString *error = nullptr);
    bool changePassword(const UserSession &actor, int userId,
                        const QString &newPassword, QString *error = nullptr);
    static bool validPassword(const QString &password, QString *error = nullptr);
private:
    static QString hashPassword(const QString &password);
    static bool verifyPassword(const QString &password, const QString &encoded);
    DatabaseManager &db_;
};

