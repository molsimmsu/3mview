#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtOpenGL/QGLWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    widget(NULL)
{
    ui->setupUi(this);
#ifdef QT_WS_MAC
    QMenuBar* mb = new QMenuBar(0);
#else
    QMenuBar* mb = new QMenuBar(this);
#ifdef QT_WS_WINDOWS
    // TODO: set geometry on windows
#endif
#endif
    QMenu* fileMenu = mb->addMenu(QString("File"));
    QAction* openFileAction = fileMenu->addAction(QString("Open..."));
    QAction* exitAction = fileMenu->addAction(QString("Exit"));
    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(openFileAction, SIGNAL(triggered()), this, SLOT(OpenFile()));
    widget = new GLWidget(this);
    QRect geo = this->geometry();
    widget->setGeometry(geo);
}

void MainWindow::resizeEvent(QResizeEvent *) {
    QRect geo = this->geometry();
    /*if (widget) {
        widget->hide();
        widget->setGeometry(geo);
        widget->show();
    }*/
}

void MainWindow::OpenFile() {
    // TODO: show file open dialog
}

MainWindow::~MainWindow()
{
    delete ui;
}
