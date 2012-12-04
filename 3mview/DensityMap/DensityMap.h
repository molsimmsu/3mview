#ifndef DENSITYMAP_H
#define DENSITYMAP_H

#include "DataObject.h"
#include <string>
using namespace std;

#include "ost/gfx/map_iso.hh"
#include "ost/io/img/load_map.hh"
#include "ost/gfx/scene.hh"

typedef struct DensityMapSize
{
    unsigned int x, y, z;
}
DensityMapSize;

class DensityMap : public DataObject
{
public:
    // Загрузка карты из файла
    DensityMap(string fileName, DensityMap* parent = 0);

    // Создание пустой карты
    DensityMap(unsigned int xSize, unsigned int ySize, unsigned int zSize, DensityMap* parent = 0);
    DensityMap(DensityMapSize size, DensityMap* parent = 0);

    virtual string typeName() { return string("DensityMap"); }

    // Добавление в список отображения
    void addToScene();

    // Заполнение карты случайными данными
    void randomize();

    // Размер карты по 3-м измерениям
    DensityMapSize size();

    // Получение и задание значений элементов карты
    double getValue(unsigned int x, unsigned int y, unsigned int z);
    double setValue(unsigned int x, unsigned int y, unsigned int z, double value);

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
