#ifndef VRN_MATRIXPORT_H
#define VRN_MATRIXPORT_H

#include "tgt/matrix.h"
#include "voreen/core/ports/genericport.h"

using namespace voreen;

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<tgt::Matrix4<double> >;
#endif

class VRN_CORE_API Matrix4doublePort : public GenericPort<tgt::Matrix4<double> > {
public:
    Matrix4doublePort(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "Matrix4doublePort"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(255, 128, 255); }
};

#endif // VRN_MATRIXPORT_H
