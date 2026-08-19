#include "settings_page.h"
#include "settings_service.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsPage::SettingsPage(SettingsService &service,const UserSession &session,QWidget *parent):QWidget(parent),service_(service),session_(session)
{
    const auto s=service_.load(); mq2_=new QSpinBox;mq2_->setRange(0,4095);mq2_->setValue(s.mq2Threshold);tempHigh_=new QDoubleSpinBox;tempHigh_->setRange(-20,100);tempHigh_->setValue(s.temperatureHigh);humidityHigh_=new QDoubleSpinBox;humidityHigh_->setRange(0,100);humidityHigh_->setValue(s.humidityHigh);humidityLow_=new QDoubleSpinBox;humidityLow_->setRange(0,100);humidityLow_->setValue(s.humidityLow);storage_=new QSpinBox;storage_->setRange(1,3600);storage_->setValue(s.storageIntervalSeconds);
    host_=new QLineEdit(s.mqttHost);port_=new QSpinBox;port_->setRange(1,65535);port_->setValue(s.mqttPort);username_=new QLineEdit(s.mqttUsername);password_=new QLineEdit;password_->setEchoMode(QLineEdit::Password);password_->setPlaceholderText(QStringLiteral("Để trống nếu không đổi"));
    auto*thresholdForm=new QFormLayout;auto*mq2Info=new QLabel(QStringLiteral("MQ-2 dùng chân AO, lấy trung bình 30 mẫu để giảm nhiễu. Ngưỡng đã kiểm thử: 2500 ADC."));mq2Info->setWordWrap(true);mq2Info->setStyleSheet(QStringLiteral("color:#64748b;padding:6px"));thresholdForm->addRow(mq2Info);thresholdForm->addRow(QStringLiteral("Ngưỡng MQ-2 (ADC)"),mq2_);thresholdForm->addRow(QStringLiteral("Nhiệt độ cao (°C)"),tempHigh_);thresholdForm->addRow(QStringLiteral("Độ ẩm cao (%)"),humidityHigh_);thresholdForm->addRow(QStringLiteral("Độ ẩm thấp (%)"),humidityLow_);thresholdForm->addRow(QStringLiteral("Chu kỳ lưu DB (giây)"),storage_);auto*thresholdBox=new QGroupBox(QStringLiteral("Ngưỡng và lưu trữ"));thresholdBox->setLayout(thresholdForm);
    auto*mqttForm=new QFormLayout;mqttForm->addRow(QStringLiteral("Host"),host_);mqttForm->addRow(QStringLiteral("Port"),port_);mqttForm->addRow(QStringLiteral("Username"),username_);mqttForm->addRow(QStringLiteral("Password"),password_);auto*mqttBox=new QGroupBox(QStringLiteral("MQTT"));mqttBox->setLayout(mqttForm);
    auto*saveButton=new QPushButton(QStringLiteral("Lưu và gửi xuống ESP32"));auto*on=new QPushButton(QStringLiteral("Buzzer ON"));auto*off=new QPushButton(QStringLiteral("Buzzer OFF"));auto*automatic=new QPushButton(QStringLiteral("Buzzer AUTO"));auto*buttons=new QHBoxLayout;buttons->addWidget(saveButton);buttons->addStretch();buttons->addWidget(on);buttons->addWidget(off);buttons->addWidget(automatic);auto*l=new QVBoxLayout(this);l->addWidget(thresholdBox);l->addWidget(mqttBox);l->addLayout(buttons);l->addStretch();
    connect(saveButton,&QPushButton::clicked,this,&SettingsPage::save);connect(on,&QPushButton::clicked,this,[this]{if(session_.isAdmin())emit buzzerCommand(QStringLiteral("ON"));});connect(off,&QPushButton::clicked,this,[this]{if(session_.isAdmin())emit buzzerCommand(QStringLiteral("OFF"));});connect(automatic,&QPushButton::clicked,this,[this]{if(session_.isAdmin())emit buzzerCommand(QStringLiteral("AUTO"));});
    if(!session_.isAdmin()){thresholdBox->setEnabled(false);mqttBox->setEnabled(false);saveButton->setEnabled(false);on->setEnabled(false);off->setEnabled(false);automatic->setEnabled(false);}
}
void SettingsPage::save(){if(!session_.isAdmin()){QMessageBox::critical(this,QStringLiteral("Phân quyền"),QStringLiteral("Không đủ quyền ADMIN."));return;}AppSettings s;s.mq2Threshold=mq2_->value();s.temperatureHigh=tempHigh_->value();s.humidityHigh=humidityHigh_->value();s.humidityLow=humidityLow_->value();s.storageIntervalSeconds=storage_->value();s.mqttHost=host_->text().trimmed();s.mqttPort=port_->value();s.mqttUsername=username_->text().trimmed();QString e;if(!service_.save(session_,s,password_->text(),&e)){QMessageBox::warning(this,QStringLiteral("Cấu hình"),e);return;}password_->clear();emit settingsChanged(s);QMessageBox::information(this,QStringLiteral("Cấu hình"),QStringLiteral("Đã lưu và publish ngưỡng."));}
