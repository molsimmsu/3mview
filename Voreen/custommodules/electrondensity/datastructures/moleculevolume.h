#ifndef VRN_MOLECULEVOLUME_H
#define VRN_MOLECULEVOLUME_H

#include "../../molecule/datastructures/molecule.h"
#include "voreen/core/datastructures/volume/volume.h"
using namespace voreen;

class MoleculeVolume : public Volume {
public:
    MoleculeVolume(Molecule* molecule, VolumeRepresentation* const volume, const tgt::vec3& spacing, const tgt::vec3& offset, const tgt::mat4& transformation = tgt::mat4::identity)
      : Volume(volume, spacing, offset, transformation)
      , molecule_(molecule)
    { }
    
    Molecule* getMolecule() { return molecule_; }
    
private:
    Molecule* molecule_;
};

#endif
