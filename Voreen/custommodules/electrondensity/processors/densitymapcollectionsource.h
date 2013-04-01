#ifndef VRN_DENSITYMAPCOLLECTIONSOURCE_H
#define VRN_DENSITYMAPCOLLECTIONSOURCE_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/volumeurllistproperty.h"
using namespace voreen;

class VRN_CORE_API DensityMapCollectionSource : public Processor {
public:
    DensityMapCollectionSource();
    virtual ~DensityMapCollectionSource() {}
    virtual Processor* create() const { return new DensityMapCollectionSource(); }

    virtual std::string getClassName() const  { return "DensityMapCollectionSource"; }
    virtual std::string getCategory() const   { return "Input";                  }
    virtual CodeState getCodeState() const    { return CODE_STATE_TESTING;        }

    virtual void invalidate(int inv = INVALID_RESULT);

    void setVolumeCollection(VolumeCollection* collection, bool owner = false);

    VolumeCollection* getVolumeCollection() const { return volumeURLList_.getVolumes(false); }
    
    VolumeCollection* getSelectedVolumeCollection() const { return volumeURLList_.getVolumes(true); }
    
    void addVolume(VolumeBase* vol, bool owner = false, bool selected = false);
    
    void invalidateOutport() {
        outport_.invalidatePort();
    }

protected:
    virtual void setDescriptions() {
        setDescription("Loads multiple electron density maps and provides them as VolumeCollection.");
    }

    virtual void process() {}
    virtual void initialize() throw (tgt::Exception);

    VolumeCollectionPort outport_;
    CoProcessorPort coProcessorOutport_;

    VolumeURLListProperty volumeURLList_;

    static const std::string loggerCat_;
};

#endif
