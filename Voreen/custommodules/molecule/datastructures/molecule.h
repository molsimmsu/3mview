#ifndef VRN_MOLECULE_H
#define VRN_MOLECULE_H

#include "voreen/core/utils/observer.h"


#include "tgt/vector.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

namespace voreen {

typedef std::vector< std::vector<char> > SecStructure;

class Molecule;

/**
 * Interface for volume handle observers.
 */
class MoleculeObserver : public Observer {
public:
    /**
     * This method is called by the observed Molecule's destructor.
     *
     * @param source the calling Molecule
     */
    virtual void moleculeDelete(const Molecule* source) = 0;

    /**
     * This method is called by the observed Molecule
     * after its member Molecule object has changed.
     *
     * When this function is called, the new Molecule object has
     * already been assigned. The former Molecule object is still
     * valid at this point, but it is deleted immediately after
     * this function has been called.
     *
     * @param source the calling Molecule
     */
    virtual void moleculeChange(const Molecule* source) = 0;
    
    virtual void moleculeTransform(const Molecule* source) = 0;
};

//-------------------------------------------------------------------------------------------------

class Molecule : public Observable<MoleculeObserver> {
public:
    Molecule();
    Molecule(const OBMol& mol);
    Molecule(const OBMol& mol, const SecStructure& secStructure);
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
     * Returns underlying OpenBabel molecule data structure
     */
    const OBMol& getOBMol() const;
    
    /**
     * Returns secondary structure of a given residue
     */
    char getSecondaryStructure(size_t chainNum, size_t residueNum) const;
    
    /**
     * Returns transformation matrix for offset and rotation
     */
     const tgt::mat4& getTransformationMatrix() const;
     
    /**
     * Returns transformation matrix for offset and rotation
     */
     void setTransformationMatrix(const tgt::mat4& matrix);
     
    /**
     * Tells if the transformation matrix has changed
     */
     void notifyTransformationChange();
    
    
private:
    OBMol mol_;  ///< OpenBabel molecule data structure
    SecStructure secStructure_;
    tgt::mat4 transformationMatrix_;
    
    static const std::string loggerCat_;
};

} // namespace voreen
#endif // VRN_MOLECULE_H
