#ifndef THRESHOLDDIVISION_H
#define THRESHOLDDIVISION_H

#include "SegmentAlgorithm.h"

class ThresholdDivision : public SegmentAlgorithm
{
public:
    ThresholdDivision(unsigned int numMaps);
    vector<DensityMap*> apply(DensityMap* map);
    DensityMap* createMap(DensityMap* map, double low, double high);

private:
   unsigned int _numMaps;

};

#endif // THRESHOLDDIVISION_H
