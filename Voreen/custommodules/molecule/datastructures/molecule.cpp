#include "molecule.h"

Molecule::Molecule() {
    mol_ = new OBMol();
}

Molecule::Molecule(OBMol* mol)
  : mol_(mol)
{}

Molecule* Molecule::clone() const {
    //TODO Should create a copy of underlying OBMol structure
    return new Molecule(mol_);
}

const OBMol* Molecule::getOBMol() const {
    return mol_;
}
