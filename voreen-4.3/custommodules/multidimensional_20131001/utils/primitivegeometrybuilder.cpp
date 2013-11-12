#include "primitivegeometrybuilder.h"

MeshGeometry PrimitiveGeometryBuilder::createConeCylinder(tgt::vec3 v1, tgt::vec3 v2, float radius1, float radius2, size_t numSides, tgt::vec3 color, bool buildCaps) {
    // Get the basis on which the cap vertices are built
    Basis basis = getCylinderCapBasis(v1, v2);
    // Cylinder vertices
    std::vector<tgt::vec3> vertices = makeBasisVertices(v1, basis, radius1, radius1, 
                                                        v2, basis, radius2, radius2, 
                                                        numSides);
    
    return buildCylinderMesh(vertices, numSides, color);
}

MeshGeometry PrimitiveGeometryBuilder::createCylinder(tgt::vec3 v1, tgt::vec3 v2, float radius, size_t numSides, tgt::vec3 color, bool buildCaps) {
    // Get the basis on which the cap vertices are built
    Basis basis = getCylinderCapBasis(v1, v2);
    // Cylinder vertices
    std::vector<tgt::vec3> vertices = makeBasisVertices(v1, basis, radius, radius, 
                                                        v2, basis, radius, radius, 
                                                        numSides);
    
    return buildCylinderMesh(vertices, numSides, color);
}


std::vector<tgt::vec3> PrimitiveGeometryBuilder::makeBasisVertices(tgt::vec3 v1, Basis basis1, float rx1, float ry1, 
                                                                   tgt::vec3 v2, Basis basis2, float rx2, float ry2,
                                                                   size_t numSides) {
    std::vector<tgt::vec3> vertices;
    
    // Build vertices of two caps by rotating a vector in the basis plane
    for (size_t i = 0; i < numSides; i++) {
        float angle = 2 * tgt::PI * i / numSides;
        
        vertices.push_back(v1 + makeBasisVertex(angle, basis1, rx1, ry1));
        vertices.push_back(v2 + makeBasisVertex(angle, basis2, rx2, ry2));
    }
    
    // Add two first vertices to the end of the list to loop the cycle
    vertices.push_back(vertices[0]);
    vertices.push_back(vertices[1]);
    
    return vertices;
}

tgt::vec3 PrimitiveGeometryBuilder::makeBasisVertex(float angle, Basis basis, float rx, float ry) {
    // Basis coefficients
    float ca = cos(angle)*rx;
    float sa = sin(angle)*ry;
    
    // Multiply basis vectors
    tgt::vec3 a(basis[0][0]*ca, basis[0][1]*ca, basis[0][2]*ca);
    tgt::vec3 b(basis[1][0]*sa, basis[1][1]*sa, basis[1][2]*sa);
    
    // Vector from cap center to its vertex
    return a + b;
}

MeshGeometry PrimitiveGeometryBuilder::buildCylinderMesh(std::vector<tgt::vec3>& vertices, size_t numSides, tgt::vec3 color) {
    // Transform vec3 to vec4
    tgt::vec4 color4(color[0], color[1], color[2], 1.f);
    
    // Build cylinder's sides
    MeshGeometry cyl;
    for (size_t i = 0; i < 2*numSides; i+=2) {
        FaceGeometry face;
        
        tgt::vec3 faceNormal = tgt::cross(vertices[i+1] - vertices[i], vertices[i+2] - vertices[i]);
        
        // Face vertices
        // TODO Replace faceNormal with vertex normals for smoother representation
        VertexGeometry fv1(vertices[i], tgt::vec3(0.f), color4, faceNormal);
        VertexGeometry fv2(vertices[i+1], tgt::vec3(0.f), color4, faceNormal);
        VertexGeometry fv3(vertices[i+3], tgt::vec3(0.f), color4, faceNormal);
        VertexGeometry fv4(vertices[i+2], tgt::vec3(0.f), color4, faceNormal);
        face.addVertex(fv1);
        face.addVertex(fv2);
        face.addVertex(fv3);
        face.addVertex(fv4);
        
        cyl.addFace(face);
    }
    return cyl;
}

std::vector<tgt::vec3> PrimitiveGeometryBuilder::getCylinderCapBasis(tgt::vec3 v1, tgt::vec3 v2) {
    tgt::vec3 a = v2 - v1; // Cylinder axis
    tgt::vec3 b, c; // Basis vectors
    
    // Build a plane perpendicular to the axis. 
    // Plane equation is: a[0]*b[0] + a[1]*b[1] + a[2]*b[2] = 0.
    
    // Find the primary index - index with the greatest absolute value
    // to avoid division by values near zero.
    int primaryIndex;
    
    if (fabs(a[0]) >= fabs(a[1]) && fabs(a[0]) >= fabs(a[2]))
        primaryIndex = 0;
    else if (fabs(a[1]) >= fabs(a[0]) && fabs(a[1]) >= fabs(a[2]))
        primaryIndex = 1;
    else if (fabs(a[2]) >= fabs(a[0]) && fabs(a[2]) >= fabs(a[1]))
        primaryIndex = 2;
    
    // Find the first basis vector by crossing a plane with a line
    // parallel to one axis depending on primary index
    if (primaryIndex == 0) {
        b[1] = b[2] = 1;
        b[0] = -(a[1] + a[2]) / a[0];
    }
    else if (primaryIndex == 1) {
        b[0] = b[2] = 1;
        b[1] = -(a[0] + a[2]) / a[1];
    }
    else if (primaryIndex == 2) {
        b[0] = b[1] = 1;
        b[2] = -(a[0] + a[1]) / a[2];
    }
    
    // Find the second basis vector
    c = tgt::cross(a, b);
    
    // Return the basis
    std::vector<tgt::vec3> basis;
    basis.push_back(tgt::normalize(b));
    basis.push_back(tgt::normalize(c));
    
    return basis;
}



