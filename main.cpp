#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
   w.show();
   w.move(QApplication::desktop()->screen()->rect().center() - w.rect().center());
    //w.showMaximized();

    /*
    QString test;
    QFile f(":/darkorange.stylesheet");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        // test =new  QString();
        test = in.readAll();
    }
    w.setStyleSheet(test);
*/
    return a.exec();
}
