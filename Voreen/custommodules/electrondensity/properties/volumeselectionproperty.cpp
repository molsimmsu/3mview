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

}   // namespace

