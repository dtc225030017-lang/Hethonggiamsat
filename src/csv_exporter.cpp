#include "csv_exporter.h"
#include <QFile>
#include <QTextStream>

namespace { QString csv(QString s) { s.replace('"', QStringLiteral("\"\"")); return QStringLiteral("\"") + s + QStringLiteral("\""); } }

bool CsvExporter::exportSensors(const QString &fileName, const QList<SensorReading> &rows, QString *error)
{
    if (rows.size() > 10000) { if (error) *error = QStringLiteral("Giới hạn xuất là 10.000 bản ghi. Hãy thu hẹp khoảng thời gian."); return false; }
    QFile file(fileName); if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) { if (error) *error = file.errorString(); return false; }
    QTextStream out(&file); out.setEncoding(QStringConverter::Utf8);
    out << "timestamp,temperature_c,humidity_percent,mq2_raw,mq2_mv,status,device_id\n";
    for (const auto &r : rows) out << csv(r.timestamp.toString(Qt::ISODate)) << ',' << r.temperature << ',' << r.humidity << ',' << r.mq2Raw << ',' << r.mq2Mv << ',' << csv(r.status) << ',' << csv(r.deviceId) << '\n';
    return true;
}

