#pragma once
#include "models.h"
#include <QWidget>
class SettingsService; class QSpinBox; class QDoubleSpinBox; class QLineEdit;
class SettingsPage : public QWidget {
    Q_OBJECT
public: SettingsPage(SettingsService &service, const UserSession &session, QWidget *parent = nullptr);
signals: void settingsChanged(const AppSettings &settings); void buzzerCommand(const QString &command);
private slots: void save();
private: SettingsService &service_; UserSession session_; QSpinBox *mq2_; QDoubleSpinBox *tempHigh_; QDoubleSpinBox *humidityHigh_; QDoubleSpinBox *humidityLow_; QSpinBox *storage_; QLineEdit *host_; QSpinBox *port_; QLineEdit *username_; QLineEdit *password_;
};

