#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "OpenStructure.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void resizeEvent ( QResizeEvent * event );

signals:

public slots:

protected:
	virtual void closeEvent(QCloseEvent * e);

private:
    ost::gui::GLCanvas* ostGL;
};

#endif // MAINWINDOW_H
