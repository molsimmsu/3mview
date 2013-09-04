#ifndef VRN_BOND_H
#define VRN_BOND_H

#include "atom.h"

class Bond
{
private:
    Atom* atom1_;
    Atom* atom2_;

public:

Bond(Atom* atom1, Atom* atom2)
{
    atom1_ = atom1;
    atom2_ = atom2;
}

Atom* atom1() const {
    return atom1_;
}

Atom* atom2() const {
    return atom2_;
}

};

#endif // VRN_BOND_H
