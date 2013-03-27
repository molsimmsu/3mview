#include "multivolumeselector.h"

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/ports/allports.h"

namespace voreen {

const std::string MultiVolumeSelector::loggerCat_("voreen.core.MultiVolumeSelector");

MultiVolumeSelector::MultiVolumeSelector()
    : Processor()
    , inport_(Port::INPORT, "volumecollection", "VolumeCollection Input", false)
    , outport1_(Port::OUTPORT, "volumehandle.volumehandle1", "Volume Output 1", false)
    , outport2_(Port::OUTPORT, "volumehandle.volumehandle2", "Volume Output 2", false)
    , outport3_(Port::OUTPORT, "volumehandle.volumehandle3", "Volume Output 3", false)
    , outport4_(Port::OUTPORT, "volumehandle.volumehandle4", "Volume Output 4", false)
{
    addPort(inport_);
    
    addPort(outport1_);
    addPort(outport2_);
    addPort(outport3_);
    addPort(outport4_);
    
    outports_.push_back(&outport1_);
    outports_.push_back(&outport2_);
    outports_.push_back(&outport3_);
    outports_.push_back(&outport4_);
}

Processor* MultiVolumeSelector::create() const {
    return new MultiVolumeSelector();
}

void MultiVolumeSelector::process() {
    // nothing
}

void MultiVolumeSelector::initialize() throw (tgt::Exception) {
    Processor::initialize();

    adjustToVolumeCollection();
}

void MultiVolumeSelector::invalidate(int /*inv = INVALID_RESULT*/) {
    adjustToVolumeCollection();
}

void MultiVolumeSelector::adjustToVolumeCollection() {
    //if (!outport_.isInitialized())
        //return;

    const VolumeCollection* collection = inport_.getData();
    
    for (size_t i = 0; i < outports_.size(); i++) {
        if (collection && i < collection->size())
            outports_[i]->setData(collection->at(i), false);
        else
            outports_[i]->setData(0);
    }
}

} // namespace
