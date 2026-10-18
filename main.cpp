#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>

#include "database.h"
#include "logindialog.h"
#include "mainwindow.h"

// Qt 的样式表里用 border 拼三角形不生效，这里画一张小箭头图给下拉框用
static QString arrowImage()
{
    static QString path;
    if (path.isEmpty()) {
        const QString dir = QDir::tempPath() + QStringLiteral("/HealthManager");
        QDir().mkpath(dir);

        QPixmap pm(20, 20);
        pm.fill(Qt::transparent);

        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor("#7A8899"), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QPainterPath arrow;
        arrow.moveTo(6, 8);
        arrow.lineTo(10, 12);
        arrow.lineTo(14, 8);
        p.drawPath(arrow);
        p.end();

        path = dir + QStringLiteral("/arrow.png");
        pm.save(path, "PNG");
    }
    return path;
}

// 全局样式表，统一改一下颜色和圆角
static QString styleSheet()
{
    return QStringLiteral(R"(
QWidget { font-family: "Microsoft YaHei"; font-size: 13px; color: #2B3440; }
QMainWindow { background: #EEF2F7; }
QDialog { background: #FFFFFF; }

#titleLabel { font-size: 19px; font-weight: bold; color: #1E7BD6; padding-left: 4px; }

QTabWidget::pane { border: 1px solid #D9E1EC; border-radius: 8px; background: #FFFFFF; top: -1px; }
QTabBar::tab { background: #DFE7F1; color: #4A5A6E; padding: 9px 26px; margin-right: 3px;
               border-top-left-radius: 8px; border-top-right-radius: 8px; }
QTabBar::tab:selected { background: #FFFFFF; color: #1E7BD6; font-weight: bold; }
QTabBar::tab:hover { background: #EBF1F8; }

QLineEdit, QComboBox, QDateEdit { background: #FFFFFF; border: 1px solid #D3DCE7;
               border-radius: 6px; padding: 5px 8px; min-height: 20px; }
QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border: 1px solid #1E7BD6; }

QComboBox::drop-down, QDateEdit::drop-down { border: none; width: 22px; }
QComboBox::down-arrow, QDateEdit::down-arrow {
               image: url("%ARROW%"); width: 10px; height: 10px; margin-right: 6px; }

QPushButton { background: #1E7BD6; color: #FFFFFF; border: none; border-radius: 6px;
              padding: 7px 20px; }
QPushButton:hover { background: #3D92E4; }
QPushButton:pressed { background: #1663AE; }
QPushButton#dangerBtn { background: #E45757; }
QPushButton#dangerBtn:hover { background: #EE7373; }
QPushButton#cancelBtn { background: #8A99AC; }
QPushButton#plainBtn { background: #FFFFFF; color: #1E7BD6; border: 1px solid #1E7BD6; }
QPushButton#plainBtn:hover { background: #EAF3FD; }

QTableWidget { background: #FFFFFF; border: 1px solid #D9E1EC; border-radius: 6px;
               gridline-color: #EDF1F6; alternate-background-color: #F7FAFD; }
QHeaderView::section { background: #F0F4F9; color: #4A5A6E; border: none;
               border-bottom: 1px solid #D9E1EC; padding: 7px 6px; font-weight: bold; }
QTableWidget::item { padding: 4px; }
QTableWidget::item:selected { background: #DCEEFF; color: #14395E; }

#loginTitle { font-size: 21px; font-weight: bold; color: #1E7BD6; }
#loginSub { color: #7A8899; font-size: 12px; }
#loginHint { color: #A3AEBD; font-size: 11px; }
)").replace(QStringLiteral("%ARROW%"), arrowImage());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(styleSheet());

    if (!openDatabase()) {
        QMessageBox::critical(nullptr, "错误", "数据库打开失败！");
        return 1;
    }

    LoginDialog login;
    if (login.exec() != QDialog::Accepted)
        return 0;

    MainWindow w;
    w.show();

    return a.exec();
}
