#include "DensityMapSegment.h"

DensityMapSegment::DensityMapSegment(QObject *parent) :
    QObject(parent)
{
}

void DensityMapSegment::send(DensityMap *map)
{
    maps.push_back(map);
    emit(update());
}

unsigned int DensityMapSegment::size()
{
    return maps.size();
}

DensityMap *DensityMapSegment::at(unsigned int i)
{
    return maps.at(i);
}
