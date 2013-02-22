#include "volumeselectionproperty.h"

namespace voreen {

VolumeSelectionProperty::VolumeSelectionProperty(const std::string& id, const std::string& guiText, 
                   const VolumeCollection* value, int invalidationLevel)
    : TemplateProperty<const VolumeCollection*>(id, guiText, value, invalidationLevel)
{}

VolumeSelectionProperty::VolumeSelectionProperty()
{}

Property* VolumeSelectionProperty::create() const {
    return new VolumeSelectionProperty();
}

const VolumeCollection* VolumeSelectionProperty::getSelected() const {
    return &selection_;
}
    
bool VolumeSelectionProperty::isSelected(const VolumeBase* volumeHandle) const {
    return selection_.contains(volumeHandle);
}

}   // namespace

