#ifndef VRN_MATRIXPORT_H
#define VRN_MATRIXPORT_H

#include "tgt/matrix.h"
#include "voreen/core/ports/genericport.h"

using namespace voreen;

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<tgt::mat4>;
#endif

class VRN_CORE_API mat4Port : public GenericPort<tgt::mat4> {
public:
    mat4Port(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "mat4Port"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(255, 200, 255); }
};

#endif // VRN_MATRIXPORT_H
