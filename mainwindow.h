#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QComboBox;
class QDateEdit;
class QLineEdit;
class QTableWidget;

// 家庭健康管理系统主窗口
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    // 家庭成员
    void addMember();
    void deleteMember();
    // 就诊记录
    void addVisit();
    void deleteVisit();
    // 体检结果
    void addExam();
    void deleteExam();
    // 诊断建议
    void addDiagnosis();
    void deleteDiagnosis();

private:
    void initUi();

    void loadMembers();
    void loadVisits();
    void loadExams();
    void loadDiagnoses();
    void loadMemberCombo();

    // 家庭成员页
    QTableWidget *memberTable;
    QLineEdit *nameEdit;
    QComboBox *genderBox;
    QLineEdit *ageEdit;
    QLineEdit *phoneEdit;
    QLineEdit *noteEdit;

    // 就诊记录页
    QTableWidget *visitTable;
    QComboBox *visitMemberBox;
    QDateEdit *visitDate;
    QLineEdit *hospitalEdit;
    QLineEdit *departmentEdit;
    QLineEdit *doctorEdit;
    QLineEdit *symptomEdit;

    // 体检结果页
    QTableWidget *examTable;
    QComboBox *examMemberBox;
    QDateEdit *examDate;
    QLineEdit *itemEdit;
    QLineEdit *resultEdit;
    QLineEdit *unitEdit;

    // 诊断建议页
    QTableWidget *diagTable;
    QComboBox *diagMemberBox;
    QDateEdit *diagDate;
    QLineEdit *diseaseEdit;
    QLineEdit *adviceEdit;
};

#endif // MAINWINDOW_H
