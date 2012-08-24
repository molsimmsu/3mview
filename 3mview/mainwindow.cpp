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
		tr("Open File"), QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
		tr("Molecule model files (*.pdb)"));
	std::ifstream ifs(this->fileName.toUtf8().constData(), std::ifstream::binary);
	QMessageBox msg;
	if (!ifs) {
		msg.setText(this->fileName.toUtf8().constData());
		msg.exec();
		msg.setText(tr("Error opening file"));
		msg.exec();
		return;
	}
	if (ofs) {
		ofs->close();
		delete ofs;
	}
	ofs = new std::ofstream(this->fileName.toUtf8().constData(), std::ofstream::binary);
	OpenBabel::OBConversion conv(&ifs, ofs);
	std::string format = "PDB";
	if(!conv.SetInAndOutFormats(format.c_str(), format.c_str())) {
		msg.setText(tr("Error with formats "));
		msg.exec();
		return;
	}
	ifs.close();
}
