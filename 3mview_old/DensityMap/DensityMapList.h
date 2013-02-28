#ifndef DENSITYMAPLIST_H
#define DENSITYMAPLIST_H

#include <QObject>
#include "OpenStructure.h"
#include "OpenBabel.h"
#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QVector>
#include "DensityMap.h"
#include "ost/img/alg/randomize.hh"
#include "ost/img/alg/gaussian.hh"

class DensityMapList : public QObject
{
Q_OBJECT
public:
	explicit DensityMapList(QObject *parent = 0);
    bool loadFromFile(string path);
    bool createNew(string name, int xSize, int ySize, int zSize);
    void add(DensityMap* map);
    void add(vector<DensityMap*> maps);

signals:
	void addDensityMap(DensityMap* map);

public slots:
	void remove(DensityMap* m);

private:
	QVector<DensityMap*> maps;

    bool addNewDensityMap(ImageHandle h, string name, Color c);
};

#endif // MOLECULELIST_H
