#include "densitymapcollectionsource.h"

#include "voreen/core/processors/processorwidget.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"

const std::string DensityMapCollectionSource::loggerCat_("voreen.core.DensityMapCollectionSource");

DensityMapCollectionSource::DensityMapCollectionSource()
    : Processor()
    , coProcessorOutport_(Port::OUTPORT, "coProcessor", "Co-processor", true)
    , outport_(Port::OUTPORT, "volumecollection", "VolumeCollection Output", false)
    , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
{
    addPort(coProcessorOutport_);
    addPort(outport_);
    addProperty(volumeURLList_);
}

void DensityMapCollectionSource::initialize() throw (tgt::Exception) {
    Processor::initialize();

    volumeURLList_.loadVolumes(false, true);
    outport_.setData(volumeURLList_.getVolumes(true), true);

    if (getProcessorWidget())
        getProcessorWidget()->updateFromProcessor();
}

void DensityMapCollectionSource::invalidate(int inv) {
    outport_.setData(volumeURLList_.getVolumes(true), true);
    
    /*const std::vector<CoProcessorPort*>& coProcessorOutports =  getCoProcessorOutports();
    
    for (size_t i=0; i<coProcessorOutports.size(); ++i) {
        const std::vector<const Port*> getConnected() const;
        std::vector<Processor*> processors = coProcessorOutports[i]->getConnectedProcessors();
    }*/
    
    Processor::invalidate(inv);
}

void DensityMapCollectionSource::setVolumeCollection(VolumeCollection* collection, bool owner) {
    volumeURLList_.clear();
    if (!collection)
        return;
    for (size_t i=0; i<collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i), owner, true);
}
