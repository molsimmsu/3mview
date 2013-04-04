#include "moleculecollectiongeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

#include "../../geometry/utils/primitivegeometrybuilder.h"

#include "openbabel/mol.h"
using namespace OpenBabel;

#include "tgt/vector.h"
using tgt::vec3;

#include <iostream>

#define REBUILD_MOLECULE_ACTION CallMemberAction<MoleculeCollectionGeometryBuilder>(this, &MoleculeCollectionGeometryBuilder::rebuildMolecule)

MoleculeCollectionGeometryBuilder::MoleculeCollectionGeometryBuilder()
  : Processor()
  , inport_(Port::INPORT, "molecule", "Molecule Input")
  , outport_(Port::OUTPORT, "geometry", "Geometry Output")
  , repType_("repType", "Representation")
  , traceTangentLength_("traceTangentLength", "Tangent length", 1.5f, 1.f, 2.f)
  , traceCylinderRadius_("traceCylinderRadius", "Trace radius", 0.1f, 0.01f, 0.3f)
  , traceNumCylinderSides_("traceNumCylinderSides", "Cylinder side count", 5, 2, 12)
  , traceNumSteps_("traceNumSteps", "Step count", 4, 1, 12)
  , showCoords_("showCoords", "Show coords", false)
{
    addPort(inport_);
    addPort(outport_);
    
    addProperty(repType_);
    addProperty(traceTangentLength_);
    addProperty(traceCylinderRadius_);
    addProperty(traceNumCylinderSides_);
    addProperty(traceNumSteps_);
    addProperty(showCoords_);
    
    repType_.addOption("atomsAndBonds", "Atoms and bonds");
    repType_.addOption("backboneTrace", "Backbone trace");
    
    repType_.onChange(REBUILD_MOLECULE_ACTION);
    traceTangentLength_.onChange(REBUILD_MOLECULE_ACTION);
    traceCylinderRadius_.onChange(REBUILD_MOLECULE_ACTION);
    traceNumCylinderSides_.onChange(REBUILD_MOLECULE_ACTION);
    traceNumSteps_.onChange(REBUILD_MOLECULE_ACTION);
    showCoords_.onChange(REBUILD_MOLECULE_ACTION);
	
	// Create empty data to make this outport valid. Take ownership is true because
	// we want the data to be automatically deleted when replaced at the next setData() call
	outport_.setData(new GeometryCollection(), true);
}

void MoleculeCollectionGeometryBuilder::invalidate(int inv) {
	//if (inport_.hasChanged()) {

	//}
    Processor::invalidate(inv);
}

void MoleculeCollectionGeometryBuilder::process() {
	// TODO Check for memory leaks and unnecessary rebuildung of geometry

	// We don't check if this port is valid because we assume that the source
	// always contains molecule data structure (even if it does not contain any data)

	// If inport is not connected or nothing changed since the last call then do nothing
	if (!inport_.isReady()) return;
	
	const MoleculeCollection* mc = inport_.getData();
	if (!mc) return;
	if (!observes(mc)) {
        mc->addObserver(this);
        LWARNING("MoleculeCollection Observeer added");
	}
	
	// Port is ready and input data has changed
    rebuildMolecule();
}

GeometryCollection* MoleculeCollectionGeometryBuilder::getOutputGeometry() {
    return outport_.getWritableData();
}

MoleculeGeometry* MoleculeCollectionGeometryBuilder::getMoleculeGeometry(const Molecule* molecule) {
    GeometryCollection* collection = getOutputGeometry();
    
    for (size_t i = 0; i < collection->size(); i++) {
        Geometry* geometry = collection->at(i);
        if (typeid(*geometry) != typeid(MoleculeGeometry)) continue;
        
        MoleculeGeometry* molGeom = dynamic_cast<MoleculeGeometry*>(geometry);
        if (molGeom->getMolecule() == molecule) return molGeom;
    }
        
    LERROR("GEOMETRY IS NULL AT MoleculeCollectionGeometryBuilder::getMoleculeGeometry()");
    return 0;
}

void MoleculeCollectionGeometryBuilder::moleculeAdded(const MoleculeCollection* mc, const Molecule* mol) {
    //LINFO("MoleculeCollectionGeometryBuilder::moleculeAdded()");
    
    createMoleculeGeometry(mol);
    outport_.invalidatePort();
}

void MoleculeCollectionGeometryBuilder::moleculeChanged(const MoleculeCollection* mc, const Molecule* mol) {
    //LINFO("MoleculeCollectionGeometryBuilder::moleculeChanged()");
    
    deleteMoleculeGeometry(mol);
    createMoleculeGeometry(mol);
    outport_.invalidatePort();
}

void MoleculeCollectionGeometryBuilder::moleculeRemoved(const MoleculeCollection* mc, const Molecule* mol) {
    //LINFO("MoleculeCollectionGeometryBuilder::moleculeRemoved()");
    
    deleteMoleculeGeometry(mol);
    outport_.invalidatePort();
}

void MoleculeCollectionGeometryBuilder::moleculeTransformed(const MoleculeCollection* mc, const Molecule* mol, const tgt::mat4& matrix) {
    //LINFO("MoleculeCollectionGeometryBuilder::moleculeTransformed()");
    
    MoleculeGeometry* moleculeGeometry = getMoleculeGeometry(mol);
    moleculeGeometry->transform(matrix);
    
    outport_.invalidatePort();
}

