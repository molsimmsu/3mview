#ifndef VRN_WEIGHTEDPOINTCLOUDPORT_H
#define VRN_WEIGHTEDPOINTCLOUDPORT_H

#include "../datastructures/weightedpointcloud.h"

#include "voreen/core/ports/genericport.h"
using namespace voreen;

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<WeightedPointCloud>;
#endif

class VRN_CORE_API WeightedPointCloudPort : public GenericPort<WeightedPointCloud> {
public:
    WeightedPointCloudPort(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "WeightedPointCloudPort"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(255, 128, 255); }
};

#endif // VRN_WEIGHTEDPOINTCLOUDPORT_H
