#include "DensityMap.h"

DensityMap::DensityMap(const char* name, ost::gfx::MapIso* map, QObject *parent) :
	_name(name),
	map(map),
	QObject(parent)
{
}

void DensityMap::setVisible(bool state)
{

}

void DensityMap::setLevel(double level)
{
	map->SetLevel(level);
}

void DensityMap::setSelection(bool state)
{

}

const char* DensityMap::name()
{
	return _name;
}

double DensityMap::minLevel()
{
	return map->GetMinLevel();
}

double DensityMap::maxLevel()
{
	return map->GetMaxLevel();
}

double DensityMap::setColor(double r, double g, double b)
{
	map->SetColor(Color(r, g, b));
}
