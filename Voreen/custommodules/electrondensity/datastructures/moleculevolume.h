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
    
    MoleculeVolume(VolumeRepresentation* const volume, const VolumeBase* vh)
      : Volume(volume, vh)
      , molecule_(0)
    {
        transFunc_ = new TransFunc1DKeys();
    }
    
    Molecule* getMolecule() { return molecule_; }
    
    // XXX This function does not clone the molecule pointer
    // because we don't want two different volumes to transform
    // the same molecule
    Volume* clone() const throw (std::bad_alloc) {
        VolumeRAM* v = getRepresentation<VolumeRAM>()->clone();
        MoleculeVolume* mv = new MoleculeVolume(v, this);
        mv->setTransFunc(getTransFunc()->clone());
        return mv;
    }
    
    virtual void setPhysicalToWorldMatrix(const tgt::mat4& transformationMatrix) {
        if (molecule_)
            molecule_->setTransformationMatrix(transformationMatrix);
        
        Volume::setPhysicalToWorldMatrix(transformationMatrix);
    }
    
    void setMolecule(Molecule* mol) {
        molecule_ = mol;
    }
    
    TransFunc* getTransFunc() const {
        return transFunc_;
    }
    
    void setTransFunc(TransFunc* tf) {
        transFunc_ = tf;
    }
    
private:
    Molecule* molecule_;
    TransFunc* transFunc_;
};

#endif
