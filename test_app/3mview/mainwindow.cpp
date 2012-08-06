#include <QHBoxLayout>
#include <QLabel>
#include <QGLFormat>
#include "mainwindow.h"
#include "aboutdialog.h"
#include "glwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGLFormat GLF(QGL::StereoBuffers);
    GLF.stereo();
    glWidget= new GLWidget(GLF,ui->centralWidget);
glWidget->setFormat(GLF);

   this->setCentralWidget(glWidget);


    connect(ui->actionFullscreen, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreenMode(bool)));
    connect(ui->actionAbout_3mview, SIGNAL(triggered()), this, SLOT(openAboutDialog()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete glWidget;
}

void MainWindow::openAboutDialog()
{
    AboutDialogVar = new AboutDialog(this);
    AboutDialogVar->show();

}

void MainWindow::toggleFullscreenMode(bool tog)
{
    if(tog) {this->showFullScreen();}
    else {this->showNormal();}


}
