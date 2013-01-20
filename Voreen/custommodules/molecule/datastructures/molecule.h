#ifndef VRN_MOLECULE_H
#define VRN_MOLECULE_H

#include "voreen/core/io/serialization/serializable.h"
#include "tgt/vector.h"

#include <vector>
#include <string>

using namespace voreen;

class Molecule : public Serializable {
public:
    Molecule();
    virtual ~Molecule() {}
    
    /**
     * Returns the name of this class as a string.
     * Necessary due to the lack of code reflection in C++.
     *
     * This method is expected to be re-implemented by each concrete subclass.
     */
    virtual std::string getClassName() const { return "Molecule"; };

    /**
     * Returns a copy of the molecule object.
     */
    virtual Molecule* clone() const;
    
    /**
     * Adds atom to the molecule
     * @param coords coordinates of the atom
     */
     void addAtom(const tgt::vec3& coords);
     
    /**
     * Returns the number of atoms this molecule.
     *
     * @returns atom count
     */
    size_t getAtomCount() const;
     
    /**
     * Returns the atom coordinates of the given atom
     * @param index atom's index in the @c atomsCoords_ array
     * @returns atom coordinates
     */
     tgt::vec3 getAtomCoords(size_t index) const;
     
    /**
     * Modifies coordinates of the given atom
     * @param index atom's index in the @c atomsCoords_ array
     * @param coords coordinates of the atom
     */
     void setAtomCoords(size_t index, const tgt::vec3& coords);
    
    /**
     * Implementation of the Serializable interface.
     */
    virtual void serialize(XmlSerializer& s) const;

    /**
     * Implementation of the Serializable interface.
     */
    virtual void deserialize(XmlDeserializer& s);
    
private:
    std::vector<tgt::vec3> atomsCoords_; ///< vector of atoms coordinates
};

#endif // VRN_MOLECULE_H
