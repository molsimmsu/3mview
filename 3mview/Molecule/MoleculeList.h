#ifndef MOLECULELIST_H
#define MOLECULELIST_H

#include <QObject>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QVector>

#include "OpenStructure.h"
#include "OpenBabel.h"
#include "Molecule.h"
#include "StrideReader.h"

using namespace ost::gfx;
using namespace ost::mol;
using namespace ost::gui;
using namespace ost::img;
using namespace geom;

class MoleculeList : public QObject
{
Q_OBJECT
public:
    explicit MoleculeList(QObject *parent = 0);
    bool loadFromFile(string path);

signals:
	void addMolecule(Molecule* mol);

public slots:

private:
	QVector<Molecule*> mols;

	bool addNewMolecule(EntityHandle h, const char* name, Color c);
	bool loadOpenStructure(const char* path);
	bool loadOpenBabel(const char* path);
};

#endif // MOLECULELIST_H
