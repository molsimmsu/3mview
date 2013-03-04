#ifndef VRN_MOLECULECOLLECTIONGEOMETRYBUILDER_H
#define VRN_MOLECULECOLLECTIONGEOMETRYBUILDER_H

#include "../ports/moleculecollectionport.h"

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/geometryport.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/datastructures/geometry/meshgeometry.h"
#include "voreen/core/datastructures/geometry/facegeometry.h"
using namespace voreen;

class MoleculeCollectionGeometryBuilder : public Processor, public MoleculeCollectionObserver {
public:
    MoleculeCollectionGeometryBuilder();

    // virtual constructor
    virtual Processor* create() const { return new MoleculeCollectionGeometryBuilder(); }

    // documentary functions
    virtual std::string getClassName() const { return "MoleculeCollectionGeometryBuilder"; }
    virtual std::string getCategory() const  { return "Geometry"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void invalidate(int inv = INVALID_RESULT);

protected:
    virtual void setDescriptions() {
        setDescription("Build mesh geometry of a given molecule");
    }

    /**
     * Build the geometry of molecules.
     *
     * TODO Check for memory leaks and unnecessary rebuildung of geometry.
     * TODO Add @repType as a parameter to the Molecule class
     */
    virtual void process();
    
    virtual void moleculeAdded(const MoleculeCollection* /*source*/, const Molecule* /*handle*/);
    virtual void moleculeRemoved(const MoleculeCollection* /*source*/, const Molecule* /*handle*/);
    
    /**
     * This function is called whenever the molecule needs to be rebuilt, 
     * i.e. if the input data or representation params have changed
     */
    void rebuildMolecule();
    
    /**
     * Geometry building methods for different representation types.
	 * These functions append meshes to the given MeshListGeometry
     *
     * @param geometry Geometry to which the crated geometry is appended
     * @param molecule Molecule which should be constructed
     */
    void buildAtomsAndBondsGeometry(MeshListGeometry* geometry, const Molecule* molecule);
    void buildBackboneTraceGeometry(MeshListGeometry* geometry, const Molecule* molecule);

private:
    // ports and properties
    MoleculeCollectionPort inport_;
    GeometryPort outport_;
    
    StringOptionProperty repType_; ///< Determines the type of representation of the molecule
    FloatProperty traceTangentLength_;
    FloatProperty traceCylinderRadius_;
    IntProperty traceNumCylinderSides_;
    IntProperty traceNumSteps_;
    BoolProperty showCoords_;

};

#endif // VRN_MOLECULECOLLECTIONGEOMETRYBUILDER_H
