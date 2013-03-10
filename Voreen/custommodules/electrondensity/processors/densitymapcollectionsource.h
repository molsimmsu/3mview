#ifndef VRN_DENSITYMAPCOLLECTIONSOURCE_H
#define VRN_DENSITYMAPCOLLECTIONSOURCE_H

#include "../../geometry/processors/manipulationbase.h"

#include "voreen/core/processors/processor.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/volumeurllistproperty.h"

namespace voreen {

class Volume;

/**
 * Loads multiple volumes and provides them
 * as VolumeCollection through its outport.
 */
class VRN_CORE_API DensityMapCollectionSource : virtual public Processor, virtual public ManipulationBase {

public:
    DensityMapCollectionSource();
    virtual ~DensityMapCollectionSource();
    virtual Processor* create() const;

    virtual std::string getClassName() const  { return "DensityMapCollectionSource"; }
    virtual std::string getCategory() const   { return "Input";                  }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;        }

    virtual void invalidate(int inv = INVALID_RESULT);

    /**
     * Assigns a volume collection to this processor.
     *
     * @param owner if true, the processor takes ownership of the passed volumes
     */
    void setVolumeCollection(VolumeCollection* collection, bool owner = false);

    /**
     * Returns the currently assigned volume collection.
     */
    VolumeCollection* getVolumeCollection() const;
    
    /**
     * Returns the currently selected volume collection.
     */
    VolumeCollection* getSelectedVolumeCollection() const;
    
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);

protected:
    virtual void setDescriptions() {
        setDescription("Loads multiple electron density maps and provides them as VolumeCollection.");
    }

    virtual void process();
    virtual void initialize() throw (tgt::Exception);

    /// The volume port the loaded data set is written to.
    VolumeCollectionPort outport_;
    CoProcessorPort coProcessorOutport_;

    /// Property storing the loaded volume collection.
    VolumeURLListProperty volumeURLList_;

    static const std::string loggerCat_;
};

} // namespace

#endif
