#include "molecule.h"

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
