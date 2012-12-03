#include "ThresholdDivision.h"

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
    //DensityMap* newMap = new DensityMap("")
}
