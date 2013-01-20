#include "moleculegeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

#include "openbabel/obmol.h"
using namespace OpenBabel;

MoleculeGeometryBuilder::MoleculeGeometryBuilder()
  : Processor()
  , inport_(Port::INPORT, "molecule", "Molecule Input")
  , outport_(Port::OUTPORT, "geometry", "Geometry Output")
{
    addPort(inport_);
    addPort(outport_);
}

void MoleculeGeometryBuilder::process() {
    MeshListGeometry* geometry = new MeshListGeometry();
    
    const Molecule* molecule = inport_.getData();
    OBMol* mol = molecule.getOBMol();
    
    // Cubes parameters
    tgt::vec3 diag(0.1f, 0.1f, 0.1f);
    tgt::vec3 tex1(0.f, 0.f, 0.f);
    tgt::vec3 tex2(1.f, 1.f, 1.f);
    tgt::vec3 color(0.f, 1.f, 1.f);
    
    // Draw atoms with cubes.
    // NOTE: Atoms indices in OpenBabel start with 1
    for (size_t i = 1; i <= mol->NumAtoms(); i++) {
        OBAtom* a = mol->GetAtom(i);
        tgt::vec3 atomCoords(a->x(), a->y(), a->z());
        
        const MeshGeometry cube = MeshGeometry::createCube(atomCoords - diag, atomCoords + diag,
                                                           tex1, tex2, color, color);
        
        geometry->addMesh(cube);
    }
    
    // Draw bonds with cylinders
    // NOTE: Bonds indices in OpenBabel start with 0
    for (size_t i = 0; i < mol->NumBonds(); i++) {
        OBBond* a = mol->GetBond(i);
    }
    
    outport_.setData(geometry);
}
