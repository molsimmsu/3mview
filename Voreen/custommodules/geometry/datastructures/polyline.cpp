#include "polyline.h"
#include <Eigen/Dense>

PolyLine::PolyLine() {
    
}

PolyLine::PolyLine(std::vector<tgt::vec3> vertices)
  : vertices_(vertices)
{}

void PolyLine::addVertex(const tgt::vec3& vertex, float scale) {
    vertices_.push_back(vertex);
    scales_.push_back(scale);
}
    
const tgt::vec3 PolyLine::getVertex(size_t index) const {
    // TODO Check boundary
    return vertices_[index];
}

const tgt::vec3 PolyLine::getSegment(size_t index) const {
    // TODO Check boundary
    return vertices_[index+1] - vertices_[index];
}

const tgt::vec3 PolyLine::getTangent(size_t index) const {
    if (index == 0)
        return tgt::normalize(getVertex(1) - getVertex(0));
        
    if (index == getVertexCount() - 1)
        return tgt::normalize(getVertex(index) - getVertex(index-1));
        
    return tgt::normalize(tgt::normalize(getSegment(index)) + tgt::normalize(getSegment(index-1)));
}

const tgt::vec3 PolyLine::getNormal(size_t index) const {
    if (index == 0)
        return tgt::cross(getBinormal(1), getTangent(0));
        
    if (index == getVertexCount() - 1)
        return tgt::cross(getBinormal(index-1), getTangent(index));
        
    return tgt::normalize(tgt::normalize(getSegment(index)) - tgt::normalize(getSegment(index-1)));
}

const tgt::vec3 PolyLine::getBinormal(size_t index) const {
    if (index == 0)
        return getBinormal(1);
        
    if (index == getVertexCount() - 1)
        return getBinormal(index-1);
        
    return tgt::cross(getTangent(index), getNormal(index));
}

size_t PolyLine::getVertexCount() const {
    return vertices_.size();
}

size_t PolyLine::getSegmentCount() const {
    return vertices_.size() - 1;
}

PolyLine* PolyLine::interpolateBezier(size_t numSteps, float tangentLength) const {
    float dt = 1.f / numSteps;
    
    Eigen::Matrix<float,4,4> M;
    M << -1,  3, -3,  1,
          3, -6,  3,  0,
         -3,  3,  0,  0,
          1,  0,  0,  0;
          
    PolyLine* res = new PolyLine();
    
    for (size_t i = 0; i < getSegmentCount(); i++) {
        Eigen::Matrix<float,4,3> P;
        
        tgt::vec3 p1 = getVertex(i);
        tgt::vec3 p2 = getVertex(i) + tangentLength * getTangent(i);
        tgt::vec3 p3 = getVertex(i+1) - tangentLength * getTangent(i+1);
        tgt::vec3 p4 = getVertex(i+1);
        
        P << p1[0], p1[1], p1[2],
             p2[0], p2[1], p2[2],
             p3[0], p3[1], p3[2],
             p4[0], p4[1], p4[2];
             
        Eigen::Matrix<float,4,3> MP = M*P;
        
        for (size_t j = 0; j < numSteps; j++) {
            float t = dt*j;
            
            Eigen::Matrix<float,1,4> R;
            R << t*t*t, t*t, t, 1;
            
            Eigen::Matrix<float,1,3> V = R*MP;
            res->addVertex(tgt::vec3(V(0,0),V(0,1),V(0,2)));
        }
    }
    res->addVertex(getVertex(getVertexCount()-1));
    
    return res;
}
