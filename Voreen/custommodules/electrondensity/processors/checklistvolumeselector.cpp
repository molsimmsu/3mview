#include "checklistvolumeselector.h"
#include <sstream>

const std::string ChecklistVolumeSelector::loggerCat_("3MTK.DensityMap.ChecklistVolumeSelector");

ChecklistVolumeSelector::ChecklistVolumeSelector()
    : volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>(), Processor::VALID)
    , updateButton_("updateButton", "Update output")
    , inport_(Port::INPORT, "volumecollection", "VolumeCollection Input", false)
    , outport1_(Port::OUTPORT, "volumehandle.volumehandle1", "Volume Output 1", false)
    , outport2_(Port::OUTPORT, "volumehandle.volumehandle2", "Volume Output 2", false)
    , outport3_(Port::OUTPORT, "volumehandle.volumehandle3", "Volume Output 3", false)
    , outport4_(Port::OUTPORT, "volumehandle.volumehandle4", "Volume Output 4", false)
{
	addProperty(volumeURLList_);
	addProperty(updateButton_);
	
    addPort(inport_);
    
    addPort(outport1_);
    addPort(outport2_);
    addPort(outport3_);
    addPort(outport4_);
    
    outports_.push_back(&outport1_);
    outports_.push_back(&outport2_);
    outports_.push_back(&outport3_);
    outports_.push_back(&outport4_);
    
    updateButton_.onChange(CallMemberAction<ChecklistVolumeSelector>(this, &ChecklistVolumeSelector::adjustToVolumeCollection));
}

void ChecklistVolumeSelector::invalidate(int inv) {
    if (inv != Processor::VALID) {
        const VolumeCollection* collection = inport_.getData();
        if (collection == 0) return;
        
        volumeURLList_.clear();
        
        for (size_t i = 0; i < collection->size(); i++)
            volumeURLList_.addVolume(collection->at(i));
            
        LINFO("Invalidation done");
    }
}

void ChecklistVolumeSelector::adjustToVolumeCollection() {

    const VolumeCollection* collection = volumeURLList_.getVolumes(true);
    if (!collection) {
        LWARNING("Input collection is 0");
        return;
    }
    
    std::stringstream info;
    info << "Setting " << collection->size() << "outports";
    LINFO(info.str());
    
    for (size_t i = 0; i < outports_.size(); i++) {
        if (collection && i < collection->size())
            outports_[i]->setData(collection->at(i), false);
        else
            outports_[i]->setData(0);
    }
}

