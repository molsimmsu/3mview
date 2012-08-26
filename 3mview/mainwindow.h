#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QGLWidget>
#include "aboutdialog.h"
#include "glwidget.h"
#include "ui_mainwindow.h"
#include <openbabel/mol.h>
#include <fstream>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
   void toggleFullscreenMode(bool tog);
   void openAboutDialog();

private slots:
   void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    AboutDialog *AboutDialogVar;
    GLWidget *glWidget;
    QString fileName;
    std::ofstream* ofs;
	OpenBabel::OBMol mol;
};

#endif // MAINWINDOW_H
