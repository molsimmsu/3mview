#include <QtGui>
#include <QDebug>
#include <QDesktopServices>
#include <QGLFormat>

#include "MainWindow.h"
#include "DataLoaderWidget.h"
#include "Molecule/MoleculeListWidget.h"
#include "DensityMap/DensityMapList.h"
#include "DensityMap/DensityMapListWidget.h"
#include "ModulesWidget.h"

#include "Stereo.h"
#include "ObjectDispatcher.h"

#include "ost/base/platform.hh"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // TODO: РАЗОБРАТЬСЯ!
    ost::SetPrefixPath("/");

	MainWindow mainWidow;
	mainWidow.show();

// ---- Создание управляющих модулей и окон ---- //
    MoleculeList* ml = new MoleculeList();
    DensityMapList* dl = new DensityMapList();

    MoleculeListWidget moleculeList(ml);
    moleculeList.show();

    DensityMapListWidget densityMapList(dl);
    densityMapList.show();

    DataLoaderWidget dataLoader(ml, dl);
    dataLoader.show();

    ModulesWidget modulesWidget;
    modulesWidget.show();

    ObjectDispatcher d;
// --------------------------------------------- //

    Stereo stereo;
    stereo.activate();

   return a.exec();
}
