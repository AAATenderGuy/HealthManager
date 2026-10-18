#include "mainwindow.h"

#include "database.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVariant>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("家庭健康管理系统 - " + currentUserName());
    resize(940, 580);

    openDatabase();
    initUi();

    loadMemberCombo();
    loadMembers();
    loadVisits();
    loadExams();
    loadDiagnoses();
}

void MainWindow::initUi()
{
    QTabWidget *tabs = new QTabWidget(this);

    // 顶上一行大标题
    QLabel *titleLabel = new QLabel("家庭健康管理系统");
    titleLabel->setObjectName("titleLabel");

    QWidget *central = new QWidget;
    QVBoxLayout *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(14, 10, 14, 14);
    rootLayout->setSpacing(8);
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(tabs);
    setCentralWidget(central);

    // ---------------- 家庭成员 ----------------
    QWidget *memberPage = new QWidget;
    nameEdit = new QLineEdit;
    genderBox = new QComboBox;
    genderBox->addItems({"男", "女"});
    ageEdit = new QLineEdit;
    phoneEdit = new QLineEdit;
    noteEdit = new QLineEdit;
    noteEdit->setPlaceholderText("备注");

    QPushButton *addMemberBtn = new QPushButton("添加");
    QPushButton *delMemberBtn = new QPushButton("删除选中");
    delMemberBtn->setObjectName("dangerBtn");

    QHBoxLayout *memberForm = new QHBoxLayout;
    memberForm->addWidget(new QLabel("姓名"));
    memberForm->addWidget(nameEdit);
    memberForm->addWidget(new QLabel("性别"));
    memberForm->addWidget(genderBox);
    memberForm->addWidget(new QLabel("年龄"));
    memberForm->addWidget(ageEdit);
    memberForm->addWidget(new QLabel("电话"));
    memberForm->addWidget(phoneEdit);
    memberForm->addWidget(noteEdit);
    memberForm->addWidget(addMemberBtn);
    memberForm->addWidget(delMemberBtn);

    memberTable = new QTableWidget;
    memberTable->setColumnCount(6);
    memberTable->setHorizontalHeaderLabels({"ID", "姓名", "性别", "年龄", "电话", "备注"});
    memberTable->setColumnHidden(0, true);
    memberTable->horizontalHeader()->setStretchLastSection(true);
    memberTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    memberTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    memberTable->setAlternatingRowColors(true);
    memberTable->verticalHeader()->setVisible(false);
    memberTable->verticalHeader()->setDefaultSectionSize(34);
    memberTable->setColumnWidth(1, 120);
    memberTable->setColumnWidth(2, 80);
    memberTable->setColumnWidth(3, 80);
    memberTable->setColumnWidth(4, 160);

    QVBoxLayout *memberLayout = new QVBoxLayout(memberPage);
    memberLayout->addLayout(memberForm);
    memberLayout->addWidget(memberTable);

    tabs->addTab(memberPage, "家庭成员");

    connect(addMemberBtn, &QPushButton::clicked, this, &MainWindow::addMember);
    connect(delMemberBtn, &QPushButton::clicked, this, &MainWindow::deleteMember);

    // ---------------- 就诊记录 ----------------
    QWidget *visitPage = new QWidget;
    visitMemberBox = new QComboBox;
    visitDate = new QDateEdit(QDate::currentDate());
    visitDate->setDisplayFormat("yyyy-MM-dd");
    visitDate->setCalendarPopup(true);
    visitDate->setMinimumWidth(145);
    hospitalEdit = new QLineEdit;
    hospitalEdit->setPlaceholderText("医院");
    departmentEdit = new QLineEdit;
    departmentEdit->setPlaceholderText("科室");
    doctorEdit = new QLineEdit;
    doctorEdit->setPlaceholderText("医生");
    symptomEdit = new QLineEdit;
    symptomEdit->setPlaceholderText("症状");

    QPushButton *addVisitBtn = new QPushButton("添加");
    QPushButton *delVisitBtn = new QPushButton("删除选中");
    delVisitBtn->setObjectName("dangerBtn");

    QHBoxLayout *visitForm = new QHBoxLayout;
    visitForm->addWidget(new QLabel("成员"));
    visitForm->addWidget(visitMemberBox);
    visitForm->addWidget(new QLabel("日期"));
    visitForm->addWidget(visitDate);
    visitForm->addWidget(hospitalEdit);
    visitForm->addWidget(departmentEdit);
    visitForm->addWidget(doctorEdit);
    visitForm->addWidget(symptomEdit);
    visitForm->addWidget(addVisitBtn);
    visitForm->addWidget(delVisitBtn);

    visitTable = new QTableWidget;
    visitTable->setColumnCount(7);
    visitTable->setHorizontalHeaderLabels({"ID", "成员", "日期", "医院", "科室", "医生", "症状"});
    visitTable->setColumnHidden(0, true);
    visitTable->horizontalHeader()->setStretchLastSection(true);
    visitTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    visitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    visitTable->setAlternatingRowColors(true);
    visitTable->verticalHeader()->setVisible(false);
    visitTable->verticalHeader()->setDefaultSectionSize(34);
    visitTable->setColumnWidth(1, 100);
    visitTable->setColumnWidth(2, 110);
    visitTable->setColumnWidth(3, 220);
    visitTable->setColumnWidth(4, 110);
    visitTable->setColumnWidth(5, 100);

    QVBoxLayout *visitLayout = new QVBoxLayout(visitPage);
    visitLayout->addLayout(visitForm);
    visitLayout->addWidget(visitTable);

    tabs->addTab(visitPage, "就诊记录");

    connect(addVisitBtn, &QPushButton::clicked, this, &MainWindow::addVisit);
    connect(delVisitBtn, &QPushButton::clicked, this, &MainWindow::deleteVisit);

    // ---------------- 体检结果 ----------------
    QWidget *examPage = new QWidget;
    examMemberBox = new QComboBox;
    examDate = new QDateEdit(QDate::currentDate());
    examDate->setDisplayFormat("yyyy-MM-dd");
    examDate->setCalendarPopup(true);
    examDate->setMinimumWidth(145);
    itemEdit = new QLineEdit;
    itemEdit->setPlaceholderText("检查项目");
    resultEdit = new QLineEdit;
    resultEdit->setPlaceholderText("结果");
    unitEdit = new QLineEdit;
    unitEdit->setPlaceholderText("单位");

    QPushButton *addExamBtn = new QPushButton("添加");
    QPushButton *delExamBtn = new QPushButton("删除选中");
    delExamBtn->setObjectName("dangerBtn");

    QHBoxLayout *examForm = new QHBoxLayout;
    examForm->addWidget(new QLabel("成员"));
    examForm->addWidget(examMemberBox);
    examForm->addWidget(new QLabel("日期"));
    examForm->addWidget(examDate);
    examForm->addWidget(itemEdit);
    examForm->addWidget(resultEdit);
    examForm->addWidget(unitEdit);
    examForm->addWidget(addExamBtn);
    examForm->addWidget(delExamBtn);

    examTable = new QTableWidget;
    examTable->setColumnCount(6);
    examTable->setHorizontalHeaderLabels({"ID", "成员", "日期", "项目", "结果", "单位"});
    examTable->setColumnHidden(0, true);
    examTable->horizontalHeader()->setStretchLastSection(true);
    examTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    examTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    examTable->setAlternatingRowColors(true);
    examTable->verticalHeader()->setVisible(false);
    examTable->verticalHeader()->setDefaultSectionSize(34);
    examTable->setColumnWidth(1, 100);
    examTable->setColumnWidth(2, 110);
    examTable->setColumnWidth(3, 160);
    examTable->setColumnWidth(4, 100);

    QVBoxLayout *examLayout = new QVBoxLayout(examPage);
    examLayout->addLayout(examForm);
    examLayout->addWidget(examTable);

    tabs->addTab(examPage, "体检结果");

    connect(addExamBtn, &QPushButton::clicked, this, &MainWindow::addExam);
    connect(delExamBtn, &QPushButton::clicked, this, &MainWindow::deleteExam);

    // ---------------- 诊断建议 ----------------
    QWidget *diagPage = new QWidget;
    diagMemberBox = new QComboBox;
    diagDate = new QDateEdit(QDate::currentDate());
    diagDate->setDisplayFormat("yyyy-MM-dd");
    diagDate->setCalendarPopup(true);
    diagDate->setMinimumWidth(145);
    diseaseEdit = new QLineEdit;
    diseaseEdit->setPlaceholderText("诊断结果");
    adviceEdit = new QLineEdit;
    adviceEdit->setPlaceholderText("康复建议");

    QPushButton *addDiagBtn = new QPushButton("添加");
    QPushButton *delDiagBtn = new QPushButton("删除选中");
    delDiagBtn->setObjectName("dangerBtn");

    QHBoxLayout *diagForm = new QHBoxLayout;
    diagForm->addWidget(new QLabel("成员"));
    diagForm->addWidget(diagMemberBox);
    diagForm->addWidget(new QLabel("日期"));
    diagForm->addWidget(diagDate);
    diagForm->addWidget(diseaseEdit);
    diagForm->addWidget(adviceEdit);
    diagForm->addWidget(addDiagBtn);
    diagForm->addWidget(delDiagBtn);

    diagTable = new QTableWidget;
    diagTable->setColumnCount(5);
    diagTable->setHorizontalHeaderLabels({"ID", "成员", "日期", "诊断", "建议"});
    diagTable->setColumnHidden(0, true);
    diagTable->horizontalHeader()->setStretchLastSection(true);
    diagTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    diagTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    diagTable->setAlternatingRowColors(true);
    diagTable->verticalHeader()->setVisible(false);
    diagTable->verticalHeader()->setDefaultSectionSize(34);
    diagTable->setColumnWidth(1, 100);
    diagTable->setColumnWidth(2, 110);
    diagTable->setColumnWidth(3, 180);

    QVBoxLayout *diagLayout = new QVBoxLayout(diagPage);
    diagLayout->addLayout(diagForm);
    diagLayout->addWidget(diagTable);

    tabs->addTab(diagPage, "诊断建议");

    connect(addDiagBtn, &QPushButton::clicked, this, &MainWindow::addDiagnosis);
    connect(delDiagBtn, &QPushButton::clicked, this, &MainWindow::deleteDiagnosis);
}

