#include "moleculegeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

#include "../../geometry/utils/primitivegeometrybuilder.h"
#include "../../geometry/utils/primitivegeometrybuilder.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

#include "tgt/vector.h"

#include <iostream>

#define REBUILD_MOLECULE_ACTION CallMemberAction<MoleculeGeometryBuilder>(this, &MoleculeGeometryBuilder::rebuildMolecule)

MoleculeGeometryBuilder::MoleculeGeometryBuilder()
  : Processor()
  , inport_(Port::INPORT, "molecule", "Molecule Input")
  , outport_(Port::OUTPORT, "geometry", "Geometry Output")
  , repType_("repType", "Representation")
  , traceTangentLength_("traceTangentLength", "Tangent length", 1.5f, 1.f, 2.f)
  , traceCylinderRadius_("traceCylinderRadius", "Trace radius", 0.1f, 0.01f, 0.3f)
  , traceNumCylinderSides_("traceNumCylinderSides", "Cylinder side count", 5, 2, 12)
  , traceNumSteps_("traceNumSteps", "Step count", 4, 1, 12)
  , showCoords_("showCoords", "Show coords", false)
  , atomRadius_("atomRadius", " Atoms radius", 0.1f, 0.01f, 1.0f)
  , bondRadius_("bondsRadius", " Bond radius", 0.1f, 0.01f, 1.0f)
  , bondAndAtomRes_("bondsCylinderSides", " Atoms and bonds resolution", 3, 5, 8)
  
{
    addPort(inport_);
    addPort(outport_);
    
    addProperty(repType_);
    addProperty(traceTangentLength_);
    addProperty(traceCylinderRadius_);
    addProperty(traceNumCylinderSides_);
    addProperty(traceNumSteps_);
    addProperty(showCoords_);
    addProperty(bondAndAtomRes_);
    addProperty(atomRadius_);
    addProperty(bondRadius_);
    
    
    
    repType_.addOption("atomsAndBonds", "Atoms and bonds");
    repType_.addOption("backboneTrace", "Backbone trace");
    
    repType_.onChange(REBUILD_MOLECULE_ACTION);
    traceTangentLength_.onChange(REBUILD_MOLECULE_ACTION);
    traceCylinderRadius_.onChange(REBUILD_MOLECULE_ACTION);
    traceNumCylinderSides_.onChange(REBUILD_MOLECULE_ACTION);
    traceNumSteps_.onChange(REBUILD_MOLECULE_ACTION);
    showCoords_.onChange(REBUILD_MOLECULE_ACTION);
    bondRadius_.onChange(REBUILD_MOLECULE_ACTION);
    atomRadius_.onChange(REBUILD_MOLECULE_ACTION);
    bondAndAtomRes_.onChange(REBUILD_MOLECULE_ACTION);
	
	// Create empty data to make this outport valid. Take ownership is true because
	// we want the data to be automatically deleted when replaced at the next setData() call
	outport_.setData(new MeshListGeometry(), true);
}

void MoleculeGeometryBuilder::process() {
	// TODO Check for memory leaks and unnecessary rebuildung of geometry

	// We don't check if this port is valid because we assume that the source
	// always contains molecule data structure (even if it does not contain any data)

	// If inport is not connected or nothing changed since the last call then do nothing
	if (!inport_.isReady() || !inport_.hasChanged()) return;
	
	// Port is ready and input data has changed
    rebuildMolecule();
}

void MoleculeGeometryBuilder::rebuildMolecule() {
	try {
	    const Molecule* mol = inport_.getData();
	    if (!mol) return;
	    
        MeshListGeometry* geom = new MeshListGeometry();

        // TODO Add @repType as a parameter to the Molecule class
        /**/ if (repType_.get() == "atomsAndBonds")
            buildAtomsAndBondsGeometry(geom, mol);
        else if (repType_.get() == "backboneTrace")
            buildBackboneTraceGeometry(geom, mol);
        
        tgt::mat4 transform = tgt::mat4::createIdentity();
        transform = mol->getTransformationMatrix();
        std::cout << transform << std::endl;
        
        geom->transform(transform);
        // Delete old data and set new
        outport_.setData(geom);
    }
    catch (...) {
        LERROR("Error at MoleculeGeometryBuilder::process()");
    }
}

tgt::vec3 MoleculeGeometryBuilder::getAtomColor(int a){
tgt::vec3 color(1.f, 0.1f, 0.7f);
if(a==1) color= {0.9f, 0.9f, 0.9f};
else if(a==6) color = {0.3f, 0.3f, 0.3f};
else if(a==8) color = {0.8f , 0.f, 0.f};
else if(a==7) color = {0.3f, 0.3f, 1.f}; 
else if(a==15) color = {1.f,0.6f,0.f};
else if(a==16) color = {1.f,0.8f,0.2f}; 
else if(a==17) color = {0.f,0.9f,0.f}; 
else if(a==11) color = {0.f,0.f,0.9f}; 
else if(a==12) color = {0.1f,0.5f,0.1f}; 
else if(a==20) color = {0.5f,0.5f,0.5f}; 

return color;
}

