#ifndef DENSITYMAP_H
#define DENSITYMAP_H

#include <QObject>
#include "OpenStructure.h"

class DensityMap : public QObject
{
Q_OBJECT
public:
	explicit DensityMap(const char* name, ost::gfx::MapIso* map, QObject *parent = 0);
	const char* name();
	void setVisible(bool);
	void setSelection(bool state);
	void setLevel(double level);
	double minLevel();
	double maxLevel();
	double setColor(double r, double g, double b);
signals:

public slots:

private:
	const char* _name;
	ost::gfx::MapIso* map;
};

#endif // DENSITYMAP_H