// 把成员表里的名字放到三个下拉框里
void MainWindow::loadMemberCombo()
{
    QComboBox *boxes[3] = {visitMemberBox, examMemberBox, diagMemberBox};

    for (int i = 0; i < 3; ++i) {
        boxes[i]->clear();
        QSqlQuery q;
        q.exec("SELECT id, name FROM member ORDER BY id");
        while (q.next())
            boxes[i]->addItem(q.value(1).toString(), q.value(0).toInt());
    }
}

void MainWindow::loadMembers()
{
    memberTable->setRowCount(0);

    QSqlQuery q;
    q.exec("SELECT id, name, gender, age, phone, note FROM member ORDER BY id");

    int row = 0;
    while (q.next()) {
        memberTable->insertRow(row);
        for (int col = 0; col < 6; ++col)
            memberTable->setItem(row, col, new QTableWidgetItem(q.value(col).toString()));
        ++row;
    }
}

void MainWindow::loadVisits()
{
    visitTable->setRowCount(0);

    QSqlQuery q;
    q.exec("SELECT visit.id, member.name, visit.date, visit.hospital,"
           " visit.department, visit.doctor, visit.symptom"
           " FROM visit LEFT JOIN member ON member.id = visit.member_id"
           " ORDER BY visit.date DESC");

    int row = 0;
    while (q.next()) {
        visitTable->insertRow(row);
        for (int col = 0; col < 7; ++col)
            visitTable->setItem(row, col, new QTableWidgetItem(q.value(col).toString()));
        ++row;
    }
}

