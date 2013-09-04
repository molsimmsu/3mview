#ifndef VRN_RESIDUE_H
#define VRN_RESIDUE_H

#include "atom.h"
#include <vector>

class MoleculeResidue
{
private:
    std::vector<Atom*> atoms_;
    std::vector<std::string> atomIDs_;
    size_t chainNum_;

public:

    MoleculeResidue(size_t chainNum) {
        chainNum_ = chainNum;
    }

    void addAtom(Atom* a, std::string id) {
        atoms_.push_back(a);
        atomIDs_.push_back(id);
    }

    const std::vector<Atom*> atoms() const {
        return atoms_;
    }
    
    const std::vector<std::string> atomIDs() const {
        return atomIDs_;
    }
    
    size_t chainNum() const {
        return chainNum_;
    }

};

#endif // VRN_RESIDUE_H
