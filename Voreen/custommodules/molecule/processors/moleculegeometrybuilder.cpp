#include "moleculegeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

#include "../../geometry/utils/primitivegeometrybuilder.h"
#include "../../geometry/utils/primitivegeometrybuilder.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

#include "tgt/vector.h"

#include <iostream>

MoleculeGeometryBuilder::MoleculeGeometryBuilder()
  : Processor()
  , inport_(Port::INPORT, "molecule", "Molecule Input")
  , outport_(Port::OUTPORT, "geometry", "Geometry Output")
  , repType_("repType", "Representation")
  , traceTangentLength_("traceTangentLength", "Tangent length", 1.5f, 1.f, 2.f)
  , traceCylinderRadius_("traceCylinderRadius", "Trace radius", 0.1f, 0.01f, 0.3f)
  , traceNumCylinderSides_("traceNumCylinderSides", "Cylinder side count", 8, 2, 12)
  , traceNumSteps_("traceNumSteps", "Step count", 8, 1, 12)
{
    addPort(inport_);
    addPort(outport_);
    
    addProperty(repType_);
    addProperty(traceTangentLength_);
    addProperty(traceCylinderRadius_);
    addProperty(traceNumCylinderSides_);
    addProperty(traceNumSteps_);
    
    repType_.addOption("atomsAndBonds", "Atoms and bonds");
    repType_.addOption("backboneTrace", "Backbone trace");
}

void MoleculeGeometryBuilder::process() {
    // TODO Check for memory leaks and unnecessary rebuildung of geometry
    MeshListGeometry* geometry = new MeshListGeometry();
    const Molecule* molecule = inport_.getData();

    // TODO Add @repType as a parameter to the Molecule class
    /**/ if (repType_.get() == "atomsAndBonds")
        buildAtomsAndBondsGeometry(geometry, molecule);
    else if (repType_.get() == "backboneTrace")
        buildBackboneTraceGeometry(geometry, molecule);
    
    outport_.setData(geometry);
}

void MoleculeGeometryBuilder::buildAtomsAndBondsGeometry(MeshListGeometry* geometry, const Molecule* molecule) {
    const OBMol* mol = molecule->getOBMol();
    
    // Cubes parameters
    tgt::vec3 diag(0.1f, 0.1f, 0.1f);
    tgt::vec3 tex1(0.f, 0.f, 0.f);
    tgt::vec3 tex2(1.f, 1.f, 1.f);
    tgt::vec3 color(0.f, 1.f, 1.f);
    
    // Draw atoms with cubes.
    // XXX Atoms indices in OpenBabel start with 1
    for (size_t i = 1; i <= mol->NumAtoms(); i++) {
        OBAtom* a = mol->GetAtom(i);
        tgt::vec3 atomCoords(a->x(), a->y(), a->z());
        
        MeshGeometry cube = MeshGeometry::createCube(atomCoords - diag, atomCoords + diag,
                                                           tex1, tex2, color, color);
        
        geometry->addMesh(cube);
    }
    
    // Draw bonds with cylinders
    // XXX Bonds indices in OpenBabel start with 0
    for (size_t i = 0; i < mol->NumBonds(); i++) {
        OBBond* bond = mol->GetBond(i);
        OBAtom* a1 = bond->GetBeginAtom();
        OBAtom* a2 = bond->GetEndAtom();
        tgt::vec3 atom1Coords(a1->x(), a1->y(), a1->z());
        tgt::vec3 atom2Coords(a2->x(), a2->y(), a2->z());
        
        MeshGeometry cyl = PrimitiveGeometryBuilder::createCylinder(atom1Coords, atom2Coords, 0.02f, 2, color);
        geometry->addMesh(cyl);
    }
}

void MoleculeGeometryBuilder::buildBackboneTraceGeometry(MeshListGeometry* geometry, const Molecule* molecule) {
    // TODO Parse chain numbers
    std::vector<PolyLine> backbone;
    
    OBMol* mol = molecule->getOBMol();
    if (mol->NumResidues() < 2) return;
    
    OBResidueIterator res = mol->BeginResidues();
    OBResidueIterator resEnd = mol->EndResidues();
    
    size_t currentChainNum = 0;
    
    for (;res != resEnd; ++res) {
        std::vector<OBAtom*> atoms = (*res)->GetAtoms();
        size_t residueChainNum = (*res)->GetChainNum();
        if (residueChainNum > currentChainNum) {
            backbone.push_back(PolyLine());
            currentChainNum = residueChainNum;
        }
        size_t numAtoms = atoms.size();
        
        for (size_t i = 0; i < numAtoms; i++) {
            OBAtom* a = atoms[i];
            
            std::string atomID = (*res)->GetAtomID(a);
            atomID.erase(remove(atomID.begin(), atomID.end(), ' '), atomID.end());
            
            if (atomID.compare("CA") == 0)
                backbone.back().addVertex(tgt::vec3(a->x(),a->y(),a->z()));
        }
    }
    for (size_t i = 0; i < backbone.size(); i++) {
        PolyLine* smoothBackbone = backbone[i].interpolateBezier(traceNumSteps_.get(), traceTangentLength_.get());
        MeshListGeometry* lineGeometry = PrimitiveGeometryBuilder::createPolyLine(smoothBackbone, traceCylinderRadius_.get(), traceNumCylinderSides_.get(), tgt::vec3(1, 1, 0));
        
        geometry->addMeshList(*lineGeometry);
        
        // Coords    
        MeshListGeometry* coordsGeometry = PrimitiveGeometryBuilder::createPolyLineCoords(smoothBackbone, traceCylinderRadius_.get() * 0.5);
        
        geometry->addMeshList(*coordsGeometry);
    }
}
