#include "app_config.h"
#include "app_logger.h"
#include "alarm_service.h"
#include "auth_service.h"
#include "database_manager.h"
#include "login_window.h"
#include "main_window.h"
#include "mq2_filter.h"
#include "mqtt_service.h"
#include "sensor_repository.h"
#include "settings_service.h"
#include <QApplication>
#include <QDir>
#include <QEventLoop>
#include <QFont>
#include <QLockFile>
#include <QTemporaryDir>
#include <QTimer>

static QString applicationStyle()
{
    return QStringLiteral(R"QSS(
        QWidget {
            color: #1e293b;
            font-family: "DejaVu Sans";
            font-size: 10pt;
        }
        QMainWindow, QDialog, QTabWidget::pane {
            background: #f1f5f9;
        }
        QTabWidget::pane {
            border: 0;
            top: -1px;
        }
        QTabBar {
            background: #ffffff;
        }
        QTabBar::tab {
            background: #ffffff;
            color: #64748b;
            border: 0;
            border-bottom: 3px solid transparent;
            min-width: 108px;
            padding: 13px 18px 11px 18px;
            font-weight: 600;
        }
        QTabBar::tab:hover {
            color: #2563eb;
            background: #f8fafc;
        }
        QTabBar::tab:selected {
            color: #1d4ed8;
            border-bottom-color: #2563eb;
        }
        QLabel#pageHeading {
            color: #0f172a;
            font-size: 21pt;
            font-weight: 700;
        }
        QLabel#pageSubheading, QLabel#lastSeen, QLabel#metricCaption {
            color: #64748b;
        }
        QFrame#metricCard, QGroupBox {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 10px;
        }
        QLabel#metricTitle {
            color: #64748b;
            font-size: 8pt;
            font-weight: 700;
        }
        QLabel#metricValue {
            color: #0f172a;
            font-size: 18pt;
            font-weight: 700;
        }
        QLabel#metricCaption {
            font-size: 8pt;
        }
        QLabel#qualityNormal, QLabel#qualityAlarm, QLabel#qualityPending,
        QLabel#qualityWaiting {
            border-radius: 9px;
            font-weight: 700;
            padding: 8px 16px;
        }
        QLabel#qualityNormal { background: #dcfce7; color: #166534; border: 1px solid #bbf7d0; }
        QLabel#qualityAlarm { background: #fee2e2; color: #b91c1c; border: 1px solid #fecaca; }
        QLabel#qualityPending { background: #fef3c7; color: #92400e; border: 1px solid #fde68a; }
        QLabel#qualityWaiting { background: #e2e8f0; color: #475569; border: 1px solid #cbd5e1; }
        QLabel#statusOnline { color: #15803d; font-weight: 700; }
        QLabel#statusOffline { color: #dc2626; font-weight: 700; }
        QLabel#statusWarning { color: #d97706; font-weight: 700; }
        QPushButton {
            background: #2563eb;
            color: #ffffff;
            border: 0;
            border-radius: 7px;
            min-height: 34px;
            padding: 0 16px;
            font-weight: 600;
        }
        QPushButton:hover { background: #1d4ed8; }
        QPushButton:pressed { background: #1e40af; }
        QPushButton:disabled { background: #cbd5e1; color: #94a3b8; }
        QLineEdit, QSpinBox, QDoubleSpinBox, QDateTimeEdit, QComboBox {
            background: #ffffff;
            color: #0f172a;
            border: 1px solid #cbd5e1;
            border-radius: 7px;
            min-height: 34px;
            padding: 0 9px;
            selection-background-color: #bfdbfe;
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus,
        QDateTimeEdit:focus, QComboBox:focus {
            border: 2px solid #3b82f6;
        }
        QGroupBox {
            margin-top: 14px;
            padding: 18px 14px 14px 14px;
            font-weight: 700;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 7px;
            color: #334155;
            background: #f1f5f9;
        }
        QTableWidget {
            background: #ffffff;
            alternate-background-color: #f8fafc;
            border: 1px solid #e2e8f0;
            border-radius: 9px;
            gridline-color: #eef2f7;
            selection-background-color: #dbeafe;
            selection-color: #1e3a8a;
        }
        QHeaderView::section {
            background: #f8fafc;
            color: #475569;
            border: 0;
            border-bottom: 1px solid #e2e8f0;
            padding: 10px 7px;
            font-weight: 700;
        }
        QStatusBar {
            background: #ffffff;
            color: #64748b;
            border-top: 1px solid #e2e8f0;
        }
        QScrollBar:vertical { width: 10px; background: transparent; margin: 2px; }
        QScrollBar::handle:vertical { background: #cbd5e1; border-radius: 4px; min-height: 28px; }
        QToolTip { background: #0f172a; color: white; border: 0; padding: 5px; }
    )QSS");
}

static int selfTest()
{
    QTemporaryDir temp; if(!temp.isValid())return 10;qputenv("HETHONGGIAMSAT_BOOTSTRAP_PASSWORD","Testpass123");DatabaseManager db;QString e;if(!db.initialize(temp.path(),&e))return 11;AuthService auth(db);if(!auth.ensureBootstrapAdmin(&e))return 12;UserSession admin;if(!auth.authenticate(QStringLiteral("admin"),QStringLiteral("Testpass123"),&admin,&e)||!admin.isAdmin())return 13;UserSession userActor;userActor.id=99;userActor.role=QStringLiteral("USER");if(auth.addUser(userActor,QStringLiteral("bad"),QStringLiteral("Password1"),QStringLiteral("Bad"),QStringLiteral("USER"),&e))return 14;if(!auth.addUser(admin,QStringLiteral("operator"),QStringLiteral("Operator123"),QStringLiteral("Vận hành"),QStringLiteral("USER"),&e))return 15;SettingsService settings(db);auto s=settings.load();if(s.mq2Threshold!=2500)return 16;s.mq2Threshold=2600;if(!settings.save(admin,s,{},&e)||settings.load().mq2Threshold!=2600)return 17;SensorRepository repo(db);SensorReading r;r.deviceId=QStringLiteral("TEST");r.mq2Raw=2700;r.alarm=true;r.alarmType=QStringLiteral("MQ2_HIGH");if(!repo.insert(r,&e)||repo.query(QDateTime::currentDateTime().addDays(-1),QDateTime::currentDateTime().addDays(1)).size()!=1)return 18;AlarmService alarms(db);if(!alarms.process(r,2600,&e)||!alarms.active())return 19;Mq2Filter filter;for(int i=0;i<4;++i)if(filter.update(i==2?3000:900,2500).alarm)return 23;bool confirmed=false;for(int i=0;i<30;++i)confirmed=filter.update(3000,2500).alarm;if(!confirmed)return 24;bool cleared=false;for(int i=0;i<30;++i)cleared=!filter.update(900,2500).alarm;if(!cleared)return 25;return 0;
}

static int mqttTest(const QString &root)
{
    DatabaseManager db; QString error;
    if (!db.initialize(root, &error)) return 20;
    SettingsService settingsService(db);
    MqttService mqtt;
    QEventLoop loop;
    int result = 21;
    QObject::connect(&mqtt, &MqttService::connectedChanged, &loop, [&](bool connected) {
        if (connected) {
            mqtt.publish(QStringLiteral("air/command/status"), QByteArrayLiteral("GET"));
            result = 0;
            loop.quit();
        }
    });
    QObject::connect(&mqtt, &MqttService::serviceError, &loop, [&](const QString &message) {
        qWarning() << message;
    });
    const auto settings = settingsService.load();
    if (!mqtt.start(settings, settingsService.mqttPassword(), &error)) return 22;
    QTimer::singleShot(8000, &loop, &QEventLoop::quit);
    loop.exec();
    mqtt.stop();
    return result;
}

static int integrationTest(QApplication &app, const QString &root)
{
    DatabaseManager db; QString error;
    if (!db.initialize(root, &error)) return 30;
    AuthService auth(db);
    if (!auth.ensureBootstrapAdmin()) return 31;
    UserSession testSession;
    testSession.id = -1;
    testSession.username = QStringLiteral("integration_test");
    testSession.fullName = QStringLiteral("Integration Test");
    testSession.role = QStringLiteral("ADMIN");
    testSession.enabled = true;
    MainWindow window(db, auth, testSession);
    window.show();
    QTimer::singleShot(10000, &app, &QCoreApplication::quit);
    return app.exec();
}

int main(int argc,char*argv[])
{
    QApplication app(argc,argv);app.setApplicationName(AppConfig::Name);app.setOrganizationName(QStringLiteral("DuyIoT"));app.setFont(QFont(QStringLiteral("DejaVu Sans"),10));app.setStyleSheet(applicationStyle());if(app.arguments().contains(QStringLiteral("--self-test")))return selfTest();QString root=qEnvironmentVariable("HETHONGGIAMSAT_ROOT",AppConfig::DefaultRoot);if(app.arguments().contains(QStringLiteral("--mqtt-test")))return mqttTest(root);if(app.arguments().contains(QStringLiteral("--integration-test")))return integrationTest(app,root);QDir().mkpath(root);QLockFile lock(QDir(root).filePath(QStringLiteral("data/Hethonggiamsat.lock")));lock.setStaleLockTime(0);if(!lock.tryLock(100)){qCritical("Ứng dụng đang chạy ở một instance khác.");return 2;}AppLogger::install(root);qInfo()<<"Application startup";DatabaseManager db;QString error;if(!db.initialize(root,&error)){qCritical()<<"Database init failed"<<error;return 3;}AuthService auth(db);QString bootstrap;if(!auth.ensureBootstrapAdmin(&bootstrap)){qCritical()<<"Bootstrap admin failed";return 4;}if(!bootstrap.isEmpty())qInfo()<<bootstrap;LoginWindow login(auth);if(login.exec()!=QDialog::Accepted){qInfo()<<"Application shutdown before login";AppLogger::shutdown();return 0;}MainWindow window(db,auth,login.session());window.show();int rc=app.exec();qInfo()<<"Application shutdown";AppLogger::shutdown();return rc;
}
