#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

class QLineEdit;

// 注册窗口
class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);

    QString userName() const;

private slots:
    void doRegister();

private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QLineEdit *pass2Edit;
};

#endif // REGISTERDIALOG_H
