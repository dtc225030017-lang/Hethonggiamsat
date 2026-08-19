#pragma once

#include <QString>

class AppLogger {
public:
    static bool install(const QString &rootPath);
    static void shutdown();
};

