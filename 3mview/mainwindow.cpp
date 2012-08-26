#include <fstream>
#include <QHBoxLayout>
#include <QLabel>
#include <QGLFormat>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include <openbabel/obconversion.h>

#include "mainwindow.h"
#include "aboutdialog.h"
#include "glwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ofs = NULL;
	QGLFormat GLF;
	glWidget = new GLWidget(GLF, ui->centralWidget);
	GLboolean b;
	glGetBooleanv(GL_STEREO, &b);
	if (!b) {
		QMessageBox mb;
		mb.setText("Stereo not supported");
		// mb.exec();
		// TODO: write to log
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
	if (ofs) delete ofs;
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

void MainWindow::on_actionOpen_triggered()
{
	this->fileName = QFileDialog::getOpenFileName(this,
		// TODO: save previous location
		tr("Open File"), QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
		tr("Molecule model files (*.pdb)"));
	OpenBabel::OBConversion conv;
	std::string format = "PDB";
	conv.SetInFormat(format.c_str());
	bool notAnEnd = conv.ReadFile(&(this->mol), this->fileName.toUtf8().constData());
	// TODO: read 
}
