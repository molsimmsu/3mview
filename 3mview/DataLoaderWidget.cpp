#include "DataLoaderWidget.h"

DataLoaderWidget::DataLoaderWidget(MoleculeList *ml, DensityMapList *dl, QWidget *parent) :
   ml(ml),
   dl(dl),
   QWidget(parent)
{
	this->setWindowTitle("Load");
	resize(195, 35);

	QPushButton* loadMolecule = new QPushButton("Molecule", this);
	loadMolecule->setGeometry(5, 5, 90, 30);

	QPushButton* loadDensityMap = new QPushButton("Density map", this);
	loadDensityMap->setGeometry(100, 5, 90, 30);

	connect(loadMolecule, SIGNAL(clicked()), SLOT(loadMolecule()));
	connect(loadDensityMap, SIGNAL(clicked()), SLOT(loadDensityMap()));
}

const char* DataLoaderWidget::browse(const char* filter)
{
	std::string _fileName = QFileDialog::getOpenFileName(0,
		QObject::tr("Load"), QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
		QObject::tr(filter)).toStdString();
	return _fileName.c_str();
}

void DataLoaderWidget::loadMolecule()
{
	ml->loadFromFile(browse("PDB (*.pdb)"));
}

void DataLoaderWidget::loadDensityMap()
{
	dl->loadFromFile(browse("Density map (*.ccp4)"));
}
