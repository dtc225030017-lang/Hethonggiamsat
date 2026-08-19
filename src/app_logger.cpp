#include "app_logger.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QTextStream>

namespace {
QFile logFile;
QMutex logMutex;
QtMessageHandler oldHandler = nullptr;

void handler(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    QMutexLocker locker(&logMutex);
    if (!logFile.isOpen()) return;
    const char *level = type == QtDebugMsg ? "DEBUG" : type == QtInfoMsg ? "INFO" :
                        type == QtWarningMsg ? "WARN" : type == QtCriticalMsg ? "ERROR" : "FATAL";
    QTextStream stream(&logFile);
    stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs)
           << " [" << level << "] " << message << '\n';
    stream.flush();
}
}

bool AppLogger::install(const QString &rootPath)
{
    QDir root(rootPath);
    if (!root.mkpath(QStringLiteral("logs"))) return false;
    logFile.setFileName(root.filePath(QStringLiteral("logs/hethonggiamsat.log")));
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) return false;
    logFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    oldHandler = qInstallMessageHandler(handler);
    return true;
}

void AppLogger::shutdown()
{
    qInstallMessageHandler(oldHandler);
    if (logFile.isOpen()) logFile.close();
}

