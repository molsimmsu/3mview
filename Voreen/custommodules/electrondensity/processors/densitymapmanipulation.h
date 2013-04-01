#ifndef VRN_DENSITYMAPMANIPULATION_H
#define VRN_DENSITYMAPMANIPULATION_H

#include "densitymapcollectionsource.h"
#include "densitymapcoprocessor.h"
#include "../../geometry/processors/manipulationbase.h"

#include "voreen/core/properties/volumeurllistproperty.h"
using namespace voreen;
 
class DensityMapManipulation : virtual public ManipulationBase, virtual public DensityMapCoProcessor {
public:
    DensityMapManipulation();
    virtual ~DensityMapManipulation() {}
    virtual Processor* create() const { return new DensityMapManipulation(); }

    virtual std::string getClassName() const { return "DensityMapManipulation";       }
    virtual std::string getCategory() const  { return "Density Map Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    void process() {}
    
    virtual void updateSelection();
    
protected:
    virtual void setDescriptions() {
        setDescription("Processor for electron density maps manipulation");
    }
    
    static const std::string loggerCat_;

private:
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);
    
    VolumeURLListProperty volumeURLList_;
    bool allowInvalidation_;
};

#endif // VRN_DENSITYMAPMANIPULATION_H
