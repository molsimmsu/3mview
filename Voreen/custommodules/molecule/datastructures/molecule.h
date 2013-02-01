#ifndef VRN_MOLECULE_H
#define VRN_MOLECULE_H

#include "voreen/core/io/serialization/serializable.h"
using namespace voreen;

#include "tgt/vector.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

typedef std::vector< std::vector<char> > SecStructure;

class Molecule : public Serializable {
public:
    Molecule();
    Molecule(OBMol* mol);
    Molecule(OBMol* mol, const SecStructure& secStructure);
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
     * TODO Should create a copy of underlying OBMol structure
     */
    virtual Molecule* clone() const;
    
    /**
     * Returns underlying OpenBabel molecule data structure
     */
    OBMol* getOBMol() const;
    
    /**
     * Returns secondary structure of a given residue
     */
    char getSecondaryStructure(size_t chainNum, size_t residueNum);
    
    /**
     * TODO Implementation of the Serializable interface.
     */
    virtual void serialize(XmlSerializer& s) const {}

    /**
     * TODO Implementation of the Serializable interface.
     */
    virtual void deserialize(XmlDeserializer& s) {}
    
private:
    OBMol* mol_;  ///< OpenBabel molecule data structure
    SecStructure secStructure_;
};

#endif // VRN_MOLECULE_H
