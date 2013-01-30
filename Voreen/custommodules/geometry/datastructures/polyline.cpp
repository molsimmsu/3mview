#include "polyline.h"

Polyline::Polyline() {
    mol_ = new OBMol();
}

Polyline::Polyline(OBMol* mol)
  : mol_(mol)
{}

Polyline* Polyline::clone() const {
    //TODO Should create a copy of underlying OBMol structure
    return new Polyline(mol_);
}


