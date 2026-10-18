#ifndef DATABASE_H
#define DATABASE_H

#include <QString>

// 打开 health.db，建表，没有数据的时候插入测试数据
bool openDatabase();

// 密码转成 MD5 再存，数据库里不放明文
QString md5(const QString &text);

// 当前登录的用户名，登录成功以后才有值
QString currentUserName();
void setCurrentUserName(const QString &name);

#endif // DATABASE_H
