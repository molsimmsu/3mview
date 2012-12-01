#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setGeometry(0, 0, 700, 700);
}

void MainWindow::closeEvent(QCloseEvent * e)
{
	QApplication::exit();
}