void MoleculeGeometryBuilder::buildAtomsAndBondsGeometry(MeshListGeometry* geometry, const Molecule* molecule) {
    tgtAssert(molecule, "molecule parameter is NULL at MoleculeGeometryBuilder::buildAtomsAndBondsGeometry()");
    const OBMol& mol = molecule->getOBMol();
    
    // Cubes parameters
    tgt::vec3 diag(0.3f, 0.3f, 0.3f);
    tgt::vec3 tex1(0.f, 0.f, 0.f);
    tgt::vec3 tex2(1.f, 1.f, 1.f);
    tgt::vec3 color(0.6f, 0.6f, 0.6f);
    
    // Draw atoms with cubes.
    // XXX Atoms indices in OpenBabel start with 1
    float radius = atomRadius_.get();
    size_t steps = bondAndAtomRes_.get();
    for (size_t i = 1; i <= mol.NumAtoms(); i++) {
        OBAtom* a = mol.GetAtom(i);
        tgt::vec3 atomCoords(a->x(), a->y(), a->z());
        tgt::vec3 acolor = getAtomColor(a->GetAtomicNum());
        
        for(int i=0; i < steps; i++){
            float x1 = 2*i/float(steps) -1;
            float x2 = 2*(i+1)/float(steps) -1;
            float R1 = radius * sqrt(1 - x1*x1);
            float R2 = radius * sqrt(1 - x2*x2);
            tgt::vec3 v1( a->x() + radius*x1, a->y(), a->z());
            tgt::vec3 v2( a->x() + radius*x2, a->y(), a->z());
            
            MeshGeometry cone = PrimitiveGeometryBuilder::createConeCylinder(v1,v2,R1,R2,steps,acolor,true);
            geometry->addMesh(cone);
        }
        //MeshGeometry cube = MeshGeometry::createCube(atomCoords - diag, atomCoords + diag,
        //                                                   tex1, tex2, getAtomColor(a->GetAtomicNum()), getAtomColor(a->GetAtomicNum()));
        
        //geometry->addMesh(cube);
    }
    
    // Draw bonds with cylinders 2 cylinders for 1 bond
    // XXX Bonds indices in OpenBabel start with 0
    for (size_t i = 0; i < mol.NumBonds(); i++) {
        OBBond* bond = mol.GetBond(i);
        OBAtom* a1 = bond->GetBeginAtom();
        OBAtom* a2 = bond->GetEndAtom();
        tgt::vec3 atom1Coords(a1->x(), a1->y(), a1->z());
        tgt::vec3 atomMidCoords(a1->x() + (a2->x() - a1->x())/2, a1->y() + (a2->y() - a1->y())/2, a1->z() + (a2->z() - a1->z())/2);
        tgt::vec3 atom2Coords(a2->x(), a2->y(), a2->z());
        
        MeshGeometry cyl1 = PrimitiveGeometryBuilder::createCylinder(atom1Coords, atomMidCoords, bondRadius_.get(), steps, getAtomColor(a1->GetAtomicNum()));
        MeshGeometry cyl2 = PrimitiveGeometryBuilder::createCylinder(atom2Coords, atomMidCoords, bondRadius_.get(), steps, getAtomColor(a2->GetAtomicNum()));
        geometry->addMesh(cyl1);
        geometry->addMesh(cyl2);
    }
}

void MoleculeGeometryBuilder::buildBackboneTraceGeometry(MeshListGeometry* geometry, const Molecule* molecule) {
    tgtAssert(molecule, "molecule parameter is NULL at MoleculeGeometryBuilder::buildBackboneTraceGeometry()");
    std::vector<PolyLine> backbone;
    
    std::vector<tgt::vec3> chainColors;
    chainColors.push_back(tgt::vec3(1, 0.4, 0.4));
    chainColors.push_back(tgt::vec3(0.4, 1, 0.4));
    chainColors.push_back(tgt::vec3(0.4, 0.4, 1));
    chainColors.push_back(tgt::vec3(0, 1, 1));
    chainColors.push_back(tgt::vec3(1, 0, 1));
    chainColors.push_back(tgt::vec3(1, 1, 0));

    const OBMol& mol = molecule->getOBMol();
 
    if (mol.NumResidues() < 2) return;
    
    
    //OBResidueIterator res = mol.BeginResidues();
    //OBResidueIterator resEnd = mol.EndResidues();
    
    size_t currentChainNum = 0;
    size_t numResidues = mol.NumResidues();
    
	// Read backbone of each chain to a separate PolyLine
    for (size_t i = 0; i < numResidues; i++) {
        
        OBResidue* res = mol.GetResidue(i);
        
        std::vector<OBAtom*> atoms = res->GetAtoms();
        size_t residueChainNum = res->GetChainNum();
        
        if (residueChainNum > currentChainNum) {
            backbone.push_back(PolyLine());
            currentChainNum = residueChainNum;
        }
        
        size_t numAtoms = atoms.size();
        
        for (size_t i = 0; i < numAtoms; i++) {
            OBAtom* a = atoms[i];
            
            std::string atomID = res->GetAtomID(a);
            atomID.erase(remove(atomID.begin(), atomID.end(), ' '), atomID.end());
            
            if (atomID.compare("CA") == 0)
                backbone.back().addVertex(tgt::vec3(a->x(),a->y(),a->z()));
        }
        
    }
    
	// For each backbone PolyLine
    for (size_t i = 0; i < backbone.size(); i++) {
		// Build ribbon trace
		
        PolyLine* smoothBackbone = backbone[i].interpolateBezier(traceNumSteps_.get(), traceTangentLength_.get());
        MeshListGeometry* lineGeometry = PrimitiveGeometryBuilder::createPolyLine(smoothBackbone, traceCylinderRadius_.get(), traceNumCylinderSides_.get(), chainColors[i]);
        
        geometry->addMeshList(*lineGeometry);
		delete lineGeometry;
        
        // Build moving frame coords (just for testing)  
        if (showCoords_.get()) {  
            MeshListGeometry* coordsGeometry = PrimitiveGeometryBuilder::createPolyLineCoords(smoothBackbone, traceCylinderRadius_.get() * 0.5);
            geometry->addMeshList(*coordsGeometry);
			delete coordsGeometry;
        }
    }
}