void MainWindow::loadExams()
{
    examTable->setRowCount(0);

    QSqlQuery q;
    q.exec("SELECT exam.id, member.name, exam.date, exam.item, exam.result, exam.unit"
           " FROM exam LEFT JOIN member ON member.id = exam.member_id"
           " ORDER BY exam.date DESC");

    int row = 0;
    while (q.next()) {
        examTable->insertRow(row);
        for (int col = 0; col < 6; ++col)
            examTable->setItem(row, col, new QTableWidgetItem(q.value(col).toString()));
        ++row;
    }
}

void MainWindow::loadDiagnoses()
{
    diagTable->setRowCount(0);

    QSqlQuery q;
    q.exec("SELECT diagnosis.id, member.name, diagnosis.date,"
           " diagnosis.disease, diagnosis.advice"
           " FROM diagnosis LEFT JOIN member ON member.id = diagnosis.member_id"
           " ORDER BY diagnosis.date DESC");

    int row = 0;
    while (q.next()) {
        diagTable->insertRow(row);
        for (int col = 0; col < 5; ++col)
            diagTable->setItem(row, col, new QTableWidgetItem(q.value(col).toString()));
        ++row;
    }
}

void MainWindow::addMember()
{
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "姓名不能为空");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO member(name, gender, age, phone, note) VALUES(?, ?, ?, ?, ?)");
    q.addBindValue(nameEdit->text());
    q.addBindValue(genderBox->currentText());
    q.addBindValue(ageEdit->text().toInt());
    q.addBindValue(phoneEdit->text());
    q.addBindValue(noteEdit->text());

    if (!q.exec()) {
        QMessageBox::warning(this, "添加失败", q.lastError().text());
        return;
    }

    nameEdit->clear();
    ageEdit->clear();
    phoneEdit->clear();
    noteEdit->clear();

    loadMembers();
    loadMemberCombo();
}

