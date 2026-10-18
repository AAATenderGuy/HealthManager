#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;

// 登录窗口
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

private slots:
    void checkLogin();
    void openRegister();

private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
};

#endif // LOGINDIALOG_H
