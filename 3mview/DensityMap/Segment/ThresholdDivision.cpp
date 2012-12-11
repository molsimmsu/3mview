#include "ThresholdDivision.h"
#include <QDebug>

ThresholdDivision::ThresholdDivision(unsigned int numMaps):
    _numMaps(numMaps)
{
}

vector<DensityMap*> ThresholdDivision::apply(DensityMap *map)
{
    // Результат сегментирования
    vector<DensityMap*> res;

    double min = map->minLevel();
    double max = map->maxLevel();
    qDebug() << "min" << min << "max" << max;
    double len = (max - min) / _numMaps;

    for (unsigned int i = 0; i < _numMaps; i++)
    {
        double low = min + i * len;
        double high = low + len;
        DensityMap* newMap = createMap(map, low, high);
        res.push_back(newMap);
    }

    return res;
}

DensityMap *ThresholdDivision::createMap(DensityMap *map, double low, double high)
{
    qDebug() << "low" << low << "high" << high;
    DensityMap* m = new DensityMap(map->size());

    for (unsigned int i = 0; i < map->size().x; i++)
        for (unsigned int j = 0; j < map->size().y; j++)
            for (unsigned int k = 0; k < map->size().z; k++)
            {
                double v = map->getValue(i, j, k);
                if (v > low && v <= high) m->setValue(i, j, k, v);
            }

    return m;
}
