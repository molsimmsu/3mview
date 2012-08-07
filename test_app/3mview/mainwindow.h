#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QGLWidget>
#include "aboutdialog.h"
#include "glwidget.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
   void toggleFullscreenMode(bool tog);
   void openAboutDialog();

private:
    Ui::MainWindow *ui;
    AboutDialog *AboutDialogVar;
    GLWidget *glWidget;
};

#endif // MAINWINDOW_H
