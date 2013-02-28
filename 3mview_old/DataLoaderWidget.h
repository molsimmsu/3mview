#ifndef DATALOADERWIDGET_H
#define DATALOADERWIDGET_H

#include <QtGui>
#include "Molecule/MoleculeList.h"
#include "DensityMap/DensityMapList.h"
#include "ObjectDispatcher.h"

class DataLoaderWidget : public QWidget
{
Q_OBJECT
public:
	DataLoaderWidget(MoleculeList *ml, DensityMapList *dl, QWidget *parent = 0);

signals:

public slots:
	void loadMolecule();
	void loadDensityMap();

private:
    string browse(const char* filter);
	MoleculeList* ml;
	DensityMapList* dl;
};

#endif // DATALOADERWIDGET_H
