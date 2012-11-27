#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:

protected:
	virtual void closeEvent(QCloseEvent * e);
};

#endif // MAINWINDOW_H
