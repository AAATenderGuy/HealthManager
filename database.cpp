#include "database.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

static QString g_currentUser;

QString md5(const QString &text)
{
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Md5).toHex();
}

QString currentUserName()
{
    return g_currentUser;
}

void setCurrentUserName(const QString &name)
{
    g_currentUser = name;
}

bool openDatabase()
{
    static bool opened = false;
    if (opened)
        return true;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("health.db");

    if (!db.open()) {
        qDebug() << "打开数据库失败:" << db.lastError().text();
        return false;
    }

    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS member("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT, gender TEXT, age INTEGER, phone TEXT, note TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS visit("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "member_id INTEGER, date TEXT, hospital TEXT,"
           "department TEXT, doctor TEXT, symptom TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS exam("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "member_id INTEGER, date TEXT, item TEXT, result TEXT, unit TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS diagnosis("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "member_id INTEGER, date TEXT, disease TEXT, advice TEXT)");

    // 登录用的账号表，username 不能重复
    q.exec("CREATE TABLE IF NOT EXISTS user("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "username TEXT UNIQUE, password TEXT, role TEXT, created_at TEXT)");

    opened = true;

    // 第一次运行，先放一个管理员账号
    q.exec("SELECT COUNT(*) FROM user");
    if (q.next() && q.value(0).toInt() == 0) {
        q.prepare("INSERT INTO user(username, password, role, created_at)"
                  " VALUES(?, ?, ?, ?)");
        q.addBindValue("admin");
        q.addBindValue(md5("123456"));
        q.addBindValue("管理员");
        q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        q.exec();
    }

    // 业务表也放几条测试数据，不然打开是空的
    q.exec("SELECT COUNT(*) FROM member");
    if (q.next() && q.value(0).toInt() == 0) {
        q.exec("INSERT INTO member(name, gender, age, phone, note)"
               " VALUES('陈建国', '男', 58, '13800000001', '高血压')");
        q.exec("INSERT INTO member(name, gender, age, phone, note)"
               " VALUES('林秀兰', '女', 55, '13800000002', '糖尿病')");
        q.exec("INSERT INTO member(name, gender, age, phone, note)"
               " VALUES('陈思远', '男', 27, '13800000003', '')");

        q.exec("INSERT INTO visit(member_id, date, hospital, department, doctor, symptom)"
               " VALUES(1, '2026-09-12', '市第一人民医院', '心内科', '王医生', '头晕、血压偏高')");
        q.exec("INSERT INTO visit(member_id, date, hospital, department, doctor, symptom)"
               " VALUES(2, '2026-09-20', '市第一人民医院', '内分泌科', '李医生', '血糖控制不好')");
        q.exec("INSERT INTO visit(member_id, date, hospital, department, doctor, symptom)"
               " VALUES(3, '2026-10-02', '社区卫生服务中心', '全科', '张医生', '感冒发烧')");

        q.exec("INSERT INTO exam(member_id, date, item, result, unit)"
               " VALUES(1, '2026-09-12', '收缩压', '152', 'mmHg')");
        q.exec("INSERT INTO exam(member_id, date, item, result, unit)"
               " VALUES(1, '2026-09-12', '空腹血糖', '7.4', 'mmol/L')");
        q.exec("INSERT INTO exam(member_id, date, item, result, unit)"
               " VALUES(2, '2026-09-20', '空腹血糖', '8.1', 'mmol/L')");
        q.exec("INSERT INTO exam(member_id, date, item, result, unit)"
               " VALUES(3, '2026-10-02', '体温', '38.5', '℃')");

        q.exec("INSERT INTO diagnosis(member_id, date, disease, advice)"
               " VALUES(1, '2026-09-12', '原发性高血压', '低盐饮食，按时吃药，一个月后复查')");
        q.exec("INSERT INTO diagnosis(member_id, date, disease, advice)"
               " VALUES(2, '2026-09-20', '2 型糖尿病', '控制主食，每天散步半小时')");
        q.exec("INSERT INTO diagnosis(member_id, date, disease, advice)"
               " VALUES(3, '2026-10-02', '上呼吸道感染', '多喝水，注意休息')");
    }

    return true;
}
