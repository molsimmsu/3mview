#ifndef SCENE_H
#define SCENE_H

#include "DensityMap/DensityMap.h"
#include "Molecule/Molecule.h"

class ObjectScene
{
public:
    ObjectScene();
    void add(DensityMap* map);
    void add(Molecule* mol);
};

#endif // SCENE_H
