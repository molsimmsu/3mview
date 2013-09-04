#ifndef MOLECULE_GEOMETRY_H
#define MOLECULE_GEOMETRY_H

#include "molecule.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/datastructures/geometry/meshgeometry.h"
#include "voreen/core/datastructures/geometry/facegeometry.h"
using namespace voreen;

class MoleculeGeometry : public MeshGeometry {
public:
    MoleculeGeometry(const Molecule* molecule)
        : molecule_(molecule)
    {}
    
    const Molecule* getMolecule() const { return molecule_; }
    
    /*
     * Add all faces of a given MeshGeometry to this MoleculeGeometry
     */
    void addMesh(MeshGeometry& mesh) {
        for (size_t i = 0; i < mesh.getFaceCount(); i++) {
            FaceGeometry& face = mesh.getFace(i);
            addFace(face);
            saveFaceVertices(getFace(getFaceCount() - 1));
        }
    }
    
    void addMeshList(MeshListGeometry& meshList) {
        for (size_t i = 0; i < meshList.getMeshCount(); i++) {
            addMesh(meshList.getMesh(i));
        }
    }
    
    void saveFaceVertices(FaceGeometry& face) {
        for (size_t i = 0; i < face.getVertexCount(); i++) {
            vertices_.push_back(&face.getVertex(i));
        }
    }
    
    //void transform(const tgt::mat4& transformation) {
        //for (size_t i = 0; i < vertices_.size(); i++)
           // vertices_[i]->transform(transformation);
    //}
    
private:
    const Molecule* molecule_;
    std::vector<VertexGeometry*> vertices_;
};

#endif
