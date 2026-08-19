#include "sensor_repository.h"
#include "database_manager.h"
#include <QSqlError>
#include <QSqlQuery>

SensorRepository::SensorRepository(DatabaseManager &db) : db_(db) {}

bool SensorRepository::insert(const SensorReading &r, QString *error)
{
    QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("INSERT INTO du_lieu_cam_bien(timestamp,temperature,humidity,mq2_raw,mq2_mv,status,device_id) VALUES(?,?,?,?,?,?,?)"));
    q.addBindValue(r.timestamp.toUTC().toString(Qt::ISODateWithMs)); q.addBindValue(r.temperature);
    q.addBindValue(r.humidity); q.addBindValue(r.mq2Raw); q.addBindValue(r.mq2Mv);
    q.addBindValue(r.status); q.addBindValue(r.deviceId);
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    return true;
}

QList<SensorReading> SensorRepository::query(const QDateTime &from, const QDateTime &to, int limit, QString *error) const
{
    QList<SensorReading> out; QSqlQuery q(db_.database());
    q.prepare(QStringLiteral("SELECT timestamp,temperature,humidity,mq2_raw,mq2_mv,status,device_id FROM du_lieu_cam_bien WHERE timestamp>=? AND timestamp<=? ORDER BY timestamp DESC LIMIT ?"));
    q.addBindValue(from.toUTC().toString(Qt::ISODateWithMs)); q.addBindValue(to.toUTC().toString(Qt::ISODateWithMs));
    q.addBindValue(qBound(1, limit, 10000));
    if (!q.exec()) { if (error) *error = q.lastError().text(); return out; }
    while (q.next()) {
        SensorReading r; r.timestamp = QDateTime::fromString(q.value(0).toString(), Qt::ISODateWithMs).toLocalTime();
        r.temperature = q.value(1).toDouble(); r.humidity = q.value(2).toDouble();
        r.mq2Raw = q.value(3).toInt(); r.mq2Mv = q.value(4).toInt();
        r.status = q.value(5).toString(); r.deviceId = q.value(6).toString(); out.append(r);
    }
    return out;
}

