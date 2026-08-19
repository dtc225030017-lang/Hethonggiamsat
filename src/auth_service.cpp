#include "auth_service.h"
#include "database_manager.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMessageAuthenticationCode>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>

namespace {
QByteArray pbkdf2(const QByteArray &password, const QByteArray &salt, int iterations, int size)
{
    QByteArray result;
    for (quint32 block = 1; result.size() < size; ++block) {
        QByteArray input = salt;
        input.append(char((block >> 24) & 0xff)); input.append(char((block >> 16) & 0xff));
        input.append(char((block >> 8) & 0xff)); input.append(char(block & 0xff));
        QByteArray u = QMessageAuthenticationCode::hash(input, password, QCryptographicHash::Sha256);
        QByteArray t = u;
        for (int i = 1; i < iterations; ++i) {
            u = QMessageAuthenticationCode::hash(u, password, QCryptographicHash::Sha256);
            for (qsizetype j = 0; j < t.size(); ++j) t[j] = char(t[j] ^ u[j]);
        }
        result += t;
    }
    return result.left(size);
}

QString randomPassword()
{
    static const QString chars = QStringLiteral("ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789!@#");
    QString out;
    for (int i = 0; i < 16; ++i) out += chars.at(QRandomGenerator::system()->bounded(chars.size()));
    return out;
}
}

AuthService::AuthService(DatabaseManager &db) : db_(db) {}

QString AuthService::hashPassword(const QString &password)
{
    QByteArray salt(16, Qt::Uninitialized);
    for (auto &c : salt) c = char(QRandomGenerator::system()->bounded(256));
    constexpr int iterations = 120000;
    const QByteArray derived = pbkdf2(password.toUtf8(), salt, iterations, 32);
    return QStringLiteral("pbkdf2_sha256$%1$%2$%3").arg(iterations)
        .arg(QString::fromLatin1(salt.toHex()), QString::fromLatin1(derived.toHex()));
}

bool AuthService::verifyPassword(const QString &password, const QString &encoded)
{
    const auto parts = encoded.split('$');
    if (parts.size() != 4 || parts[0] != QStringLiteral("pbkdf2_sha256")) return false;
    bool ok = false; const int iterations = parts[1].toInt(&ok);
    if (!ok || iterations < 10000) return false;
    const QByteArray salt = QByteArray::fromHex(parts[2].toLatin1());
    const QByteArray expected = QByteArray::fromHex(parts[3].toLatin1());
    const QByteArray actual = pbkdf2(password.toUtf8(), salt, iterations, expected.size());
    quint8 diff = quint8(actual.size() ^ expected.size());
    for (qsizetype i = 0; i < qMin(actual.size(), expected.size()); ++i) diff |= quint8(actual[i] ^ expected[i]);
    return diff == 0;
}

bool AuthService::validPassword(const QString &password, QString *error)
{
    if (password.size() < 8 || !password.contains(QRegularExpression(QStringLiteral("[A-Za-z]"))) ||
        !password.contains(QRegularExpression(QStringLiteral("[0-9]")))) {
        if (error) *error = QStringLiteral("Mật khẩu phải có ít nhất 8 ký tự, gồm chữ và số.");
        return false;
    }
    return true;
}

bool AuthService::ensureBootstrapAdmin(QString *message)
{
    QSqlQuery count(db_.database());
    if (!count.exec(QStringLiteral("SELECT COUNT(*) FROM tai_khoan")) || !count.next()) return false;
    if (count.value(0).toInt() > 0) return true;
    QString password = QString::fromUtf8(qgetenv("HETHONGGIAMSAT_BOOTSTRAP_PASSWORD"));
    if (password.isEmpty()) password = randomPassword();
    const QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("INSERT INTO tai_khoan(username,password_hash,ho_ten,role,enabled,created_at,updated_at) VALUES(?,?,?,?,1,?,?)"));
    q.addBindValue(QStringLiteral("admin")); q.addBindValue(hashPassword(password));
    q.addBindValue(QStringLiteral("Quản trị hệ thống")); q.addBindValue(QStringLiteral("ADMIN"));
    q.addBindValue(now); q.addBindValue(now);
    if (!q.exec()) return false;
    const QString credentialFile = QDir(db_.rootPath()).filePath(QStringLiteral("data/initial_admin.txt"));
    QFile file(credentialFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        file.write("username=admin\npassword="); file.write(password.toUtf8()); file.write("\n");
        file.close();
        file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }
    if (message) *message = QStringLiteral("Đã tạo tài khoản admin ban đầu; thông tin nằm tại %1 (mode 0600).").arg(credentialFile);
    return true;
}

