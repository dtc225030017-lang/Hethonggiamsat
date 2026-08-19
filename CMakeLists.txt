cmake_minimum_required(VERSION 3.18)
project(Hethonggiamsat VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Sql Network DBus)
find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h REQUIRED)
find_library(MOSQUITTO_LIBRARY NAMES mosquitto REQUIRED)

qt_add_executable(Hethonggiamsat
    include/mqtt_service.h
    ui/login_window.h
    ui/main_window.h
    ui/dashboard_page.h
    ui/history_page.h
    ui/alarm_history_page.h
    ui/users_page.h
    ui/settings_page.h
    src/main.cpp
    src/app_logger.cpp
    src/database_manager.cpp
    src/auth_service.cpp
    src/sensor_repository.cpp
    src/alarm_service.cpp
    src/settings_service.cpp
    src/csv_exporter.cpp
    src/mq2_filter.cpp
    src/mqtt_service.cpp
    ui/login_window.cpp
    ui/main_window.cpp
    ui/dashboard_page.cpp
    ui/history_page.cpp
    ui/alarm_history_page.cpp
    ui/users_page.cpp
    ui/settings_page.cpp
)

target_include_directories(Hethonggiamsat PRIVATE include ui ${MOSQUITTO_INCLUDE_DIR})
target_link_libraries(Hethonggiamsat PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Network Qt6::DBus
    ${MOSQUITTO_LIBRARY}
)
target_compile_options(Hethonggiamsat PRIVATE -Wall -Wextra -Wpedantic)
install(TARGETS Hethonggiamsat RUNTIME DESTINATION .)
