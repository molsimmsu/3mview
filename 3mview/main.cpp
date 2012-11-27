#include <QtGui>
#include <QDebug>
#include <QDesktopServices>

#include "MainWindow.h"
#include "DataLoaderWidget.h"
#include "MoleculeListWidget.h"
#include "DensityMapListWidget.h"
#include "OpenStructure.h"
#include "DensityMapList.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	MainWindow mainWidow;
	mainWidow.show();

	MoleculeList* ml = new MoleculeList();
	DensityMapList* dl = new DensityMapList();

	MoleculeListWidget moleculeList(ml);
	moleculeList.show();

	DensityMapListWidget densityMapList(dl);
	densityMapList.show();

	DataLoaderWidget dataLoader(ml, dl);
	dataLoader.show();

	ost::gui::GLWin* gl_win;
	QWidget* parent = &mainWidow;
	QGLFormat f;

	ost::gui::GLCanvas ostGL(gl_win, parent, f);
	ostGL.resize(mainWidow.width(), mainWidow.height());
	ostGL.show();

	ost::gfx::Scene& scene = ost::gfx::Scene::Instance();
	scene.SetBackground(ost::gfx::Color(0.1, 0.1, 0.5));
	scene.SetShowCenter(true);

   return a.exec();
}
