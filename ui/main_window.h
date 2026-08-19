#pragma once
#include "mq2_filter.h"
#include "models.h"
#include <QMainWindow>
class DatabaseManager; class AuthService; class SensorRepository; class AlarmService; class SettingsService; class MqttService; class DashboardPage; class HistoryPage; class AlarmHistoryPage; class SettingsPage;
class MainWindow : public QMainWindow {
    Q_OBJECT
public: MainWindow(DatabaseManager &db, AuthService &auth, const UserSession &session, QWidget *parent = nullptr);
    ~MainWindow() override;
private slots: void onSensor(const SensorReading &reading); void onSettings(const AppSettings &settings); void watchdog();
private: DatabaseManager &db_; AuthService &auth_; UserSession session_; SensorRepository *sensorRepo_; AlarmService *alarmService_; SettingsService *settingsService_; MqttService *mqtt_; DashboardPage *dashboard_; HistoryPage *history_; AlarmHistoryPage *alarms_; AppSettings settings_; QDateTime lastReading_; QDateTime lastStored_;
    Mq2Filter mq2Filter_;
};
