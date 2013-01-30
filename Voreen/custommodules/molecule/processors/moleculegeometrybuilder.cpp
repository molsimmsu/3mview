#include "moleculegeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

#include "../../geometry/utils/primitivegeometrybuilder.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

MoleculeGeometryBuilder::MoleculeGeometryBuilder()
  : Processor()
  , inport_(Port::INPORT, "molecule", "Molecule Input")
  , outport_(Port::OUTPORT, "geometry", "Geometry Output")
  , repType_("repType", "Representation")
{
    addPort(inport_);
    addPort(outport_);
    
    addProperty(repType_);
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
    // 1. Первичная обработка. На выходе: координаты и матрицы вращения вершин сплайнов, типы вторичных структур и цвета сегментов.
    // 2. Построение геометрии на основе указанных данных.
}