void MainWindow::deleteMember()
{
    int row = memberTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在表格里选中一行");
        return;
    }

    int id = memberTable->item(row, 0)->text().toInt();

    if (QMessageBox::question(this, "确认", "确定要删除这个成员吗？")
        != QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.exec("DELETE FROM visit WHERE member_id = " + QString::number(id));
    q.exec("DELETE FROM exam WHERE member_id = " + QString::number(id));
    q.exec("DELETE FROM diagnosis WHERE member_id = " + QString::number(id));
    q.exec("DELETE FROM member WHERE id = " + QString::number(id));

    loadMembers();
    loadMemberCombo();
    loadVisits();
    loadExams();
    loadDiagnoses();
}

void MainWindow::addVisit()
{
    if (visitMemberBox->count() == 0) {
        QMessageBox::warning(this, "提示", "请先添加家庭成员");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO visit(member_id, date, hospital, department, doctor, symptom)"
              " VALUES(?, ?, ?, ?, ?, ?)");
    q.addBindValue(visitMemberBox->currentData().toInt());
    q.addBindValue(visitDate->date().toString("yyyy-MM-dd"));
    q.addBindValue(hospitalEdit->text());
    q.addBindValue(departmentEdit->text());
    q.addBindValue(doctorEdit->text());
    q.addBindValue(symptomEdit->text());

    if (!q.exec()) {
        QMessageBox::warning(this, "添加失败", q.lastError().text());
        return;
    }

    hospitalEdit->clear();
    departmentEdit->clear();
    doctorEdit->clear();
    symptomEdit->clear();

    loadVisits();
}

void MainWindow::deleteVisit()
{
    int row = visitTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在表格里选中一行");
        return;
    }

    int id = visitTable->item(row, 0)->text().toInt();

    QSqlQuery q;
    q.exec("DELETE FROM visit WHERE id = " + QString::number(id));

    loadVisits();
}

void MainWindow::addExam()
{
    if (examMemberBox->count() == 0) {
        QMessageBox::warning(this, "提示", "请先添加家庭成员");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO exam(member_id, date, item, result, unit) VALUES(?, ?, ?, ?, ?)");
    q.addBindValue(examMemberBox->currentData().toInt());
    q.addBindValue(examDate->date().toString("yyyy-MM-dd"));
    q.addBindValue(itemEdit->text());
    q.addBindValue(resultEdit->text());
    q.addBindValue(unitEdit->text());

    if (!q.exec()) {
        QMessageBox::warning(this, "添加失败", q.lastError().text());
        return;
    }

    itemEdit->clear();
    resultEdit->clear();
    unitEdit->clear();

    loadExams();
}

void MainWindow::deleteExam()
{
    int row = examTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在表格里选中一行");
        return;
    }

    int id = examTable->item(row, 0)->text().toInt();

    QSqlQuery q;
    q.exec("DELETE FROM exam WHERE id = " + QString::number(id));

    loadExams();
}

void MainWindow::addDiagnosis()
{
    if (diagMemberBox->count() == 0) {
        QMessageBox::warning(this, "提示", "请先添加家庭成员");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO diagnosis(member_id, date, disease, advice) VALUES(?, ?, ?, ?)");
    q.addBindValue(diagMemberBox->currentData().toInt());
    q.addBindValue(diagDate->date().toString("yyyy-MM-dd"));
    q.addBindValue(diseaseEdit->text());
    q.addBindValue(adviceEdit->text());

    if (!q.exec()) {
        QMessageBox::warning(this, "添加失败", q.lastError().text());
        return;
    }

    diseaseEdit->clear();
    adviceEdit->clear();

    loadDiagnoses();
}

void MainWindow::deleteDiagnosis()
{
    int row = diagTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先在表格里选中一行");
        return;
    }

    int id = diagTable->item(row, 0)->text().toInt();

    QSqlQuery q;
    q.exec("DELETE FROM diagnosis WHERE id = " + QString::number(id));

    loadDiagnoses();
}
