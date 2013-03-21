#include "molecule.h"

namespace voreen {
  
const std::string Molecule::loggerCat_ = "voreen.Molecule";

Molecule::Molecule() {
    transformationMatrix_ = tgt::mat4::createIdentity();
}

Molecule::Molecule(const OBMol& mol)
  : mol_(mol)
{
    transformationMatrix_ = tgt::mat4::createIdentity();
}

Molecule::Molecule(const OBMol& mol, const SecStructure& secStructure)
  : mol_(mol)
  , secStructure_(secStructure)
{}

Molecule* Molecule::clone() const {
    return new Molecule(mol_);
}

const OBMol& Molecule::getOBMol() const {
    return mol_;
}

char Molecule::getSecondaryStructure(size_t chainNum, size_t residueNum) const {
	return secStructure_[chainNum][residueNum];
}

const tgt::mat4& Molecule::getTransformationMatrix() const {
    return transformationMatrix_;
}
     
void Molecule::transform(const tgt::mat4& matrix) {
    transformationMatrix_ = matrix * transformationMatrix_;
    notifyTransformationChange(matrix);
}

const VolumeURL& Molecule::getOrigin() const {
    return origin_;
}

VolumeURL& Molecule::getOrigin() {
    return origin_;
}

void Molecule::setOrigin(const VolumeURL& origin) {
    origin_ = origin;
}

void Molecule::notifyDelete() {
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeDelete(this);
}

void Molecule::notifyReload() {
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeChange(this);
}

void Molecule::notifyTransformationChange(const tgt::mat4& matrix) {
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i) {
        observers[i]->moleculeTransform(this, matrix);
    }
}

} // namespace voreen
