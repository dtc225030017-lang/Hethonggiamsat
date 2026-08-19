#pragma once
#include "models.h"
#include <QDialog>
class AuthService; class QLineEdit; class QLabel;
class LoginWindow : public QDialog {
    Q_OBJECT
public: explicit LoginWindow(AuthService &auth, QWidget *parent = nullptr); UserSession session() const;
private slots: void login();
private: AuthService &auth_; QLineEdit *username_; QLineEdit *password_; QLabel *error_; UserSession session_;
};

