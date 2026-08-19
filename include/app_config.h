#pragma once

#include <QString>

namespace AppConfig {
inline const QString Name = QStringLiteral("Hethonggiamsat");
inline const QString DefaultRoot = QStringLiteral("/home/pi/Duy/Hethonggiamsat");
inline const QString DatabaseFile = QStringLiteral("data/hethonggiamsat.sqlite");
inline const QString MqttSecretFile = QStringLiteral("data/mqtt_credentials.ini");
inline constexpr int StaleSeconds = 8;
inline constexpr int MaxChartPoints = 300;
}

