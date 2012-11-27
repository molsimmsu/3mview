#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	resize(700, 700);
}

void MainWindow::closeEvent(QCloseEvent * e)
{
	QApplication::exit();
}
