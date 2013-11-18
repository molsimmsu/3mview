#include "moleculecollectiongeometrybuilder.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

#include "../../geometry/utils/primitivegeometrybuilder.h"
#include "../../formcharacteristics/datastructures/pointcloud.h"

#include "tgt/vector.h"
using tgt::vec3;

#include <iostream>

#define CGAL_DISABLE_ROUNDING_MATH_CHECK
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/make_skin_surface_mesh_3.h>
#include <list>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Bare_point;
typedef CGAL::Weighted_point<Bare_point,K::RT> Weighted_point;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef Polyhedron::Facet_iterator Facet_iterator;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;

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
    else if (rep->getName() == "Surface") molGeom = buildSurfaceGeometry(mol);
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

MoleculeGeometry* MoleculeCollectionGeometryBuilder::buildAtomsAndBondsGeometry(const Molecule* molecule) {
    tgtAssert(molecule, "molecule parameter is NULL at MoleculeCollectionGeometryBuilder::buildAtomsAndBondsGeometry()");
    
    MoleculeGeometry* moleculeGeometry = new MoleculeGeometry(molecule);
    
    BallsAndSticksRep* rep = dynamic_cast<BallsAndSticksRep*>(molecule->getRepresentation());
    
    float radius = rep->atomsRadius();
    float bondRadius = rep->bondsRadius();
    size_t steps = rep->atomsResolution();
    size_t bondSteps = rep->bondsResolution();
    
    for (size_t i = 0; i < molecule->numAtoms(); i++) {
        Atom* a = molecule->atom(i);
        tgt::vec3 atomCoords(a->x(), a->y(), a->z());
        tgt::vec3 acolor = getAtomColor(a->atomicNumber());
        
        for(int i=0; i < steps; i++) {
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
    for (size_t i = 0; i < molecule->numBonds(); i++) {
        Bond* bond = molecule->bond(i);
        Atom* a1 = bond->atom1();
        Atom* a2 = bond->atom2();
        tgt::vec3 atom1Coords(a1->x(), a1->y(), a1->z());
        tgt::vec3 atomMidCoords(a1->x() + (a2->x() - a1->x())/2, a1->y() + (a2->y() - a1->y())/2, a1->z() + (a2->z() - a1->z())/2);
        tgt::vec3 atom2Coords(a2->x(), a2->y(), a2->z());
        
        MeshGeometry cyl1 = PrimitiveGeometryBuilder::createCylinder(atom1Coords, atomMidCoords, bondRadius, bondSteps, getAtomColor(a1->atomicNumber()));
        MeshGeometry cyl2 = PrimitiveGeometryBuilder::createCylinder(atom2Coords, atomMidCoords, bondRadius, bondSteps, getAtomColor(a2->atomicNumber()));
        moleculeGeometry->addMesh(cyl1);
        moleculeGeometry->addMesh(cyl2);
    }
    
    return moleculeGeometry;
}

MoleculeGeometry* MoleculeCollectionGeometryBuilder::buildSurfaceGeometry(const Molecule* molecule) {
    tgtAssert(molecule, "molecule parameter is NULL at MoleculeCollectionGeometryBuilder::buildAtomsAndBondsGeometry()");
    
    MoleculeGeometry* moleculeGeometry = new MoleculeGeometry(molecule);
    
    SurfaceRep* rep = dynamic_cast<SurfaceRep*>(molecule->getRepresentation());
    
    float radius = rep->atomsRadius();
    float bondRadius = rep->bondsRadius();
    size_t steps = rep->atomsResolution();
    size_t bondSteps = rep->bondsResolution();
    
	PointCloud cloud;
	cloud.MoleculeFill(molecule);
    
    std::list<Weighted_point> l;
    double shrinkfactor = 0.5;
    
    for (size_t i = 0; i < cloud.entries_num; i++) {
        tgt::vec3 point = cloud.points[i];
        l.push_front(Weighted_point(Bare_point(point[0],point[1], point[2]), radius));
    }
    
    Polyhedron P;
    CGAL::make_skin_surface_mesh_3(P, l.begin(), l.end(), shrinkfactor);

    for ( Facet_iterator i = P.facets_begin(); i != P.facets_end(); ++i) {
        Halfedge_facet_circulator j = i->facet_begin();
        // Facets in polyhedral surfaces are at least triangles.
        CGAL_assertion( CGAL::circulator_size(j) >= 3);
        
        std::cout << CGAL::circulator_size(j) << ' ';
                
        std::vector<tgt::vec3> vertices;
        
        do {
            Polyhedron::Point_3 p = j->vertex()->point();
            vertices.push_back(tgt::vec3(p[0],p[1],p[2]));
        } while ( ++j != i->facet_begin());
        std::cout << std::endl;
        
        
        FaceGeometry face;
        tgt::vec4 color4(0.7, 0.7, 0.7, 1.0);
        tgt::vec3 faceNormal = tgt::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
        VertexGeometry fv1(vertices[0], tgt::vec3(0.f), color4, faceNormal);
        VertexGeometry fv2(vertices[1], tgt::vec3(0.f), color4, faceNormal);
        VertexGeometry fv3(vertices[2], tgt::vec3(0.f), color4, faceNormal);
        face.addVertex(fv1);
        face.addVertex(fv2);
        face.addVertex(fv3);
        
        moleculeGeometry->addFace(face);

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
    
    size_t numResidues = molecule->numResidues();
    if (molecule->numResidues() < 2) return moleculeGeometry;
    
    size_t currentChainNum = 0;
    
	// Read backbone of each chain to a separate PolyLine
    for (size_t i = 0; i < numResidues; i++) {
        
        //OBResidue* res = mol.GetResidue(i);
        MoleculeResidue* res = molecule->residue(i);
        
        const std::vector<Atom*> atoms = res->atoms();
        const std::vector<std::string> atomIDs = res->atomIDs();
        size_t residueChainNum = res->chainNum();
        
        if (residueChainNum > currentChainNum) {
            backbone.push_back(PolyLine());
            currentChainNum = residueChainNum;
        }
        
        size_t numAtoms = atoms.size();
        
        for (size_t i = 0; i < numAtoms; i++) {
            Atom* a = atoms[i];
            std::string atomID = atomIDs[i];
            
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

