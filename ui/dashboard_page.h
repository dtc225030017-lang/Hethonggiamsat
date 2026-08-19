#pragma once
#include "models.h"
#include <QWidget>
class QLabel; class TimeSeriesWidget;
class DashboardPage : public QWidget {
    Q_OBJECT
public: explicit DashboardPage(QWidget *parent = nullptr);
    void updateReading(const SensorReading &reading); void setMqttConnected(bool value);
    void setDeviceStatus(const QString &status); void setDataStale(bool stale);
    void setMq2Pending(bool pending);
    void setMq2Ready(bool ready);
    void setMq2FirmwareCompatible(bool compatible);
private: QLabel *temperature_; QLabel *humidity_; QLabel *mq2_; QLabel *quality_;
    QLabel *esp32_; QLabel *mqtt_; QLabel *lastSeen_; TimeSeriesWidget *tempChart_; TimeSeriesWidget *humidityChart_; TimeSeriesWidget *mq2Chart_;
};
