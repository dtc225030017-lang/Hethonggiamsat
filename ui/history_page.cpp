#include "history_page.h"
#include "csv_exporter.h"
#include "sensor_repository.h"
#include <QDateTimeEdit>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

HistoryPage::HistoryPage(SensorRepository &repo,bool admin,QWidget *parent):QWidget(parent),repo_(repo),admin_(admin){ from_=new QDateTimeEdit(QDateTime::currentDateTime().addDays(-1));to_=new QDateTimeEdit(QDateTime::currentDateTime());from_->setCalendarPopup(true);to_->setCalendarPopup(true);auto *refreshButton=new QPushButton(QStringLiteral("Tìm kiếm / Làm mới"));export_=new QPushButton(QStringLiteral("Xuất CSV"));export_->setVisible(admin_);table_=new QTableWidget;table_->setColumnCount(7);table_->setHorizontalHeaderLabels({QStringLiteral("Thời gian"),QStringLiteral("Nhiệt độ"),QStringLiteral("Độ ẩm"),QStringLiteral("MQ-2"),QStringLiteral("mV"),QStringLiteral("Trạng thái"),QStringLiteral("Thiết bị")});table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);table_->setEditTriggers(QAbstractItemView::NoEditTriggers);table_->setAlternatingRowColors(true);table_->verticalHeader()->setVisible(false);table_->setSelectionBehavior(QAbstractItemView::SelectRows);auto *bar=new QHBoxLayout;bar->addWidget(from_);bar->addWidget(to_);bar->addWidget(refreshButton);bar->addWidget(export_);auto *layout=new QVBoxLayout(this);layout->setContentsMargins(20,20,20,20);layout->setSpacing(12);layout->addLayout(bar);layout->addWidget(table_);connect(refreshButton,&QPushButton::clicked,this,&HistoryPage::refresh);connect(export_,&QPushButton::clicked,this,&HistoryPage::exportCsv);refresh(); }
void HistoryPage::refresh(){QString e;auto rows=repo_.query(from_->dateTime(),to_->dateTime(),1000,&e);table_->setRowCount(rows.size());for(int i=0;i<rows.size();++i){const auto&r=rows[i];QStringList v={r.timestamp.toString("dd/MM/yyyy HH:mm:ss"),QString::number(r.temperature,'f',1),QString::number(r.humidity,'f',1),QString::number(r.mq2Raw),QString::number(r.mq2Mv),r.status,r.deviceId};for(int c=0;c<v.size();++c)table_->setItem(i,c,new QTableWidgetItem(v[c]));}if(!e.isEmpty())QMessageBox::warning(this,QStringLiteral("Database"),e);}
void HistoryPage::exportCsv(){if(!admin_){QMessageBox::critical(this,QStringLiteral("Phân quyền"),QStringLiteral("Chỉ ADMIN được xuất CSV."));return;}QString e;auto rows=repo_.query(from_->dateTime(),to_->dateTime(),10000,&e);const QString name=QFileDialog::getSaveFileName(this,QStringLiteral("Xuất CSV"),QStringLiteral("du_lieu_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),QStringLiteral("CSV (*.csv)"));if(name.isEmpty())return;if(!CsvExporter::exportSensors(name,rows,&e))QMessageBox::warning(this,QStringLiteral("Xuất CSV"),e);else QMessageBox::information(this,QStringLiteral("Xuất CSV"),QStringLiteral("Đã xuất %1 bản ghi.").arg(rows.size()));}
