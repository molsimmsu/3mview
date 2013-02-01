#include "molecule.h"

Molecule::Molecule() {
    mol_ = new OBMol();
}

Molecule::Molecule(OBMol* mol)
  : mol_(mol)
{}

Molecule::Molecule(OBMol* mol, const SecStructure& secStructure)
  : mol_(mol)
  , secStructure_(secStructure)
{}

Molecule* Molecule::clone() const {
    //TODO Should create a copy of underlying OBMol structure
    return new Molecule(mol_);
}

OBMol* Molecule::getOBMol() const {
    return mol_;
}

char Molecule::getSecondaryStructure(size_t chainNum, size_t residueNum)
{
	return secStructure_[chainNum][residueNum];
}
