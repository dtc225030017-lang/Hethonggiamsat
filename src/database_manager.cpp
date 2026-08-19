#include "database_manager.h"
#include "app_config.h"
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

DatabaseManager::DatabaseManager()
    : connectionName_(QStringLiteral("airdb_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces))) {}

DatabaseManager::~DatabaseManager()
{
    {
        auto db = QSqlDatabase::database(connectionName_, false);
        if (db.isValid()) db.close();
    }
    QSqlDatabase::removeDatabase(connectionName_);
}

bool DatabaseManager::initialize(const QString &rootPath, QString *error)
{
    rootPath_ = rootPath;
    QDir root(rootPath_);
    if (!root.mkpath(QStringLiteral("data")) || !root.mkpath(QStringLiteral("logs"))) {
        if (error) *error = QStringLiteral("Không thể tạo data/logs trong %1").arg(rootPath_);
        return false;
    }
    QFile::setPermissions(root.filePath(QStringLiteral("data")),
                          QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName_);
    const QString fileName = root.filePath(AppConfig::DatabaseFile);
    db.setDatabaseName(fileName);
    if (!db.open()) {
        if (error) *error = db.lastError().text();
        return false;
    }
    QSqlQuery pragma(db);
    pragma.exec(QStringLiteral("PRAGMA journal_mode=WAL"));
    pragma.exec(QStringLiteral("PRAGMA synchronous=NORMAL"));
    pragma.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
    if (!createSchema(error)) return false;
    QFile::setPermissions(fileName, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    return true;
}

bool DatabaseManager::createSchema(QString *error)
{
    const QStringList statements = {
        QStringLiteral("CREATE TABLE IF NOT EXISTS tai_khoan (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL UNIQUE, password_hash TEXT NOT NULL, ho_ten TEXT NOT NULL, role TEXT NOT NULL CHECK(role IN ('ADMIN','USER')), enabled INTEGER NOT NULL DEFAULT 1, created_at TEXT NOT NULL, updated_at TEXT NOT NULL)"),
        QStringLiteral("CREATE TABLE IF NOT EXISTS du_lieu_cam_bien (id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TEXT NOT NULL, temperature REAL NOT NULL, humidity REAL NOT NULL, mq2_raw INTEGER NOT NULL, mq2_mv INTEGER NOT NULL, status TEXT NOT NULL, device_id TEXT NOT NULL)"),
        QStringLiteral("CREATE INDEX IF NOT EXISTS idx_sensor_time ON du_lieu_cam_bien(timestamp)"),
        QStringLiteral("CREATE TABLE IF NOT EXISTS canh_bao (id INTEGER PRIMARY KEY AUTOINCREMENT, start_time TEXT NOT NULL, end_time TEXT, alarm_type TEXT NOT NULL, value REAL NOT NULL, threshold REAL NOT NULL, max_value REAL NOT NULL, status TEXT NOT NULL, acknowledged INTEGER NOT NULL DEFAULT 0, acknowledged_by INTEGER, FOREIGN KEY(acknowledged_by) REFERENCES tai_khoan(id))"),
        QStringLiteral("CREATE INDEX IF NOT EXISTS idx_alarm_time ON canh_bao(start_time)"),
        QStringLiteral("CREATE TABLE IF NOT EXISTS cau_hinh (key TEXT PRIMARY KEY, value TEXT NOT NULL, updated_at TEXT NOT NULL)")
    };
    auto db = database();
    if (!db.transaction()) { if (error) *error = db.lastError().text(); return false; }
    QSqlQuery query(db);
    for (const auto &sql : statements) {
        if (!query.exec(sql)) {
            db.rollback();
            if (error) *error = query.lastError().text();
            return false;
        }
    }
    if (!db.commit()) { if (error) *error = db.lastError().text(); return false; }
    return true;
}

QSqlDatabase DatabaseManager::database() const { return QSqlDatabase::database(connectionName_); }
QString DatabaseManager::rootPath() const { return rootPath_; }

bool DatabaseManager::setConfig(const QString &key, const QString &value, QString *error)
{
    QSqlQuery q(database());
    q.prepare(QStringLiteral("INSERT INTO cau_hinh(key,value,updated_at) VALUES(?,?,?) ON CONFLICT(key) DO UPDATE SET value=excluded.value, updated_at=excluded.updated_at"));
    q.addBindValue(key); q.addBindValue(value); q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    return true;
}

QString DatabaseManager::config(const QString &key, const QString &defaultValue) const
{
    QSqlQuery q(database());
    q.prepare(QStringLiteral("SELECT value FROM cau_hinh WHERE key=?"));
    q.addBindValue(key);
    return q.exec() && q.next() ? q.value(0).toString() : defaultValue;
}

