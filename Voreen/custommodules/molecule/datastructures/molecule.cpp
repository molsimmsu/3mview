#include "molecule.h"

#include <sstream>

Molecule::Molecule() {

}

Molecule* Molecule::clone() const {
    Molecule* mol = new Molecule();
    
    size_t atomCount = getAtomCount();
    
    for (size_t i = 0; i < atomCount; i++)
        mol->addAtom(getAtomCoords(i));
    
    return mol;
}

void Molecule::addAtom(const tgt::vec3& coords) {
    atomsCoords_.push_back(coords);
}

size_t Molecule::getAtomCount() const {
    return atomsCoords_.size();
}

tgt::vec3 Molecule::getAtomCoords(size_t index) const {
    return atomsCoords_[index];
}

void Molecule::setAtomCoords(size_t index, const tgt::vec3& coords) {
    atomsCoords_[index] = coords;
}

void Molecule::serialize(XmlSerializer& s) const {
    size_t atomCount = getAtomCount();
    
    s.serialize("atomCount", atomCount);
    
    const std::string& keyBase("atom");
    
    for (size_t i = 0; i < atomCount; i++) {
        std::stringstream key;
        key << keyBase << i;
        s.serialize(key.str(), getAtomCoords(i));
    }
}

void Molecule::deserialize(XmlDeserializer& s) {
    size_t atomCount;
    
    s.deserialize("atomCount", atomCount);
    
    const std::string& keyBase("atom");
    
    for (size_t i = 0; i < atomCount; i++) {
        std::stringstream key;
        tgt::vec3 atomCoords;
        key << keyBase << i;
        s.deserialize(key.str(), atomCoords);
        setAtomCoords(i, atomCoords);
    }
}
