#ifndef VRN_DENSITYMAPMANIPULATION_H
#define VRN_DENSITYMAPMANIPULATION_H

#include "densitymapcollectionsource.h"
#include "densitymapcoprocessor.h"

#include "../../geometry/processors/manipulationbase.h"
#include "../properties/volumeselectionproperty.h"

namespace voreen {
 
class DensityMapManipulation : virtual public ManipulationBase, virtual public DensityMapCoProcessor {
public:
    DensityMapManipulation();
    virtual ~DensityMapManipulation();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "DensityMapManipulation";       }
    virtual std::string getCategory() const  { return "Density Map Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void invalidate(int inv = INVALID_RESULT);
    
    void process() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Processor for electron density maps manipulation");
    }

private:
    void updateSelection();
    
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);
    
    VolumeSelectionProperty volumeSelection_;
};

} // namespace

#endif // VRN_DENSITYMAPMANIPULATION_H
