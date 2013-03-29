#include "checklistvolumeselector.h"

const std::string ChecklistVolumeSelector::loggerCat_("3MTK.DensityMap.ChecklistVolumeSelector");

ChecklistVolumeSelector::ChecklistVolumeSelector()
    : volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
    /*, inport_(Port::INPORT, "volumecollection", "VolumeCollection Input", false)
    , outport1_(Port::OUTPORT, "volumehandle.volumehandle1", "Volume Output 1", false)
    , outport2_(Port::OUTPORT, "volumehandle.volumehandle2", "Volume Output 2", false)
    , outport3_(Port::OUTPORT, "volumehandle.volumehandle3", "Volume Output 3", false)
    , outport4_(Port::OUTPORT, "volumehandle.volumehandle4", "Volume Output 4", false)*/
{
	addProperty(volumeURLList_);
	
    /*addPort(inport_);
    
    addPort(outport1_);
    addPort(outport2_);
    addPort(outport3_);
    addPort(outport4_);
    
    outports_.push_back(&outport1_);
    outports_.push_back(&outport2_);
    outports_.push_back(&outport3_);
    outports_.push_back(&outport4_);*/
}

void ChecklistVolumeSelector::invalidate(int /*inv = INVALID_RESULT*/) {
    /*const VolumeCollection* collection = inport_.getData();
    
    if (collection == 0) {
        LERROR("Collection is NULL");
        return;
    }
    
    volumeURLList_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i));*/
}
