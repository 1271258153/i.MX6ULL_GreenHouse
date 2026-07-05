#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/style.qss");
    if(file.exists()){
        file.open(QFile::ReadOnly);
        QString style = QLatin1String(file.readAll());
        qApp->setStyleSheet(style);
        file.close();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
