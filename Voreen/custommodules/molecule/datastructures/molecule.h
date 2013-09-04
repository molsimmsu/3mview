#ifndef VRN_MOLECULE_H
#define VRN_MOLECULE_H

#include "voreen/core/utils/observer.h"
#include "voreen/core/datastructures/volume/volume.h"

#include "atom.h"
#include "bond.h"
#include "residue.h"

#include "tgt/vector.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

namespace voreen {
  
typedef VolumeURL MoleculeURL;
typedef std::vector< std::vector<char> > SecStructure;

class Molecule;

class MoleculeRep {
public:
    MoleculeRep() {}
    ~MoleculeRep() {}
    
    virtual std::string getName() = 0;
};

class BallsAndSticksRep : public MoleculeRep {
public:
    BallsAndSticksRep(
        float atomsRadius = 0.40f,
        float bondsRadius = 0.25f,
        size_t atomsResolution = 5,
        size_t bondsResolution = 5,
        bool atomsVisible = true,
        bool bondsVisible = true
    )
    : atomsRadius_(atomsRadius)
    , bondsRadius_(bondsRadius)
    , atomsResolution_(atomsResolution)
    , bondsResolution_(bondsResolution)
    , atomsVisible_(atomsVisible)
    , bondsVisible_(bondsVisible)
    {  }
    
    std::string getName() { return std::string("BallsAndSticks"); }

    float atomsRadius() { return atomsRadius_; }
    float bondsRadius() { return bondsRadius_; }
    size_t atomsResolution() { return atomsResolution_; }
    size_t bondsResolution() { return bondsResolution_; }
    bool atomsVisible() { return atomsVisible_; }
    bool bondsVisible() { return bondsVisible_; }
    
    void atomsRadius(float value) { atomsRadius_ = value; }
    void bondsRadius(float value) { bondsRadius_ = value; }
    void atomsResolution(size_t value) { atomsResolution_ = value; }
    void bondsResolution(size_t value) { bondsResolution_ = value; }
    void atomsVisible(bool value) { atomsVisible_ = value; }
    void bondsVisible(bool value) { bondsVisible_ = value; }

private:
    float atomsRadius_;
    float bondsRadius_;
    size_t atomsResolution_;
    size_t bondsResolution_;
    bool atomsVisible_;
    bool bondsVisible_;
};

class RibbonsRep : public MoleculeRep {
public:
    RibbonsRep(
        float tangentLength = 1.5f,
        float traceRadius = 0.1f,
        size_t cylinderResolution = 5,
        size_t splineResolution = 4
    )
    : tangentLength_(tangentLength)
    , traceRadius_(traceRadius)
    , cylinderResolution_(cylinderResolution)
    , splineResolution_(splineResolution)
    {  }
    
    std::string getName() { return std::string("Ribbons"); }

    float tangentLength() { return tangentLength_; }
    float traceRadius() { return traceRadius_; }
    size_t cylinderResolution() { return cylinderResolution_; }
    size_t splineResolution() { return splineResolution_; }
    
    void tangentLength(float value) { tangentLength_ = value; }
    void traceRadius(float value) { traceRadius_ = value; }
    void cylinderResolution(size_t value) { cylinderResolution_ = value; }
    void splineResolution(size_t value) { splineResolution_ = value; }

private:
    float tangentLength_;
    float traceRadius_;
    size_t cylinderResolution_;
    size_t splineResolution_;
};

/**
 * Interface for molecule handle observers.
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
    
    virtual void moleculeTransform(const Molecule* source, const tgt::mat4& matrix) = 0;
};

//-------------------------------------------------------------------------------------------------

class Molecule : public Observable<MoleculeObserver> {
public:
    enum ResidueType {
        AMINO = 0, AMINO_NUCLEO = 1, COENZYME = 2, ION = 3, 
        NUCLEO = 4, PROTEIN = 5, PURINE = 6, PYRIMIDINE = 7, 
        SOLVENT = 8, WATER = 9 
    };

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
    
    
    void DeleteHydrogens();
    
    void CalcCenterOfMass();
    
    const tgt::vec3 getCenterOfMass();
    
    size_t numAtoms() const;
    Atom* atom(size_t i) const;
    
    size_t numBonds() const;
    Bond* bond(size_t i) const;
    
    size_t numResidues() const;
    MoleculeResidue* residue(size_t i) const;
    
    const OBMol& getOBMol() const;
    
    /**
     *  Delete residues of restype
     *  or delete all residues exept restype if invert=true
     *  AMINO = 0, AMINO_NUCLEO = 1, COENZYME = 2, ION = 3, 
     *  NUCLEO = 4, PROTEIN = 5, PURINE = 6, PYRIMIDINE = 7, 
     *  SOLVENT = 8, WATER = 9 
     *  
     */
    
    void clearResidues(int restype, bool invert=false);
    
    void addMolecule(Molecule * mol);
    
    /**
     * Returns a sequence of the molecule in FASTA format
     */
    std::vector<std::string> getSequence() const;
    
    /**
     * Returns secondary structure of a given residue
     */
    char getSecondaryStructure(size_t chainNum, size_t residueNum) const;
    
    /**
     * Returns transformation matrix for offset and rotation
     */
     const tgt::mat4& getTransformationMatrix() const;
     
    /**
     * Transform molecule and notify its observers
     */
     void transform(const tgt::mat4& matrix);
     
     /**
     * Update molecular coordinates, sets identity transformation matrix
     * notify its observers
     */
     void updateCoordinates();
     
    /**
     * Transform molecule and notify its observers (uses transform() function)
     */
     void setTransformationMatrix(const tgt::mat4& transformationMatrix);
     
    /**
     * Returns the origin the volume has been loaded from,
     * usually a file path.
     */
    const VolumeURL& getOrigin() const;

    /// @overload
    VolumeURL& getOrigin();

    /**
     * Sets the origin the volume has been loaded from,
     * usually a file path.
     */
    void setOrigin(const VolumeURL& origin);

    /**
     * Notifies the registered VolumeHandleObservers about the pending
     * deletion of the Volume.
     */
    void notifyDelete();

    /**
     * Notifies the registered VolumeHandleObservers that a reload
     * of the volume was done.
     */
    void notifyReload();
    
    MoleculeRep* getRepresentation() const { return rep_; }
    
    void setRepresentation(MoleculeRep* rep) {
        if (rep_) delete rep_;
        rep_ = rep;
        notifyReload();
    }
     
    /**
     * Tells if the transformation matrix has changed
     */
     void notifyTransformationChange(const tgt::mat4& matrix);
     
     static std::string getAminoFasta(std::string name);
    
private:
    OBMol mol_;  ///< OpenBabel molecule data structure
    MoleculeRep* rep_;
    VolumeURL origin_;
    SecStructure secStructure_;
    tgt::mat4 transformationMatrix_;
    tgt::vec3 MassCent_;
    
    Atom* createAtomFromOBAtom(OBAtom* a) const;
    
    static const std::string loggerCat_;
};

} // namespace voreen

#endif // VRN_MOLECULE_H
