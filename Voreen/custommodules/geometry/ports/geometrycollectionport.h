#ifndef VRN_GeometryCOLLECTIONPORT_H
#define VRN_GeometryCOLLECTIONPORT_H

#include "voreen/core/ports/genericport.h"
#include "voreen/core/datastructures/geometry/geometry.h"
using namespace voreen;

#include <vector>

typedef std::vector<Geometry*> GeometryCollection;

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<GeometryCollection>;
#endif

class VRN_CORE_API GeometryCollectionPort : public GenericPort<GeometryCollection> {
public:
    GeometryCollectionPort(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "GeometryCollectionPort"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(0, 128, 255); }
};

} // namespace

#endif // VRN_GeometryCOLLECTIONPORT_H
