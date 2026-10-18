#include "logindialog.h"

#include "database.h"
#include "registerdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QVariant>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("登录");
    setFixedSize(380, 310);

    QLabel *titleLabel = new QLabel("家庭健康管理系统");
    titleLabel->setObjectName("loginTitle");

    QLabel *subLabel = new QLabel("请输入账号和密码");
    subLabel->setObjectName("loginSub");

    userEdit = new QLineEdit;
    userEdit->setPlaceholderText("用户名");
    passEdit = new QLineEdit;
    passEdit->setPlaceholderText("密码");
    passEdit->setEchoMode(QLineEdit::Password);

    QLabel *hintLabel = new QLabel("第一次使用可以点「注册」创建账号");
    hintLabel->setObjectName("loginHint");

    QPushButton *registerBtn = new QPushButton("注 册");
    registerBtn->setObjectName("plainBtn");
    QPushButton *okBtn = new QPushButton("登 录");
    okBtn->setDefault(true);
    QPushButton *cancelBtn = new QPushButton("退 出");
    cancelBtn->setObjectName("cancelBtn");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(registerBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 28, 36, 24);
    layout->setSpacing(10);
    layout->addWidget(titleLabel);
    layout->addWidget(subLabel);
    layout->addSpacing(8);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addWidget(hintLabel);
    layout->addSpacing(6);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &LoginDialog::checkLogin);
    connect(registerBtn, &QPushButton::clicked, this, &LoginDialog::openRegister);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(passEdit, &QLineEdit::returnPressed, this, &LoginDialog::checkLogin);
}

// 到数据库里查这个用户名和密码对不对
void LoginDialog::checkLogin()
{
    const QString name = userEdit->text().trimmed();
    const QString pass = passEdit->text();

    if (name.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码都不能为空");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT username FROM user WHERE username = ? AND password = ?");
    q.addBindValue(name);
    q.addBindValue(md5(pass));

    if (q.exec() && q.next()) {
        setCurrentUserName(q.value(0).toString());
        accept();
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码不正确！");
        passEdit->clear();
        passEdit->setFocus();
    }
}

// 打开注册窗口，注册成功以后把用户名填回来
void LoginDialog::openRegister()
{
    RegisterDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        userEdit->setText(dlg.userName());
        passEdit->clear();
        passEdit->setFocus();
        QMessageBox::information(this, "注册成功", "账号已创建，请用新账号登录。");
    }
}
