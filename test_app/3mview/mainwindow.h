#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class AboutDialog;
class GLWidget;
class QHBoxLayout;
class QLabel;



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
