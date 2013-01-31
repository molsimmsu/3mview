#include "polyline.h"

PolyLine::PolyLine() {
    
}

PolyLine::PolyLine(std::vector<tgt::vec3> vertices)
  : vertices_(vertices)
{}

void PolyLine::addVertex(const tgt::vec3& vertex) {
    vertices_.push_back(vertex);
}
    
const tgt::vec3& PolyLine::getVertex(size_t index) const {
    // TODO Check boundary
    return vertices_[index];
}
    
size_t PolyLine::getVertexCount() const {
    return vertices_.size();
}
