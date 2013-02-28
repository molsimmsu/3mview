#include "DataLoaderWidget.h"

DataLoaderWidget::DataLoaderWidget(MoleculeList *ml, DensityMapList *dl, QWidget *parent) :
   ml(ml),
   dl(dl),
   QWidget(parent)
{
	this->setWindowTitle("Load");
    setGeometry(750, 0, 195, 40);

	QPushButton* loadMolecule = new QPushButton("Molecule", this);
	loadMolecule->setGeometry(5, 5, 90, 30);

	QPushButton* loadDensityMap = new QPushButton("Density map", this);
	loadDensityMap->setGeometry(100, 5, 90, 30);

	connect(loadMolecule, SIGNAL(clicked()), SLOT(loadMolecule()));
	connect(loadDensityMap, SIGNAL(clicked()), SLOT(loadDensityMap()));
}

string DataLoaderWidget::browse(const char* filter)
{
	std::string _fileName = QFileDialog::getOpenFileName(0,
		QObject::tr("Load"), QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
		QObject::tr(filter)).toStdString();
    return _fileName;
}

void DataLoaderWidget::loadMolecule()
{
    ml->loadFromFile(browse("PDB (*.pdb)"));
}

void DataLoaderWidget::loadDensityMap()
{
    string path = browse("Density map (*.ccp4)");
    if (path == "") return;

    DensityMap* dm = new DensityMap(path);
    dm->setName(path);
    ObjectDispatcher::setName(dm);
    ObjectDispatcher::setColor(dm);
    dm->addToScene();
    dl->add(dm);
}
