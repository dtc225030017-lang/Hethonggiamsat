#pragma once
#include <QWidget>
class AlarmService; class QDateTimeEdit; class QTableWidget;
class AlarmHistoryPage : public QWidget {
    Q_OBJECT
public: explicit AlarmHistoryPage(AlarmService &service, QWidget *parent = nullptr);
public slots: void refresh();
private: AlarmService &service_; QDateTimeEdit *from_; QDateTimeEdit *to_; QTableWidget *table_;
};

