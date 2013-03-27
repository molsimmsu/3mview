#ifndef VRN_MultiVolumeSelector_H
#define VRN_MultiVolumeSelector_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/datastructures/volume/volume.h"

#include <vector>

namespace voreen {

class Volume;
class ProcessorWidgetFactory;

/**
 * Selects multiple volumes out of a input collection.
 */
class MultiVolumeSelector : public Processor {

public:
    MultiVolumeSelector();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MultiVolumeSelector";  }
    virtual std::string getCategory() const  { return "Input";           }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE; }
    virtual bool isUtility() const           { return true; }

    virtual void invalidate(int inv = INVALID_RESULT);

protected:
    virtual void setDescriptions() {
        setDescription("Selects multiple volumes from the input collection.");
    }

    virtual void process();
    virtual void initialize() throw (tgt::Exception);

    /// Inport for the volume collection.
    VolumeCollectionPort inport_;

    /// The volume ports the selected volumes are written to.
    VolumePort outport1_, outport2_, outport3_, outport4_;
    
    std::vector<VolumePort*> outports_;

    static const std::string loggerCat_;

private:
    void adjustToVolumeCollection();

};

} // namespace

#endif
