#include "moleculegeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

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
    
    tgt::vec3 diag(0.1f, 0.1f, 0.1f);
    tgt::vec3 tex1(0.f, 0.f, 0.f);
    tgt::vec3 tex2(1.f, 1.f, 1.f);
    tgt::vec3 color(0.f, 1.f, 1.f);
    
    for (size_t i = 0; i < molecule->getAtomCount(); i++) {
        tgt::vec3 atomCoords = molecule->getAtomCoords(i);
        
        const MeshGeometry cube = MeshGeometry::createCube(atomCoords - diag, atomCoords + diag,
                                                           tex1, tex2, color, color);
        
        geometry->addMesh(cube);
      }
    
    
    outport_.setData(geometry);
}
