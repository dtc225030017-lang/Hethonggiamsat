#include "alarm_history_page.h"
#include "alarm_service.h"
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
AlarmHistoryPage::AlarmHistoryPage(AlarmService&s,QWidget*p):QWidget(p),service_(s){from_=new QDateTimeEdit(QDateTime::currentDateTime().addDays(-7));to_=new QDateTimeEdit(QDateTime::currentDateTime());from_->setCalendarPopup(true);to_->setCalendarPopup(true);auto*b=new QPushButton(QStringLiteral("Tìm kiếm / Làm mới"));table_=new QTableWidget;table_->setColumnCount(7);table_->setHorizontalHeaderLabels({QStringLiteral("Bắt đầu"),QStringLiteral("Kết thúc"),QStringLiteral("Loại"),QStringLiteral("Giá trị"),QStringLiteral("Ngưỡng"),QStringLiteral("Lớn nhất"),QStringLiteral("Trạng thái")});table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);table_->setEditTriggers(QAbstractItemView::NoEditTriggers);table_->setAlternatingRowColors(true);table_->verticalHeader()->setVisible(false);table_->setSelectionBehavior(QAbstractItemView::SelectRows);auto*bar=new QHBoxLayout;bar->addWidget(from_);bar->addWidget(to_);bar->addWidget(b);auto*l=new QVBoxLayout(this);l->setContentsMargins(20,20,20,20);l->setSpacing(12);l->addLayout(bar);l->addWidget(table_);connect(b,&QPushButton::clicked,this,&AlarmHistoryPage::refresh);refresh();}
void AlarmHistoryPage::refresh(){QString e;auto rows=service_.query(from_->dateTime(),to_->dateTime(),1000,&e);table_->setRowCount(rows.size());for(int i=0;i<rows.size();++i){const auto&r=rows[i];QStringList v={r.startTime.toString("dd/MM/yyyy HH:mm:ss"),r.endTime.isValid()?r.endTime.toString("dd/MM/yyyy HH:mm:ss"):QStringLiteral("--"),r.type,QString::number(r.value),QString::number(r.threshold),QString::number(r.maxValue),r.status};for(int c=0;c<v.size();++c)table_->setItem(i,c,new QTableWidgetItem(v[c]));}if(!e.isEmpty())QMessageBox::warning(this,QStringLiteral("Database"),e);}
