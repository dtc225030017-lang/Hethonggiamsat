#include "alarm_service.h"
#include "database_manager.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

AlarmService::AlarmService(DatabaseManager &db) : db_(db)
{
    QSqlQuery q(db_.database());
    if (q.exec(QStringLiteral("SELECT id,alarm_type,max_value FROM canh_bao WHERE status='ACTIVE' ORDER BY id DESC LIMIT 1")) && q.next()) {
        activeId_ = q.value(0).toInt(); activeType_ = q.value(1).toString(); maxValue_ = q.value(2).toDouble();
    }
}

bool AlarmService::start(const QString &type, double value, double threshold, QString *error)
{
    QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("INSERT INTO canh_bao(start_time,alarm_type,value,threshold,max_value,status) VALUES(?,?,?,?,?,'ACTIVE')"));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)); q.addBindValue(type);
    q.addBindValue(value); q.addBindValue(threshold); q.addBindValue(value);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    activeId_ = q.lastInsertId().toInt(); activeType_ = type; maxValue_ = value; normalSamples_ = 0;
    qWarning() << "Alarm started" << type << value << threshold;
    return true;
}

bool AlarmService::update(double value, bool lowerIsWorse, QString *error)
{
    if ((!lowerIsWorse && value <= maxValue_) || (lowerIsWorse && value >= maxValue_)) return true;
    maxValue_ = value; QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("UPDATE canh_bao SET max_value=? WHERE id=?")); q.addBindValue(value); q.addBindValue(activeId_);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; } return true;
}

bool AlarmService::finish(QString *error)
{
    QSqlQuery q(db_.database()); q.prepare(QStringLiteral("UPDATE canh_bao SET end_time=?,status='ENDED' WHERE id=?"));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)); q.addBindValue(activeId_);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    qInfo() << "Alarm ended" << activeId_; activeId_ = 0; activeType_.clear(); maxValue_ = 0; normalSamples_ = 0; return true;
}

bool AlarmService::process(const SensorReading &r, double threshold, QString *error)
{
    const QString type = r.alarmType == QStringLiteral("NONE")
        ? QStringLiteral("MQ2_HIGH") : r.alarmType;
    const bool lowerIsWorse = type == QStringLiteral("HUMIDITY_LOW");
    const double value = type == QStringLiteral("TEMP_HIGH") ? r.temperature
        : (type.startsWith(QStringLiteral("HUMIDITY")) ? r.humidity : r.mq2Raw);
    const bool high = r.alarm;
    if (!activeId_ && high) return start(type, value, threshold, error);
    if (!activeId_) return true;
    if (high && type != activeType_) {
        if (!finish(error)) return false;
        return start(type, value, threshold, error);
    }
    if (high) { normalSamples_ = 0; return update(value, lowerIsWorse, error); }
    if (++normalSamples_ >= 3) return finish(error);
    return true;
}

bool AlarmService::externalAlarm(const QString &type, double value, double threshold, QString *error)
{
    return activeId_ ? update(value, type == QStringLiteral("HUMIDITY_LOW"), error) : start(type, value, threshold, error);
}

QList<AlarmRecord> AlarmService::query(const QDateTime &from, const QDateTime &to, int limit, QString *error) const
{
    QList<AlarmRecord> out; QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("SELECT id,start_time,end_time,alarm_type,value,threshold,max_value,status FROM canh_bao WHERE start_time>=? AND start_time<=? ORDER BY start_time DESC LIMIT ?"));
    q.addBindValue(from.toUTC().toString(Qt::ISODateWithMs)); q.addBindValue(to.toUTC().toString(Qt::ISODateWithMs)); q.addBindValue(qBound(1, limit, 10000));
    if (!q.exec()) { if (error) *error = q.lastError().text(); return out; }
    while (q.next()) out.append({q.value(0).toInt(), QDateTime::fromString(q.value(1).toString(), Qt::ISODateWithMs).toLocalTime(), QDateTime::fromString(q.value(2).toString(), Qt::ISODateWithMs).toLocalTime(), q.value(3).toString(), q.value(4).toDouble(), q.value(5).toDouble(), q.value(6).toDouble(), q.value(7).toString()});
    return out;
}
bool AlarmService::active() const { return activeId_ != 0; }
