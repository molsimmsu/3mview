#include "molecule.h"
using namespace voreen;

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
    transformationMatrix_ *= matrix;
    notifyTransformationChange(matrix);
}

void Molecule::notifyTransformationChange(const tgt::mat4& matrix) {
    LINFO("Molecule::notifyTransformationChange()");
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i) {
        if (observers[i] == 0) {
            LERROR("Observer is 0 at Molecule::notifyTransformationChange()");
            continue;
        }
        observers[i]->moleculeTransform(this, matrix);
    }
}
