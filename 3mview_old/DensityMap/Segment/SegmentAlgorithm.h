#ifndef SEGMENTALGORITHM_H
#define SEGMENTALGORITHM_H

#include "DensityMap/DensityMap.h"
#include <vector>
using namespace std;

/*
  Базовый класс для алгоритмов сегментирования
  карт электронной плотности
*/

class SegmentAlgorithm
{
public:
    SegmentAlgorithm() {}
    virtual ~SegmentAlgorithm() {}
    virtual vector<DensityMap*> apply(DensityMap* map) = 0;
};

#endif // SEGMENTALGORITHM_H
