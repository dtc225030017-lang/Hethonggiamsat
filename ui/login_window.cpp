#include "login_window.h"
#include "auth_service.h"

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginWindow::LoginWindow(AuthService &auth, QWidget *parent)
    : QDialog(parent), auth_(auth)
{
    setWindowTitle(QStringLiteral("Đăng nhập — Hệ thống giám sát"));
    setFixedSize(470, 480);

    auto *mark = new QLabel(QStringLiteral("AQ"));
    mark->setAlignment(Qt::AlignCenter);
    mark->setFixedSize(58, 58);
    mark->setStyleSheet(QStringLiteral(
        "background:#2563eb;color:white;border-radius:16px;"
        "font-size:18pt;font-weight:800;"));

    auto *title = new QLabel(QStringLiteral("Chào mừng trở lại"));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QStringLiteral(
        "color:#0f172a;font-size:20pt;font-weight:700;"));
    auto *subtitle = new QLabel(
        QStringLiteral("Đăng nhập để theo dõi chất lượng không khí"));
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(QStringLiteral("color:#64748b;"));

    auto *usernameLabel = new QLabel(QStringLiteral("Tên đăng nhập"));
    auto *passwordLabel = new QLabel(QStringLiteral("Mật khẩu"));
    usernameLabel->setStyleSheet(QStringLiteral("font-weight:600;color:#334155;"));
    passwordLabel->setStyleSheet(QStringLiteral("font-weight:600;color:#334155;"));

    username_ = new QLineEdit;
    password_ = new QLineEdit;
    password_->setEchoMode(QLineEdit::Password);
    username_->setPlaceholderText(QStringLiteral("Nhập tên đăng nhập"));
    password_->setPlaceholderText(QStringLiteral("Nhập mật khẩu"));
    username_->setMinimumHeight(40);
    password_->setMinimumHeight(40);

    error_ = new QLabel;
    error_->setStyleSheet(QStringLiteral(
        "color:#b91c1c;background:#fee2e2;border-radius:6px;padding:7px;"));
    error_->setWordWrap(true);
    error_->hide();

    auto *loginButton = new QPushButton(QStringLiteral("Đăng nhập"));
    loginButton->setDefault(true);
    loginButton->setMinimumHeight(42);
    auto *exitButton = new QPushButton(QStringLiteral("Thoát"));
    exitButton->setMinimumHeight(38);
    exitButton->setStyleSheet(QStringLiteral(
        "QPushButton{background:transparent;color:#64748b;}"
        "QPushButton:hover{background:#e2e8f0;color:#334155;}"));

    auto *card = new QFrame;
    card->setObjectName(QStringLiteral("loginCard"));
    card->setStyleSheet(QStringLiteral(
        "QFrame#loginCard{background:white;border:1px solid #e2e8f0;"
        "border-radius:14px;}"));
    auto *content = new QVBoxLayout(card);
    content->setContentsMargins(34, 28, 34, 24);
    content->setSpacing(10);
    content->addWidget(mark, 0, Qt::AlignHCenter);
    content->addSpacing(5);
    content->addWidget(title);
    content->addWidget(subtitle);
    content->addSpacing(14);
    content->addWidget(usernameLabel);
    content->addWidget(username_);
    content->addWidget(passwordLabel);
    content->addWidget(password_);
    content->addWidget(error_);
    content->addSpacing(4);
    content->addWidget(loginButton);
    content->addWidget(exitButton);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->addWidget(card);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::login);
    connect(exitButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(password_, &QLineEdit::returnPressed, this, &LoginWindow::login);
}

void LoginWindow::login()
{
    QString error;
    if (auth_.authenticate(username_->text(), password_->text(), &session_, &error)) {
        accept();
    } else {
        error_->setText(error);
        error_->show();
        password_->clear();
        password_->setFocus();
    }
}

UserSession LoginWindow::session() const
{
    return session_;
}
