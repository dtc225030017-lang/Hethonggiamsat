#pragma once
#include "models.h"
#include <QWidget>
class AuthService; class QTableWidget;
class UsersPage : public QWidget {
    Q_OBJECT
public: UsersPage(AuthService &auth, const UserSession &session, QWidget *parent = nullptr);
private slots: void refresh(); void add(); void edit(); void changePassword();
private: int selectedId() const; AuthService &auth_; UserSession session_; QTableWidget *table_;
};

