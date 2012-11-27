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

using namespace ost::gfx;
using namespace ost::mol;
using namespace ost::gui;
using namespace ost::img;
using namespace geom;

class DensityMapList : public QObject
{
Q_OBJECT
public:
	explicit DensityMapList(QObject *parent = 0);
	bool loadFromFile(const char* path);

signals:
	void addDensityMap(DensityMap* map);

public slots:
	void remove(DensityMap* m);

private:
	QVector<DensityMap*> maps;

	bool addNewDensityMap(ImageHandle h, const char* name, Color c);
};

#endif // MOLECULELIST_H
