#ifndef VRN_ELECTRONDENSITYMANIPULATION_H
#define VRN_ELECTRONDENSITYMANIPULATION_H

#include "../../geometry/processors/manipulationbase.h"

#include "../properties/volumeselectionproperty.h"
#include "densitymapcollectionsource.h"

namespace voreen {
 
class ElectronDensityManipulation : public ManipulationBase {
public:
    ElectronDensityManipulation();
    virtual ~ElectronDensityManipulation();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "ElectronDensityManipulation";       }
    virtual std::string getCategory() const  { return "Electron Density Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void invalidate(int inv = INVALID_RESULT);
    
protected:
    virtual void setDescriptions() {
        setDescription("Processor for electron density maps manipulation");
    }

private:
    DensityMapCollectionSource* getSourceProcessor() const;

    const VolumeCollection* getInputVolumeCollection() const;
    
    void updateSelection();
    
    void forceUpdate();
    
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);
    
    VolumeSelectionProperty volumeSelection_;
};

} // namespace

#endif // VRN_ELECTRONDENSITYMANIPULATION_H
