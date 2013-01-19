#ifndef VRN_PRIMITIVEGEOMETRYBUILDER_H
#define VRN_PRIMITIVEGEOMETRYBUILDER_H

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/datastructures/geometry/meshgeometry.h"
#include "voreen/core/datastructures/geometry/facegeometry.h"
using namespace voreen;

#include "tgt/vector.h"
#include "tgt/tgt_math.h"

#include <vector>
#include <cmath>

class PrimitiveGeometryBuilder {
public:
    static MeshGeometry createCylinder(tgt::vec3 v1, tgt::vec3 v2, float radius, 
                                size_t numSides, tgt::vec4 color, bool buildCaps = false);

private:
   /* Returns the basis, on which cylinder's caps are built.
    * This is the basis of two vectors, which define caps' planes orientation.
    */ 
    static std::vector<tgt::vec3> getCylinderCapBasis(tgt::vec3 v1, tgt::vec3 v2);

};

#endif // VRN_PRIMITIVEGEOMETRYBUILDER_H
