#include "collectionsource.h"

#include "voreen/core/processors/processorwidget.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"
using namespace voreen;

const std::string DensityMapCollectionSource::loggerCat_("voreen.core.CollectionSource");

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

DensityMapCollectionSource::~DensityMapCollectionSource() {
}

Processor* DensityMapCollectionSource::create() const {
    return new DensityMapCollectionSource();
}

void DensityMapCollectionSource::process() {
    // nothing
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
    
    Processor::invalidate(inv);
}

void DensityMapCollectionSource::setVolumeCollection(VolumeCollection* collection, bool owner) {
    volumeURLList_.clear();
    if (!collection)
        return;
    for (size_t i=0; i<collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i), owner, true);
}

VolumeCollection* DensityMapCollectionSource::getVolumeCollection() const {
    return volumeURLList_.getVolumes(false);
}

VolumeCollection* DensityMapCollectionSource::getSelectedVolumeCollection() const {
    return volumeURLList_.getVolumes(true);
}

