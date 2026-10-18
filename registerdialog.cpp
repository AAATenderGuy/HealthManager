#include "registerdialog.h"

#include "database.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QVariant>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("注册");
    setFixedSize(380, 330);

    QLabel *titleLabel = new QLabel("注册新账号");
    titleLabel->setObjectName("loginTitle");

    QLabel *subLabel = new QLabel("用户名至少 3 位，密码至少 6 位");
    subLabel->setObjectName("loginSub");

    userEdit = new QLineEdit;
    userEdit->setPlaceholderText("用户名");
    passEdit = new QLineEdit;
    passEdit->setPlaceholderText("密码");
    passEdit->setEchoMode(QLineEdit::Password);
    pass2Edit = new QLineEdit;
    pass2Edit->setPlaceholderText("再输一次密码");
    pass2Edit->setEchoMode(QLineEdit::Password);

    QPushButton *okBtn = new QPushButton("注 册");
    okBtn->setDefault(true);
    QPushButton *cancelBtn = new QPushButton("取 消");
    cancelBtn->setObjectName("cancelBtn");

    QHBoxLayout *btnLayout = new QHBoxLayout;
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
    layout->addWidget(pass2Edit);
    layout->addSpacing(6);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &RegisterDialog::doRegister);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

QString RegisterDialog::userName() const
{
    return userEdit->text().trimmed();
}

void RegisterDialog::doRegister()
{
    const QString name = userEdit->text().trimmed();
    const QString pass1 = passEdit->text();
    const QString pass2 = pass2Edit->text();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名");
        return;
    }
    if (name.length() < 3) {
        QMessageBox::warning(this, "提示", "用户名至少 3 个字符");
        return;
    }
    if (pass1.length() < 6) {
        QMessageBox::warning(this, "提示", "密码至少 6 位");
        return;
    }
    if (pass1 != pass2) {
        QMessageBox::warning(this, "提示", "两次输入的密码不一样");
        return;
    }

    // 先查一下用户名有没有被占用
    QSqlQuery q;
    q.prepare("SELECT id FROM user WHERE username = ?");
    q.addBindValue(name);
    if (q.exec() && q.next()) {
        QMessageBox::warning(this, "提示", "这个用户名已经有人用了，换一个吧");
        return;
    }

    q.prepare("INSERT INTO user(username, password, role, created_at) VALUES(?, ?, ?, ?)");
    q.addBindValue(name);
    q.addBindValue(md5(pass1));
    q.addBindValue("普通用户");
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!q.exec()) {
        QMessageBox::warning(this, "注册失败", q.lastError().text());
        return;
    }

    accept();
}
