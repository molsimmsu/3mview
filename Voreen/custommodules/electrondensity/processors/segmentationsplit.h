#ifndef VRN_segmentationsplit_H
#define VRN_segmentationsplit_H

#include "densitymapcoprocessor.h"
#include "segmentation.h"

#include "voreen/core/ports/volumeport.h"
#include "voreen/core/properties/buttonproperty.h"
using namespace voreen;
 
class SegmentationSplit : public DensityMapCoProcessor {
public:
    SegmentationSplit();
    virtual ~SegmentationSplit() {}
    virtual Processor* create() const { return new SegmentationSplit(); }

    virtual std::string getClassName() const { return "SegmentationSplit";       }
    virtual std::string getCategory() const  { return "Segmentation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    void process() {}
    
    virtual void updateSelection() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Splitting a given segmentation into several volumes and puts them into the density map collection");
    }
    
    void splitSegmentation();
    
    static const std::string loggerCat_;

private:
    VolumePort volumePort_;
    VolumePort segmentationPort_;
    
    ButtonProperty splitButton_;

};

#endif // VRN_DENSITYMAPMANIPULATION_H
