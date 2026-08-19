#pragma once

#include "models.h"
#include <QString>

class CsvExporter {
public:
    static bool exportSensors(const QString &fileName, const QList<SensorReading> &rows,
                              QString *error = nullptr);
};

