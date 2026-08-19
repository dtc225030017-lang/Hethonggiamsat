#include "alarm_service.h"
#include "database_manager.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

AlarmService::AlarmService(DatabaseManager &db) : db_(db)
{
    QSqlQuery q(db_.database());
    if (q.exec(QStringLiteral("SELECT id,max_value FROM canh_bao WHERE status='ACTIVE' ORDER BY id DESC LIMIT 1")) && q.next()) {
        activeId_ = q.value(0).toInt(); maxValue_ = q.value(1).toDouble();
    }
}

bool AlarmService::start(const QString &type, double value, double threshold, QString *error)
{
    QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("INSERT INTO canh_bao(start_time,alarm_type,value,threshold,max_value,status) VALUES(?,?,?,?,?,'ACTIVE')"));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)); q.addBindValue(type);
    q.addBindValue(value); q.addBindValue(threshold); q.addBindValue(value);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    activeId_ = q.lastInsertId().toInt(); maxValue_ = value; normalSamples_ = 0;
    qWarning() << "Alarm started" << type << value << threshold;
    return true;
}

bool AlarmService::update(double value, QString *error)
{
    if (value <= maxValue_) return true;
    maxValue_ = value; QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("UPDATE canh_bao SET max_value=? WHERE id=?")); q.addBindValue(value); q.addBindValue(activeId_);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; } return true;
}

bool AlarmService::finish(QString *error)
{
    QSqlQuery q(db_.database()); q.prepare(QStringLiteral("UPDATE canh_bao SET end_time=?,status='ENDED' WHERE id=?"));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)); q.addBindValue(activeId_);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    qInfo() << "Alarm ended" << activeId_; activeId_ = 0; maxValue_ = 0; normalSamples_ = 0; return true;
}

bool AlarmService::process(const SensorReading &r, double threshold, QString *error)
{
    // MainWindow only sets this flag after filtering spikes and confirming a
    // sustained high MQ-2 level.
    const bool high = r.alarm;
    if (!activeId_ && high) return start(r.alarmType == QStringLiteral("NONE") ? QStringLiteral("MQ2_HIGH") : r.alarmType, r.mq2Raw, threshold, error);
    if (!activeId_) return true;
    if (high) { normalSamples_ = 0; return update(r.mq2Raw, error); }
    if (threshold <= 1.0) return finish(error);
    const double hysteresis = qBound(5.0, threshold * 0.10, 100.0);
    if (r.mq2Raw <= threshold - hysteresis && ++normalSamples_ >= 3) return finish(error);
    return true;
}

bool AlarmService::externalAlarm(const QString &type, double value, double threshold, QString *error)
{
    return activeId_ ? update(value, error) : start(type, value, threshold, error);
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
