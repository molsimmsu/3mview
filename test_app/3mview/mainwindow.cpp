#include <QHBoxLayout>
#include <QLabel>
#include <QGLFormat>
#include <QMessageBox>
#include "mainwindow.h"
#include "aboutdialog.h"
#include "glwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGLFormat GLF;
    glWidget = new GLWidget(GLF, ui->centralWidget);
    GLboolean b;
    glGetBooleanv(GL_STEREO, &b);
    if (!b) {
        QMessageBox mb;
        mb.setText("Stereo not supported");
        // mb.exec();
    } else {
        QGLFormat GLF2(QGL::StereoBuffers);
        glWidget->setFormat(GLF2);
    }

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
