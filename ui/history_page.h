#pragma once
#include "models.h"
#include <QWidget>
class SensorRepository; class QDateTimeEdit; class QTableWidget; class QPushButton;
class HistoryPage : public QWidget {
    Q_OBJECT
public: HistoryPage(SensorRepository &repo, bool admin, QWidget *parent = nullptr);
public slots: void refresh();
private slots: void exportCsv();
private: SensorRepository &repo_; bool admin_; QDateTimeEdit *from_; QDateTimeEdit *to_; QTableWidget *table_; QPushButton *export_;
};

