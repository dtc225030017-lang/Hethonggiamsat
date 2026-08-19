#include "main_window.h"
#include "alarm_history_page.h"
#include "alarm_service.h"
#include "app_config.h"
#include "auth_service.h"
#include "dashboard_page.h"
#include "database_manager.h"
#include "history_page.h"
#include "mqtt_service.h"
#include "sensor_repository.h"
#include "settings_page.h"
#include "settings_service.h"
#include "users_page.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <cmath>

MainWindow::MainWindow(DatabaseManager &db, AuthService &auth,
                       const UserSession &session, QWidget *parent)
    : QMainWindow(parent), db_(db), auth_(auth), session_(session)
{
    sensorRepo_ = new SensorRepository(db_);
    alarmService_ = new AlarmService(db_);
    settingsService_ = new SettingsService(db_);
    mqtt_ = new MqttService(this);
    settings_ = settingsService_->load();

    dashboard_ = new DashboardPage;
    history_ = new HistoryPage(*sensorRepo_, session_.isAdmin());
    alarms_ = new AlarmHistoryPage(*alarmService_);

    auto *tabs = new QTabWidget;
    tabs->setDocumentMode(true);
    tabs->addTab(dashboard_, QStringLiteral("Tổng quan"));
    tabs->addTab(history_, QStringLiteral("Dữ liệu"));
    tabs->addTab(alarms_, QStringLiteral("Cảnh báo"));

    if (session_.isAdmin()) {
        auto *settingsPage = new SettingsPage(*settingsService_, session_);
        auto *usersPage = new UsersPage(auth_, session_);
        tabs->addTab(settingsPage, QStringLiteral("Cài đặt"));
        tabs->addTab(usersPage, QStringLiteral("Tài khoản"));
        connect(settingsPage, &SettingsPage::settingsChanged,
                this, &MainWindow::onSettings);
        connect(settingsPage, &SettingsPage::buzzerCommand, this,
                [this](const QString &value) {
                    if (session_.isAdmin())
                        mqtt_->publish(QStringLiteral("air/command/buzzer"),
                                       value.toUtf8());
                });
    }

    setCentralWidget(tabs);
    setWindowTitle(QStringLiteral("Hệ thống giám sát chất lượng không khí — %1 (%2)")
                       .arg(session_.fullName, session_.role));
    resize(1240, 820);
    setMinimumSize(980, 680);
    statusBar()->showMessage(QStringLiteral("Đã đăng nhập: %1  •  %2")
                                 .arg(session_.username, session_.role));

    connect(mqtt_, &MqttService::connectedChanged,
            dashboard_, &DashboardPage::setMqttConnected);
    connect(mqtt_, &MqttService::sensorReceived,
            this, &MainWindow::onSensor);
    connect(mqtt_, &MqttService::deviceStatusReceived,
            dashboard_, &DashboardPage::setDeviceStatus);
    connect(mqtt_, &MqttService::alarmReceived, this,
            [this](const QString &type, double value, double threshold) {
                // MQ-2 alarms are created from the filtered telemetry stream.
                // Accepting this topic would bypass the confirmation window.
                if (type.startsWith(QStringLiteral("MQ2"), Qt::CaseInsensitive))
                    return;
                QString error;
                if (!alarmService_->externalAlarm(type, value, threshold, &error))
                    qWarning() << error;
                alarms_->refresh();
            });
    connect(mqtt_, &MqttService::serviceError, this,
            [this](const QString &error) {
                statusBar()->showMessage(error, 5000);
                qWarning() << error;
            });

    QString error;
    if (!mqtt_->start(settings_, settingsService_->mqttPassword(), &error))
        QMessageBox::warning(this, QStringLiteral("MQTT"), error);
    else
        mqtt_->publish(QStringLiteral("air/command/status"), QByteArray("GET"));

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::watchdog);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    mqtt_->stop();
    delete sensorRepo_;
    delete alarmService_;
    delete settingsService_;
}

void MainWindow::onSensor(const SensorReading &reading)
{
    lastReading_ = QDateTime::currentDateTime();
    SensorReading stable = reading;
    int alarmThreshold = settings_.mq2Threshold;
    bool pending = false;

    if (!reading.mq2DigitalMode) {
        alarmThreshold = reading.mq2Threshold > 0
            ? reading.mq2Threshold : settings_.mq2Threshold;
        mq2Filter_.reset();
        stable.alarm = reading.mq2Ready && reading.alarm;
        pending = false;
    } else {
        // The system now uses the tested AO averaging firmware.
        alarmThreshold = settings_.mq2Threshold;
        mq2Filter_.reset();
        stable.mq2Raw = 0;
        stable.mq2Mv = 0;
        stable.alarm = false;
    }
    stable.alarmType = stable.alarm ? QStringLiteral("MQ2_HIGH")
                                    : QStringLiteral("NONE");
    stable.status = stable.alarm ? QStringLiteral("ALARM")
                                 : QStringLiteral("NORMAL");

    dashboard_->updateReading(stable);
    dashboard_->setMq2FirmwareCompatible(!reading.mq2DigitalMode);
    dashboard_->setMq2Ready(reading.mq2Ready);
    if (reading.mq2Ready)
        dashboard_->setMq2Pending(pending);
    dashboard_->setDeviceStatus(QStringLiteral("ONLINE"));

    QString error;
    const bool wasActive = alarmService_->active();
    if (!alarmService_->process(stable, alarmThreshold, &error))
        qWarning() << error;

    const bool due = !lastStored_.isValid()
        || lastStored_.secsTo(lastReading_) >= settings_.storageIntervalSeconds;
    if (due || stable.alarm) {
        if (sensorRepo_->insert(stable, &error))
            lastStored_ = lastReading_;
        else
            qWarning() << "DB sensor" << error;
    }
    if (wasActive != alarmService_->active() || stable.alarm)
        alarms_->refresh();
}

void MainWindow::onSettings(const AppSettings &settings)
{
    settings_ = settings;
    mq2Filter_.reset();
    mqtt_->stop();
    QString error;
    mqtt_->start(settings_, settingsService_->mqttPassword(), &error);
    mqtt_->publishSettings(settings_);
}

void MainWindow::watchdog()
{
    dashboard_->setDataStale(lastReading_.isValid()
        && lastReading_.secsTo(QDateTime::currentDateTime()) > AppConfig::StaleSeconds);
}