bool AuthService::authenticate(const QString &username, const QString &password, UserSession *session, QString *error)
{
    QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("SELECT id,username,password_hash,ho_ten,role,enabled FROM tai_khoan WHERE username=?"));
    q.addBindValue(username.trimmed());
    if (!q.exec() || !q.next() || !verifyPassword(password, q.value(2).toString())) {
        qWarning() << "Login failed for" << username;
        if (error) *error = QStringLiteral("Sai tên đăng nhập hoặc mật khẩu.");
        return false;
    }
    if (!q.value(5).toBool()) { if (error) *error = QStringLiteral("Tài khoản đã bị khóa."); return false; }
    if (session) *session = {q.value(0).toInt(), q.value(1).toString(), q.value(3).toString(), q.value(4).toString(), true};
    qInfo() << "Login successful for" << username;
    return true;
}

QList<UserSession> AuthService::users(QString *error) const
{
    QList<UserSession> out; QSqlQuery q(db_.database());
    if (!q.exec(QStringLiteral("SELECT id,username,ho_ten,role,enabled FROM tai_khoan ORDER BY username"))) {
        if (error) *error = q.lastError().text();
        return out;
    }
    while (q.next()) out.append({q.value(0).toInt(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toBool()});
    return out;
}

bool AuthService::addUser(const UserSession &actor, const QString &username, const QString &password,
                          const QString &fullName, const QString &role, QString *error)
{
    if (!actor.isAdmin()) { if (error) *error = QStringLiteral("Không đủ quyền ADMIN."); return false; }
    if (username.trimmed().size() < 3 || !QRegularExpression(QStringLiteral("^[A-Za-z0-9_.-]+$")).match(username).hasMatch()) {
        if (error) *error = QStringLiteral("Username không hợp lệ.");
        return false;
    }
    if (role != QStringLiteral("ADMIN") && role != QStringLiteral("USER")) { if (error) *error = QStringLiteral("Role không hợp lệ."); return false; }
    if (!validPassword(password, error)) return false;
    const QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("INSERT INTO tai_khoan(username,password_hash,ho_ten,role,enabled,created_at,updated_at) VALUES(?,?,?,?,1,?,?)"));
    q.addBindValue(username.trimmed()); q.addBindValue(hashPassword(password)); q.addBindValue(fullName.trimmed());
    q.addBindValue(role); q.addBindValue(now); q.addBindValue(now);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    return true;
}

bool AuthService::updateUser(const UserSession &actor, int userId, const QString &fullName,
                             const QString &role, bool enabled, QString *error)
{
    if (!actor.isAdmin()) { if (error) *error = QStringLiteral("Không đủ quyền ADMIN."); return false; }
    if (actor.id == userId && (!enabled || role != QStringLiteral("ADMIN"))) { if (error) *error = QStringLiteral("Không thể tự khóa hoặc hạ quyền tài khoản đang dùng."); return false; }
    if (role != QStringLiteral("ADMIN") && role != QStringLiteral("USER")) { if (error) *error = QStringLiteral("Role không hợp lệ."); return false; }
    QSqlQuery q(db_.database()); q.prepare(QStringLiteral("UPDATE tai_khoan SET ho_ten=?,role=?,enabled=?,updated_at=? WHERE id=?"));
    q.addBindValue(fullName.trimmed()); q.addBindValue(role); q.addBindValue(enabled ? 1 : 0);
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)); q.addBindValue(userId);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    return q.numRowsAffected() == 1;
}

bool AuthService::changePassword(const UserSession &actor, int userId, const QString &newPassword, QString *error)
{
    if (!actor.isAdmin() && actor.id != userId) { if (error) *error = QStringLiteral("Không đủ quyền."); return false; }
    if (!validPassword(newPassword, error)) return false;
    QSqlQuery q(db_.database()); q.prepare(QStringLiteral("UPDATE tai_khoan SET password_hash=?,updated_at=? WHERE id=?"));
    q.addBindValue(hashPassword(newPassword)); q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)); q.addBindValue(userId);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    return q.numRowsAffected() == 1;
}
