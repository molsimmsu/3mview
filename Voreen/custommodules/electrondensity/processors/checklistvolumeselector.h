#ifndef VRN_ChecklistVolumeSelector_H
#define VRN_ChecklistVolumeSelector_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/volumeurllistproperty.h"
using namespace voreen;
 
class ChecklistVolumeSelector : public Processor {
public:
    ChecklistVolumeSelector();
    virtual ~ChecklistVolumeSelector() {}
    virtual Processor* create() const { return new ChecklistVolumeSelector(); }

    virtual std::string getClassName() const { return "ChecklistVolumeSelector";       }
    virtual std::string getCategory() const  { return "Input";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    void process() {}
    
    virtual void invalidate(int inv = INVALID_RESULT);
    
protected:
    virtual void setDescriptions() {
        setDescription("Select several volumes");
    }
    
    void adjustToVolumeCollection();
    
    /// Inport for the volume collection.
    VolumeCollectionPort inport_;

    /// The volume ports the selected volumes are written to.
    VolumePort outport1_, outport2_, outport3_, outport4_;
    
    std::vector<VolumePort*> outports_;

    static const std::string loggerCat_;

private:
    VolumeURLListProperty volumeURLList_;
    ButtonProperty        updateButton_;
};

#endif // VRN_ChecklistVolumeSelector_H
