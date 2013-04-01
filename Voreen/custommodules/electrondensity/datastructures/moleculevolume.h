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
    
    virtual void setPhysicalToWorldMatrix(const tgt::mat4& transformationMatrix) {
        tgt::mat4 oldMatrix = getPhysicalToWorldMatrix();
        tgt::mat4 invertMatrix;
        oldMatrix.invert(invertMatrix);
        tgt::mat4 molTransform = transformationMatrix * invertMatrix;
        
        molecule_->transform(molTransform);
        
        Volume::setPhysicalToWorldMatrix(transformationMatrix);
    }
    
private:
    Molecule* molecule_;
};

#endif
