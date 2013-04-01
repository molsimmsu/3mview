#ifndef VRN_PRIMITIVEGEOMETRYBUILDER_H
#define VRN_PRIMITIVEGEOMETRYBUILDER_H

#include "../datastructures/polyline.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/datastructures/geometry/meshgeometry.h"
#include "voreen/core/datastructures/geometry/facegeometry.h"
using namespace voreen;

#include "tgt/vector.h"
#include "tgt/tgt_math.h"

#include <vector>
#include <cmath>

typedef std::vector<tgt::vec3> Basis;

class PrimitiveGeometryBuilder {
public:
    static MeshGeometry createConeCylinder(tgt::vec3 v1, tgt::vec3 v2, float radius1, float radius2, size_t numSides, tgt::vec3 color, bool buildCaps);

    static MeshGeometry createCylinder(tgt::vec3 v1, tgt::vec3 v2, float radius, 
                                size_t numSides, tgt::vec3 color, bool buildCaps = false);
                                
    static MeshListGeometry* createPolyLine(const PolyLine* line, float radius, 
                                size_t numSides, tgt::vec3 color, bool buildCaps = false);
                                
    static MeshListGeometry* createPolyLineCoords(const PolyLine* line, float radius);

private:
    static std::vector<tgt::vec3> makeBasisVertices(tgt::vec3 v1, Basis basis1, float rx1, float ry1, 
                                             tgt::vec3 v2, Basis basis2, float rx2, float ry2,
                                             size_t numSides);
                                             
    static tgt::vec3 makeBasisVertex(float angle, Basis basis, float rx, float ry);
    
    static MeshGeometry buildCylinderMesh(std::vector<tgt::vec3>& vertices, size_t numSides, tgt::vec3 color);
    
   /* Returns the basis, on which cylinder's caps are built.
    * This is the basis of two vectors, which define caps' planes orientation.
    */ 
    static std::vector<tgt::vec3> getCylinderCapBasis(tgt::vec3 v1, tgt::vec3 v2);

};

#endif // VRN_PRIMITIVEGEOMETRYBUILDER_H
