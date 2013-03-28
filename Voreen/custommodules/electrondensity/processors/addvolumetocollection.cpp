#include "addvolumetocollection.h"

const std::string AddVolumeToCollection::loggerCat_("3MTK.DensityMap.AddVolumeToCollection");

AddVolumeToCollection::AddVolumeToCollection()
    : volumePort_(Port::INPORT, "volumePort", "Volume Inport")
    , addButton_("addButton", "Add volume")
{
    addPort(volumePort_);
    addProperty(addButton_);
    
    addButton_.onChange(CallMemberAction<AddVolumeToCollection>(this, &AddVolumeToCollection::addVolume));
}

void AddVolumeToCollection::addVolume() {
    LINFO("addVolume() start");
    
	if (volumePort_.getData() == 0) {
		LWARNING("Inport data is 0");
		return;
	}

    const VolumeBase* volume = volumePort_.getData();
    LINFO("Input volume origin:");
	LINFO(volume->getOrigin().getURL());
    
    VolumeBase* outputVolume =
        new VolumeDecoratorReplaceOffset(volume, volume->getOffset());
    
	outputVolume->setOrigin(volume->getOrigin());
	LINFO("Output volume origin:");
	LINFO(outputVolume->getOrigin().getURL());
	getSourceProcessor()->addVolume(outputVolume, true, true);
	
	LINFO("addVolume() finished");
}