void MoleculeCollectionGeometryBuilder::createMoleculeGeometry(const Molecule* mol) {
    MoleculeGeometry* molGeom;
    MoleculeRep* rep = mol->getRepresentation();
    
          if (rep->getName() == "BallsAndSticks") molGeom = buildAtomsAndBondsGeometry(mol);
    else if (rep->getName() == "Ribbons") molGeom = buildBackboneTraceGeometry(mol);
    else {
        LERROR("Unknown representation name");
        return;
    }
    molGeom->transform(mol->getTransformationMatrix());
    getOutputGeometry()->push_back(molGeom);
}

void MoleculeCollectionGeometryBuilder::deleteMoleculeGeometry(const Molecule* mol) {
    GeometryCollection* geom = getOutputGeometry();
    
    for (size_t i = 0; i < geom->size(); i++) {
        MoleculeGeometry* molGeom = dynamic_cast<MoleculeGeometry*>(geom->at(i));
        tgtAssert(molGeom, "Wrong dynamic cast");
        // TODO Check if there can be more than one instances
        if (molGeom->getMolecule() == mol) {
            getOutputGeometry()->erase(getOutputGeometry()->begin() + i);
            delete molGeom;
        }
    }
}

void MoleculeCollectionGeometryBuilder::rebuildMolecule() {
	/*try {
	    const Molecule* mol = inport_.getData();
	    if (!mol) return;
	    
        MeshListGeometry* geom = new MeshListGeometry();

        // TODO Add @repType as a parameter to the Molecule class
             if (repType_.get() == "atomsAndBonds")
            buildAtomsAndBondsGeometry(geom, mol);
        else if (repType_.get() == "backboneTrace")
            buildBackboneTraceGeometry(geom, mol);
        
        // Delete old data and set new
        outport_.setData(geom);
    }
    catch (...) {
        LERROR("Error at MoleculeCollectionGeometryBuilder::process()");
    }*/
}

MoleculeGeometry* MoleculeCollectionGeometryBuilder::buildAtomsAndBondsGeometry(const Molecule* molecule) {
    tgtAssert(molecule, "molecule parameter is NULL at MoleculeCollectionGeometryBuilder::buildAtomsAndBondsGeometry()");
    const OBMol& mol = molecule->getOBMol();
    
    MoleculeGeometry* moleculeGeometry = new MoleculeGeometry(molecule);
    
    BallsAndSticksRep* rep = dynamic_cast<BallsAndSticksRep*>(molecule->getRepresentation());
    
    float radius = rep->atomsRadius();
    float bondRadius = rep->bondsRadius();
    size_t steps = rep->atomsResolution();
    size_t bondSteps = rep->bondsResolution();
    
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
            moleculeGeometry->addMesh(cone);
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
        
        MeshGeometry cyl1 = PrimitiveGeometryBuilder::createCylinder(atom1Coords, atomMidCoords, bondRadius, bondSteps, getAtomColor(a1->GetAtomicNum()));
        MeshGeometry cyl2 = PrimitiveGeometryBuilder::createCylinder(atom2Coords, atomMidCoords, bondRadius, bondSteps, getAtomColor(a2->GetAtomicNum()));
        moleculeGeometry->addMesh(cyl1);
        moleculeGeometry->addMesh(cyl2);
    }
    
    return moleculeGeometry;
}

MoleculeGeometry* MoleculeCollectionGeometryBuilder::buildBackboneTraceGeometry(const Molecule* molecule) {
    MoleculeGeometry* moleculeGeometry = new MoleculeGeometry(molecule);
  
    tgtAssert(molecule, "molecule parameter is NULL at MoleculeCollectionGeometryBuilder::buildBackboneTraceGeometry()");
    std::vector<PolyLine> backbone;
    
    std::vector<tgt::vec3> chainColors;
    chainColors.push_back(tgt::vec3(1, 0.4, 0.4));
    chainColors.push_back(tgt::vec3(0.4, 1, 0.4));
    chainColors.push_back(tgt::vec3(0.4, 0.4, 1));
    chainColors.push_back(tgt::vec3(0, 1, 1));
    chainColors.push_back(tgt::vec3(1, 0, 1));
    chainColors.push_back(tgt::vec3(1, 1, 0));

    const OBMol& mol = molecule->getOBMol();
 
    if (mol.NumResidues() < 2) return moleculeGeometry;
    
    
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
        
        moleculeGeometry->addMeshList(*lineGeometry);
		delete lineGeometry;
        
        // Build moving frame coords (just for testing)  
        if (showCoords_.get()) {  
            MeshListGeometry* coordsGeometry = PrimitiveGeometryBuilder::createPolyLineCoords(smoothBackbone, traceCylinderRadius_.get() * 0.5);
            moleculeGeometry->addMeshList(*coordsGeometry);
			delete coordsGeometry;
        }
    }
    
    return moleculeGeometry;
}

vec3 MoleculeCollectionGeometryBuilder::getAtomColor(int a) {
    vec3 color(1.f, 0.1f, 0.7f);

         if (a==1)  color = vec3(0.9f, 0.9f, 0.9f);
    else if (a==6)  color = vec3(0.6f, 0.9f, 0.9f);
    else if (a==8)  color = vec3(1.f , 0.1f, 0.1f);
    else if (a==7)  color = vec3(0.2f, 0.2f, 1.f); 
    else if (a==15) color = vec3(1.f,0.6f,0.f);
    else if (a==16) color = vec3(1.f,0.8f,0.2f); 
    else if (a==17) color = vec3(0.f,0.9f,0.f); 
    else if (a==11) color = vec3(0.f,0.f,0.9f); 
    else if (a==12) color = vec3(0.1f,0.5f,0.1f); 
    else if (a==20) color = vec3(0.5f,0.5f,0.5f); 

    return color;
}
