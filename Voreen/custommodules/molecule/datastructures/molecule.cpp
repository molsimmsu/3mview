#include "molecule.h"
using namespace voreen;

const std::string Molecule::loggerCat_ = "voreen.Molecule";

Molecule::Molecule() {
    
}

Molecule::Molecule(const OBMol& mol)
  : mol_(mol)
{}

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
     
void Molecule::setTransformationMatrix(const tgt::mat4& matrix) {
    transformationMatrix_ = matrix;
    notifyTransformationChange();
}

void Molecule::notifyTransformationChange() {
    LINFO("Molecule::notifyTransformationChange()");
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeTransform(this);
}
