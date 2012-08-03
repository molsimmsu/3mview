#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtOpenGL/QGLWidget>
#include "glwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *);

public slots:
    void OpenFile();

private:
    Ui::MainWindow *ui;
    GLWidget* widget;
};

#endif // MAINWINDOW_H
