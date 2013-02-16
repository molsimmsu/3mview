#ifndef VRN_POINTCLOUDCHARACTERISTICS_H
#define VRN_POINTCLOUDCHARACTERISTICS_H

#include "../ports/weightedpointcloudport.h"

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/geometryport.h"
using namespace voreen;

class PointCloudCharacteristics : public Processor {
public:
    PointCloudCharacteristics();

    // virtual constructor
    virtual Processor* create() const { return new PointCloudCharacteristics(); }

    // documentary functions
    virtual std::string getClassName() const { return "PointCloudCharacteristics"; }
    virtual std::string getCategory() const  { return "Point cloud"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }

protected:
    virtual void setDescriptions() {
        setDescription("Compute the characteristics of a point cloud");
    }

    // Compute the characteristics of a point cloud
    virtual void process();

private:
    // ports and properties
    WeightedPointCloudPort inport_;
    // TODO Add outport
    
};

#endif // VRN_POINTCLOUDCHARACTERISTICS_H
