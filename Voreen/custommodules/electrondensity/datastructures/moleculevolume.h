#ifndef VRN_MOLECULEVOLUME_H
#define VRN_MOLECULEVOLUME_H

#include "../../molecule/datastructures/molecule.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/transfunc/transfunc1dkeys.h"
using namespace voreen;

class MoleculeVolume : public Volume {
public:
    MoleculeVolume(VolumeRepresentation* const volume, const tgt::vec3& spacing, const tgt::vec3& offset, const tgt::mat4& transformation = tgt::mat4::identity)
      : Volume(volume, spacing, offset, transformation)
      , molecule_(0)
    { 
        transFunc_ = new TransFunc1DKeys();
    }
    
    Molecule* getMolecule() { return molecule_; }
    
    virtual void setPhysicalToWorldMatrix(const tgt::mat4& transformationMatrix) {
        molecule_->setTransformationMatrix(transformationMatrix);
        
        Volume::setPhysicalToWorldMatrix(transformationMatrix);
    }
    
    void setMolecule(Molecule* mol) {
        molecule_ = mol;
    }
    
    TransFunc1DKeys* getTransFunc() const {
        return transFunc_;
    }
    
private:
    Molecule* molecule_;
    TransFunc1DKeys* transFunc_;
};

#endif
