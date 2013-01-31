#ifndef VRN_PolyLine_H
#define VRN_PolyLine_H

#include "voreen/core/io/serialization/serializable.h"
using namespace voreen;

#include "tgt/vector.h"

#include <vector>

class PolyLine : public Serializable {
public:
    PolyLine();
    PolyLine(std::vector<tgt::vec3> vertices);
    virtual ~PolyLine() {}
    
    /**
     * Returns the name of this class as a string.
     * Necessary due to the lack of code reflection in C++.
     *
     * This method is expected to be re-implemented by each concrete subclass.
     */
    virtual std::string getClassName() const { return "PolyLine"; };
    
    void addVertex(const tgt::vec3& vertex);
    
    const tgt::vec3 getVertex(size_t index) const;
    
    const tgt::vec3 getSegment(size_t index) const;
    
    const tgt::vec3 getTangent(size_t index) const;
    
    const tgt::vec3 getNormal(size_t index) const;
    
    const tgt::vec3 getBinormal(size_t index) const;
    
    size_t getVertexCount() const;
    
    size_t getSegmentCount() const;
    
    PolyLine* interpolateBezier(size_t numSegments, float tangentLength) const;
    
    /**
     * TODO Implementation of the Serializable interface.
     */
    virtual void serialize(XmlSerializer& s) const {}

    /**
     * TODO Implementation of the Serializable interface.
     */
    virtual void deserialize(XmlDeserializer& s) {}
    
private:
    std::vector<tgt::vec3> vertices_; ///< The array of vertices
};

#endif // VRN_PolyLine_H
