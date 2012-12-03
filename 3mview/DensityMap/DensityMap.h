#ifndef DENSITYMAP_H
#define DENSITYMAP_H

#include "DataObject.h"
#include <string>
using namespace std;

#include "ost/gfx/map_iso.hh"
#include "ost/io/img/load_map.hh"
#include "ost/gfx/scene.hh"

class DensityMap : public DataObject
{
public:
    // Загрузка карты из файла
    DensityMap(string fileName, DensityMap* parent = 0);

    // Создание пустой карты
    DensityMap(int xSize, int ySize, int zSize, DensityMap* parent = 0);

    virtual string typeName() { return string("DensityMap"); }

    // Добавление в список отображения
    void addToScene();

    // Заполнение карты случайными данными
    void randomize();

	void setVisible(bool);
	void setSelection(bool state);
	void setLevel(double level);
	double minLevel();
	double maxLevel();
	double setColor(double r, double g, double b);

private:
    // Структура с данными
    ost::img::ImageHandle _map;
    ost::gfx::MapIso* _mapIso;
    ost::gfx::GfxNodeP _node;
};

#endif // DENSITYMAP_H
