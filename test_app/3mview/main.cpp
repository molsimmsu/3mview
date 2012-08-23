#include <QtGui/QApplication>
#include "mainwindow.h"
#include "aboutdialog.h"
#include <clocale>

int main(int argc, char *argv[])
{
    setlocale(LC_CTYPE,"Russian");
    QApplication a(argc, argv);
    MainWindow w;
	w.show();

   return a.exec();
}
