#include "volumeselectionproperty.h"

namespace voreen {

std::string VolumeSelectionProperty::loggerCat_("VolumeSelectionProperty");

VolumeSelectionProperty::VolumeSelectionProperty(const std::string& id, const std::string& guiText, 
                   const VolumeCollection* inputVolumes, int invalidationLevel)
    : TemplateProperty<const VolumeCollection*>(id, guiText, 0, invalidationLevel)
{
    if (!inputVolumes_) inputVolumes_ = new VolumeCollection();
    selectedVolumes_ = new VolumeCollection();
}

VolumeSelectionProperty::VolumeSelectionProperty() {
    inputVolumes_ = new VolumeCollection();
    selectedVolumes_ = new VolumeCollection();
}

Property* VolumeSelectionProperty::create() const {
    return new VolumeSelectionProperty();
}

const VolumeCollection* VolumeSelectionProperty::getInputVolumes() const {
    return inputVolumes_;
}

void VolumeSelectionProperty::setInputVolumes(const VolumeCollection* inputVolumes) {
    inputVolumes_ = inputVolumes;
    updateWidgets();
}

VolumeCollection* VolumeSelectionProperty::getSelectedVolumes() const {
    return selectedVolumes_;
}

void VolumeSelectionProperty::setSelectedVolumes(VolumeCollection* selectedVolumes) {
    selectedVolumes_ = selectedVolumes;
    updateWidgets();
}
    
bool VolumeSelectionProperty::isSelected(const VolumeBase* volumeHandle) const {
    return selectedVolumes_->contains(volumeHandle);
}

void VolumeSelectionProperty::setSelected(VolumeBase* volumeHandle, bool selected) {
    if (!getInputVolumes()->contains(volumeHandle)) {
        LWARNING("setSelected(): passed URL not contained by this property");
        return;
    }
	
	if (!isSelected(volumeHandle) && selected) {
		selectedVolumes_->add(volumeHandle);
		invalidate();
	}
	else if (isSelected(volumeHandle) && !selected) {
		selectedVolumes_->remove(volumeHandle);
		invalidate();
	}
}

void VolumeSelectionProperty::setAllSelected(bool selected) {
    const VolumeCollection* allVolumes = getInputVolumes();
    for (size_t i = 0; i < allVolumes->size(); i++)
        setSelected(allVolumes->at(i), selected);
}

}   // namespace

